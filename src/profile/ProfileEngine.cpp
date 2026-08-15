#include "profile/ProfileEngine.h"

#include "profile/CanonicalJson.h"
#include "profile/Sha256.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <regex>
#include <set>
#include <tuple>

namespace sdrcal::profile {
namespace {

bool finite(double value) {
    return std::isfinite(value);
}
bool nonempty(const std::string& value) {
    return !value.empty();
}
using TimestampKey = std::pair<std::int64_t, std::string>;
std::optional<TimestampKey> timestampKey(const std::string& value) {
    static const std::regex expression(
        R"(^([0-9]{4})-(0[1-9]|1[0-2])-([0-2][0-9]|3[01])T([01][0-9]|2[0-3]):([0-5][0-9]):([0-5][0-9])(\.([0-9]+))?(Z|[+-]([01][0-9]|2[0-3]):([0-5][0-9]))$)");
    std::smatch match;
    if (!std::regex_match(value, match, expression))
        return std::nullopt;
    const int year = std::stoi(value.substr(0, 4));
    const unsigned month = static_cast<unsigned>(std::stoi(value.substr(5, 2)));
    const unsigned day = static_cast<unsigned>(std::stoi(value.substr(8, 2)));
    const auto date = std::chrono::year_month_day{std::chrono::year{year},
                                                  std::chrono::month{month}, std::chrono::day{day}};
    if (!date.ok())
        return std::nullopt;
    const auto days = std::chrono::sys_days(date).time_since_epoch().count();
    std::int64_t seconds = days * 86'400 + std::stoll(match[4]) * 3'600 +
                           std::stoll(match[5]) * 60 + std::stoll(match[6]);
    if (match[9] != "Z") {
        const std::int64_t offset = std::stoll(match[10]) * 3'600 + std::stoll(match[11]) * 60;
        seconds += match[9].str().front() == '+' ? -offset : offset;
    }
    std::string fraction = match[8].matched ? match[8].str() : std::string{};
    while (!fraction.empty() && fraction.back() == '0')
        fraction.pop_back();
    return TimestampKey{seconds, fraction};
}
bool timestamp(const std::string& value) {
    return timestampKey(value).has_value();
}
template <typename T> bool unique(const std::vector<T>& values) {
    return std::set<T>(values.begin(), values.end()).size() == values.size();
}
void issue(ValidationResult& result, ValidationCode code, std::string path, std::string message) {
    result.issues.push_back({code, std::move(path), std::move(message)});
}
std::string payload(const CalibrationProfile& profile) {
    return serializeProfile(profile, false);
}
bool sameIdentity(const DeviceIdentity& a, const DeviceIdentity& b) {
    return a.driver == b.driver && a.manufacturer == b.manufacturer && a.model == b.model &&
           a.identifier == b.identifier && a.strength == b.strength;
}
bool sameConfiguration(const DeviceConfiguration& a, const DeviceConfiguration& b) {
    return a.clock_source == b.clock_source && a.sample_rate_hz == b.sample_rate_hz &&
           a.bandwidth_hz == b.bandwidth_hz &&
           a.frequency_correction_ppm == b.frequency_correction_ppm &&
           a.driver_version == b.driver_version && a.firmware_version == b.firmware_version &&
           a.antenna_port == b.antenna_port && a.tuner_path == b.tuner_path &&
           canonicalizeJson(a.binding_extension).value ==
               canonicalizeJson(b.binding_extension).value;
}
EvaluationResult failed(EvaluationStatus status, std::string reason,
                        const CalibrationProfile& profile) {
    EvaluationResult result;
    result.status = status;
    result.reason = std::move(reason);
    result.profile_id = profile.profile_id;
    return result;
}

} // namespace

CompatibilityResult checkCompatibility(const CalibrationProfile& profile) {
    if (profile.schema_name != "sdr-calibration-profile")
        return {CompatibilityStatus::unsupported_schema, "unsupported schema name"};
    static const std::regex semantic_version(R"(^([0-9]+)\.([0-9]+)\.([0-9]+)$)");
    std::smatch match;
    if (!std::regex_match(profile.schema_version, match, semantic_version) || match[1] != "1")
        return {CompatibilityStatus::unsupported_schema, "unsupported schema major version"};
    if (!profile.required_capabilities.empty())
        return {CompatibilityStatus::unsupported_required_capability,
                "unknown required capability"};
    return {CompatibilityStatus::supported, "schema major version 1 is supported"};
}

ValidationResult validateProfile(const CalibrationProfile& p, bool verify_digest) {
    ValidationResult r;
    const auto compatibility = checkCompatibility(p);
    if (compatibility.status != CompatibilityStatus::supported)
        issue(r,
              compatibility.status == CompatibilityStatus::unsupported_schema
                  ? ValidationCode::unsupported_schema
                  : ValidationCode::unsupported_capability,
              "schema", compatibility.reason);
    if (p.schema_version == "1.0.0" && !p.optional_extensions.empty())
        issue(r, ValidationCode::unsupported_capability, "optional_extensions",
              "schema 1.0.0 rejects unknown top-level members");
    const auto require = [&r](bool condition, std::string path) {
        if (!condition)
            issue(r, ValidationCode::empty_value, std::move(path), "required value is empty");
    };
    require(nonempty(p.profile_id), "profile_id");
    require(timestamp(p.created_at), "created_at");
    require(nonempty(p.device.driver), "device.driver");
    require(nonempty(p.device.manufacturer), "device.manufacturer");
    require(nonempty(p.device.model), "device.model");
    require(nonempty(p.device.identifier), "device.identifier");
    require(nonempty(p.configuration.clock_source), "configuration.clock_source");
    if (p.configuration.sample_rate_hz < 1 ||
        (p.configuration.bandwidth_hz && *p.configuration.bandwidth_hz < 1) ||
        !finite(p.configuration.frequency_correction_ppm))
        issue(r, ValidationCode::invalid_value, "configuration",
              "configuration values must be finite and positive where required");
    if (p.device.strength == IdentityStrength::insufficient &&
        p.profile_status != ProfileStatus::informational_only &&
        p.profile_status != ProfileStatus::revoked)
        issue(r, ValidationCode::invalid_value, "profile_status",
              "insufficient identity is informational only");
    if (p.segments.empty())
        issue(r, ValidationCode::empty_value, "frequency_model.segments",
              "at least one segment is required");
    std::set<std::string> segment_ids;
    double previous_maximum = -1.0;
    for (std::size_t i = 0; i < p.segments.size(); ++i) {
        const auto& s = p.segments[i];
        const auto base = "frequency_model.segments[" + std::to_string(i) + "]";
        if (!nonempty(s.segment_id) || !segment_ids.insert(s.segment_id).second)
            issue(r, ValidationCode::duplicate_value, base + ".segment_id",
                  "segment identifier must be non-empty and unique");
        if (!finite(s.minimum_frequency_hz) || !finite(s.maximum_frequency_hz) ||
            s.minimum_frequency_hz < 0.0 || s.maximum_frequency_hz <= s.minimum_frequency_hz)
            issue(r, ValidationCode::invalid_value, base, "segment frequency bounds are invalid");
        if (i != 0U && s.minimum_frequency_hz <= previous_maximum)
            issue(r, ValidationCode::overlapping_segments, base,
                  "segments must be ordered and non-overlapping");
        previous_maximum = s.maximum_frequency_hz;
        if ((s.model_type == SegmentModelType::constant_ppm && !finite(s.error_ppm)) ||
            (s.model_type == SegmentModelType::linear &&
             (!finite(s.reference_frequency_hz) || s.reference_frequency_hz < 0.0 ||
              !finite(s.intercept_error_hz) || !finite(s.slope_ppm))))
            issue(r, ValidationCode::invalid_value, base + ".model", "model values are invalid");
        const auto& u = s.uncertainty;
        if (!finite(u.coverage_factor) || u.coverage_factor <= 0.0 || !finite(u.base_hz) ||
            u.base_hz < 0.0 || !finite(u.ppm_component) || u.ppm_component < 0.0 ||
            u.included_components.empty() || !unique(u.included_components) ||
            !unique(u.excluded_components) ||
            (u.confidence_level_approximate &&
             (!finite(*u.confidence_level_approximate) || *u.confidence_level_approximate <= 0.0 ||
              *u.confidence_level_approximate > 1.0)))
            issue(r, ValidationCode::invalid_value, base + ".uncertainty",
                  "uncertainty model is invalid");
        if (u.kind == UncertaintyKind::standard && u.coverage_factor != 1.0)
            issue(r, ValidationCode::invalid_value, base + ".uncertainty.coverage_factor",
                  "standard uncertainty requires coverage factor 1");
    }
    std::vector<core::AssuranceComponent> components;
    std::set<std::string> component_names;
    for (const auto& c : p.assurance.components) {
        if (!nonempty(c.name) || !nonempty(c.basis) || !component_names.insert(c.name).second)
            issue(r, ValidationCode::duplicate_value, "assurance.components",
                  "component names must be unique and values non-empty");
        components.push_back({c.name, c.score});
    }
    const auto calculated = core::calculateReliabilityQuotient(
        p.assurance.reference_class, components, p.assurance.scoring_policy_version);
    if (!calculated.succeeded() || calculated.quotient != p.assurance.reliability_quotient ||
        calculated.reference_ceiling != p.assurance.reference_score_ceiling)
        issue(r, ValidationCode::assurance_mismatch, "assurance",
              "reliability quotient or reference ceiling does not match policy");
    if (p.profile_status == ProfileStatus::qualification_capable &&
        p.assurance.reliability_quotient < 90)
        issue(r, ValidationCode::assurance_mismatch, "profile_status",
              "qualification-capable status requires reliability quotient of at least 90");
    std::set<std::string> expected_limiting;
    for (const auto& c : p.assurance.components)
        if (c.score == p.assurance.reliability_quotient)
            expected_limiting.insert(c.name);
    if (p.assurance.reference_score_ceiling == p.assurance.reliability_quotient)
        expected_limiting.insert("reference_class_ceiling");
    if (std::set<std::string>(p.assurance.limiting_components.begin(),
                              p.assurance.limiting_components.end()) != expected_limiting)
        issue(r, ValidationCode::assurance_mismatch, "assurance.limiting_components",
              "limiting components do not match quotient");
    require(timestamp(p.validity.calibrated_at), "validity.calibrated_at");
    require(timestamp(p.validity.not_valid_after), "validity.not_valid_after");
    const auto calibrated_key = timestampKey(p.validity.calibrated_at);
    const auto expiration_key = timestampKey(p.validity.not_valid_after);
    if (calibrated_key && expiration_key && *calibrated_key >= *expiration_key)
        issue(r, ValidationCode::invalid_value, "validity",
              "expiration must follow calibration time");
    if (p.validity.minimum_warmup_seconds < 0 || !finite(p.validity.temperature.reference_c) ||
        !finite(p.validity.temperature.minimum_c) || !finite(p.validity.temperature.maximum_c) ||
        p.validity.temperature.minimum_c > p.validity.temperature.reference_c ||
        p.validity.temperature.reference_c > p.validity.temperature.maximum_c ||
        !nonempty(p.validity.temperature.measurement_location))
        issue(r, ValidationCode::invalid_value, "validity", "validity environment is invalid");
    require(nonempty(p.provenance.calibration_run_id), "provenance.calibration_run_id");
    require(nonempty(p.provenance.software.name), "provenance.software.name");
    require(nonempty(p.provenance.software.version), "provenance.software.version");
    if (p.provenance.reference_set.empty() || p.provenance.observation_ids.empty() ||
        !unique(p.provenance.observation_ids))
        issue(r, ValidationCode::invalid_value, "provenance",
              "references and unique observations are required");
    std::set<std::string> reference_ids;
    int weakest_reference_ceiling = 100;
    for (const auto& reference : p.provenance.reference_set) {
        weakest_reference_ceiling =
            std::min(weakest_reference_ceiling, reference.assurance_score_ceiling);
        if (!nonempty(reference.reference_id) ||
            !reference_ids.insert(reference.reference_id).second ||
            !finite(reference.nominal_frequency_hz) || reference.nominal_frequency_hz < 0.0 ||
            reference.assurance_score_ceiling != core::referenceAssuranceCeiling(reference.kind) ||
            !isSha256Hex(reference.evidence_sha256) ||
            (reference.frequency_uncertainty_hz && (!finite(*reference.frequency_uncertainty_hz) ||
                                                    *reference.frequency_uncertainty_hz < 0.0)))
            issue(r, ValidationCode::invalid_value, "provenance.reference_set",
                  "reference record is invalid");
    }
    if (!p.provenance.reference_set.empty() &&
        weakest_reference_ceiling != p.assurance.reference_score_ceiling)
        issue(r, ValidationCode::assurance_mismatch, "assurance.reference_score_ceiling",
              "profile reference class must represent the weakest reference record");
    if (p.provenance.reference_registry &&
        (p.provenance.reference_registry->schema_version < 1 ||
         p.provenance.reference_registry->registry_version.empty() ||
         !isSha256Hex(p.provenance.reference_registry->sha256)))
        issue(r, ValidationCode::invalid_value, "provenance.reference_registry",
              "registry record is invalid");
    if (p.provenance.evidence_bundle && (p.provenance.evidence_bundle->media_type.empty() ||
                                         !isSha256Hex(p.provenance.evidence_bundle->sha256) ||
                                         (p.provenance.evidence_bundle->location &&
                                          p.provenance.evidence_bundle->location->empty())))
        issue(r, ValidationCode::invalid_value, "provenance.evidence_bundle",
              "evidence bundle record is invalid");
    if (p.supersedes_profile_id && *p.supersedes_profile_id == p.profile_id)
        issue(r, ValidationCode::invalid_revocation, "supersedes_profile_id",
              "profile cannot supersede itself");
    for (const auto& a : p.adapters) {
        const bool recognized =
            a.adapter_id == "wsjt-x-settings" || a.adapter_id == "wsjt-x-fmt-all";
        if (!recognized || (a.direction != "import" && a.direction != "export") ||
            a.application != "WSJT-X" || a.application_version.empty() ||
            !timestamp(a.created_at) || !isSha256Hex(a.artifact_sha256) ||
            (a.adapter_id == "wsjt-x-fmt-all" && a.direction != "import") ||
            (a.adapter_id == "wsjt-x-settings" && a.direction == "export" &&
             (!a.lossy || a.omitted_capabilities.empty() || !a.compatibility)))
            issue(r, ValidationCode::invalid_adapter, "interoperability.adapters",
                  "adapter record violates schema contract");
    }
    if (p.integrity.canonicalization != "RFC8785" || !isSha256Hex(p.integrity.sha256) ||
        (p.integrity.signature &&
         (p.integrity.signature->algorithm != "ed25519" || p.integrity.signature->key_id.empty() ||
          p.integrity.signature->value.empty())))
        issue(r, ValidationCode::invalid_integrity, "integrity", "integrity record is invalid");
    if (verify_digest && isSha256Hex(p.integrity.sha256) &&
        sha256Hex(payload(p)) != p.integrity.sha256)
        issue(r, ValidationCode::invalid_integrity, "integrity.sha256",
              "digest does not match canonical payload");
    return r;
}

bool refreshIntegrity(CalibrationProfile& profile, std::string* error) {
    const auto canonical = payload(profile);
    if (canonical.empty()) {
        if (error)
            *error = "profile cannot be canonicalized";
        return false;
    }
    profile.integrity.canonicalization = "RFC8785";
    profile.integrity.sha256 = sha256Hex(canonical);
    profile.integrity.signature.reset();
    return true;
}

bool signProfile(CalibrationProfile& profile, std::string key_id, const SignHook& signer,
                 std::string* error) {
    if (!signer || key_id.empty()) {
        if (error)
            *error = "signer and key identifier are required";
        return false;
    }
    auto candidate = profile;
    if (!refreshIntegrity(candidate, error))
        return false;
    const auto canonical = payload(candidate);
    const auto signature = signer(canonical, key_id);
    if (!signature || signature->empty()) {
        if (error)
            *error = "signing hook failed";
        return false;
    }
    candidate.integrity.signature = SignatureRecord{"ed25519", std::move(key_id), *signature};
    profile = std::move(candidate);
    return true;
}

bool verifyIntegrity(const CalibrationProfile& profile, const VerifyHook& verifier,
                     bool require_signature, std::string* error) {
    const auto canonical = payload(profile);
    if (canonical.empty() || !isSha256Hex(profile.integrity.sha256) ||
        sha256Hex(canonical) != profile.integrity.sha256) {
        if (error)
            *error = "SHA-256 verification failed";
        return false;
    }
    if (!profile.integrity.signature) {
        if (require_signature) {
            if (error)
                *error = "verified signature is required";
            return false;
        }
        return true;
    }
    if (!verifier || profile.integrity.signature->algorithm != "ed25519" ||
        !verifier(canonical, profile.integrity.signature->key_id,
                  profile.integrity.signature->value)) {
        if (error)
            *error = "Ed25519 signature verification failed";
        return false;
    }
    return true;
}

EvaluationResult evaluateProfile(const CalibrationProfile& p, const EvaluationRequest& q,
                                 const VerifyHook& verifier) {
    const auto compatibility = checkCompatibility(p);
    if (compatibility.status != CompatibilityStatus::supported)
        return failed(EvaluationStatus::unsupported_schema, compatibility.reason, p);
    if (!validateProfile(p, false).valid())
        return failed(EvaluationStatus::invalid_request, "profile semantic validation failed", p);
    std::string integrity_error;
    if (!verifyIntegrity(p, verifier, q.require_verified_signature, &integrity_error))
        return failed(EvaluationStatus::integrity_failure, integrity_error, p);
    if (p.profile_status == ProfileStatus::revoked)
        return failed(EvaluationStatus::revoked, "profile is revoked", p);
    if (!finite(q.indicated_frequency_hz) || q.indicated_frequency_hz < 0.0 ||
        !finite(q.temperature_c) || q.warmup_seconds < 0 || !timestamp(q.evaluated_at) ||
        q.required_reliability_quotient < 0 || q.required_reliability_quotient > 100 ||
        (q.target_frequency_hz && !finite(*q.target_frequency_hz)) ||
        (q.maximum_expanded_uncertainty_hz &&
         (!finite(*q.maximum_expanded_uncertainty_hz) || *q.maximum_expanded_uncertainty_hz < 0.0)))
        return failed(EvaluationStatus::invalid_request, "evaluation request is invalid", p);
    const auto evaluated_key = timestampKey(q.evaluated_at);
    const auto calibrated_key = timestampKey(p.validity.calibrated_at);
    const auto expiration_key = timestampKey(p.validity.not_valid_after);
    if (evaluated_key && calibrated_key && *evaluated_key < *calibrated_key)
        return failed(EvaluationStatus::invalid_request, "evaluation predates calibration", p);
    if (evaluated_key && expiration_key && *evaluated_key >= *expiration_key)
        return failed(EvaluationStatus::profile_expired, "profile has expired", p);
    if (!sameIdentity(p.device, q.device))
        return failed(EvaluationStatus::identity_mismatch, "device identity does not match", p);
    if (!sameConfiguration(p.configuration, q.configuration))
        return failed(EvaluationStatus::configuration_mismatch,
                      "binding configuration does not match", p);
    if (q.temperature_c < p.validity.temperature.minimum_c ||
        q.temperature_c > p.validity.temperature.maximum_c)
        return failed(EvaluationStatus::outside_temperature_domain,
                      "temperature is outside validity domain", p);
    if (q.warmup_seconds < p.validity.minimum_warmup_seconds)
        return failed(EvaluationStatus::insufficient_warmup, "minimum warm-up is not satisfied", p);
    const auto segment = std::find_if(p.segments.begin(), p.segments.end(), [&q](const auto& s) {
        return q.indicated_frequency_hz >= s.minimum_frequency_hz &&
               q.indicated_frequency_hz <= s.maximum_frequency_hz;
    });
    if (segment == p.segments.end())
        return failed(EvaluationStatus::outside_frequency_domain,
                      "frequency is outside every segment", p);
    if (p.assurance.reliability_quotient < q.required_reliability_quotient)
        return failed(EvaluationStatus::assurance_requirement_not_met,
                      "reliability quotient is below requirement", p);
    double error_hz = 0.0;
    if (segment->model_type == SegmentModelType::constant_ppm)
        error_hz = segment->error_ppm * q.indicated_frequency_hz / 1'000'000.0;
    else if (segment->model_type == SegmentModelType::linear)
        error_hz = segment->intercept_error_hz +
                   segment->slope_ppm *
                       (q.indicated_frequency_hz - segment->reference_frequency_hz) / 1'000'000.0;
    else
        return failed(EvaluationStatus::unsupported_model, "segment model is unsupported", p);
    double uncertainty = segment->uncertainty.base_hz + segment->uncertainty.ppm_component *
                                                            q.indicated_frequency_hz / 1'000'000.0;
    if (segment->uncertainty.kind == UncertaintyKind::standard)
        uncertainty *= segment->uncertainty.coverage_factor;
    if (q.maximum_expanded_uncertainty_hz && uncertainty > *q.maximum_expanded_uncertainty_hz)
        return failed(EvaluationStatus::uncertainty_requirement_not_met,
                      "expanded uncertainty exceeds requirement", p);
    EvaluationResult result;
    result.profile_id = p.profile_id;
    result.segment_id = segment->segment_id;
    result.indicated_error_hz = error_hz;
    result.estimated_true_frequency_hz = q.indicated_frequency_hz - error_hz;
    result.expanded_uncertainty_hz = uncertainty;
    if (q.target_frequency_hz)
        result.target_offset_hz = result.estimated_true_frequency_hz - *q.target_frequency_hz;
    switch (p.profile_status) {
    case ProfileStatus::qualification_capable:
        result.status = EvaluationStatus::qualification_capable;
        break;
    case ProfileStatus::calibrated:
        result.status = EvaluationStatus::calibrated;
        break;
    case ProfileStatus::informational_only:
        result.status = EvaluationStatus::informational_only;
        break;
    case ProfileStatus::revoked:
        result.status = EvaluationStatus::revoked;
        break;
    }
    result.reason = "profile evaluated within its validated domain";
    return result;
}

} // namespace sdrcal::profile
