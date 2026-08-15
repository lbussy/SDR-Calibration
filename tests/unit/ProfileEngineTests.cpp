#include "profile/CanonicalJson.h"
#include "profile/ProfileEngine.h"
#include "profile/ProfileStorage.h"
#include "profile/Sha256.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

using namespace sdrcal::core;
using namespace sdrcal::profile;

namespace {

class TestFailure : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};
#define CHECK(condition)                                                                           \
    do {                                                                                           \
        if (!(condition))                                                                          \
            throw TestFailure(std::string("check failed: ") + #condition);                         \
    } while (false)

std::vector<AssuranceComponentRecord> assurance() {
    return {{"reference_provenance", 92, "traceability"},
            {"received_signal_suitability", 95, "accepted"},
            {"device_binding", 96, "serial"},
            {"environmental_validity", 94, "measured"},
            {"observation_quality", 93, "accepted"},
            {"model_quality", 97, "fit"},
            {"artifact_integrity", 98, "digest"},
            {"evidence_completeness", 92, "bundle"}};
}

CalibrationProfile validProfile() {
    CalibrationProfile p;
    p.profile_id = "profile-1";
    p.profile_status = ProfileStatus::qualification_capable;
    p.created_at = "2026-08-15T12:00:00Z";
    p.device = {"airspyhf", "Airspy", "HF+ Discovery", "ABC", IdentityStrength::hardware_serial};
    p.configuration.clock_source = "internal";
    p.configuration.sample_rate_hz = 768000;
    p.configuration.bandwidth_hz = 200000;
    p.configuration.frequency_correction_ppm = 0.0;
    FrequencySegment s;
    s.segment_id = "seg-1";
    s.minimum_frequency_hz = 9'000'000.0;
    s.maximum_frequency_hz = 11'000'000.0;
    s.model_type = SegmentModelType::linear;
    s.reference_frequency_hz = 10'000'000.0;
    s.intercept_error_hz = 6.0;
    s.slope_ppm = 2.0;
    s.uncertainty.kind = UncertaintyKind::expanded;
    s.uncertainty.coverage_factor = 2.0;
    s.uncertainty.base_hz = 1.0;
    s.uncertainty.ppm_component = 0.1;
    s.uncertainty.included_components = {"reference", "estimator"};
    p.segments = {s};
    p.assurance.reference_class = ReferenceClass::authority_confirmed;
    p.assurance.reference_score_ceiling = 100;
    p.assurance.reliability_quotient = 92;
    p.assurance.limiting_components = {"evidence_completeness", "reference_provenance"};
    p.assurance.components = assurance();
    p.validity = {
        "2026-08-15T11:00:00Z", "2027-08-15T11:00:00Z", 1200, {20.0, 10.0, 30.0, "SDR enclosure"}};
    ReferenceRecord reference;
    reference.reference_id = "ref-1";
    reference.kind = ReferenceClass::authority_confirmed;
    reference.nominal_frequency_hz = 10'000'000.0;
    reference.assurance_score_ceiling = 100;
    reference.evidence_sha256 = std::string(64, 'a');
    p.provenance.calibration_run_id = "run-1";
    p.provenance.software = {"SDR Calibration", "0.1.0"};
    p.provenance.reference_set = {reference};
    p.provenance.observation_ids = {"obs-1", "obs-2"};
    CHECK(refreshIntegrity(p));
    return p;
}

EvaluationRequest validRequest(const CalibrationProfile& p) {
    EvaluationRequest q;
    q.indicated_frequency_hz = 10'500'000.0;
    q.target_frequency_hz = 10'499'993.0;
    q.device = p.device;
    q.configuration = p.configuration;
    q.temperature_c = 20.0;
    q.warmup_seconds = 1200;
    q.evaluated_at = "2026-08-15T13:00:00Z";
    q.maximum_expanded_uncertainty_hz = 3.0;
    return q;
}

void testCanonicalJsonAndSha256() {
    CHECK(sha256Hex("") == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    CHECK(sha256Hex("abc") == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
    JsonObject object{
        {"z", -0.0}, {"small", 1e-7}, {"fixed", 1e-6}, {"big", 1e21}, {"text", "line\nquote\""}};
    const auto json = canonicalizeJson(object);
    CHECK(json.success);
    CHECK(
        json.value ==
        "{\"big\":1e+21,\"fixed\":0.000001,\"small\":1e-7,\"text\":\"line\\nquote\\\"\",\"z\":0}");
    const auto rfc_numbers = canonicalizeJson(
        JsonArray{333333333.33333329, 1e30, 4.50, 2e-3, 0.000000000000000000000000001});
    CHECK(rfc_numbers.success);
    CHECK(rfc_numbers.value == "[333333333.3333333,1e+30,4.5,0.002,1e-27]");
    JsonObject ordered{{"\xF0\x90\x80\x80", 1}, {"\xEE\x80\x80", 2}};
    const auto ordered_json = canonicalizeJson(ordered);
    CHECK(ordered_json.success);
    CHECK(ordered_json.value.find("\xF0\x90\x80\x80") < ordered_json.value.find("\xEE\x80\x80"));
    CHECK(!canonicalizeJson(std::numeric_limits<double>::infinity()).success);
    CHECK(!canonicalizeJson(std::int64_t{9'007'199'254'740'992LL}).success);
    const auto parsed = parseJson("{\"escaped\":\"\\ud800\\udc00\",\"n\":1e-7}");
    CHECK(parsed.success);
    CHECK(canonicalizeJson(parsed.value).value == "{\"escaped\":\"\xF0\x90\x80\x80\",\"n\":1e-7}");
    CHECK(!parseJson("{\"duplicate\":1,\"duplicate\":2}").success);
    CHECK(!parseJson("\"\\ud800\"").success);
    CHECK(!parseJson("true trailing").success);
}

void testValidationIntegrityAndSignatureHooks() {
    auto p = validProfile();
    CHECK(validateProfile(p).valid());
    const auto digest = p.integrity.sha256;
    p.integrity.sha256[0] = p.integrity.sha256[0] == '0' ? '1' : '0';
    CHECK(!validateProfile(p).valid());
    p.integrity.sha256 = digest;
    CHECK(signProfile(p, "key-1", [](std::string_view bytes, std::string_view key) {
        return std::optional<std::string>(sha256Hex(std::string(bytes) + std::string(key)));
    }));
    const auto verifier = [](std::string_view bytes, std::string_view key,
                             std::string_view signature) {
        return signature == sha256Hex(std::string(bytes) + std::string(key));
    };
    std::string error;
    CHECK(verifyIntegrity(p, verifier, true, &error));
    p.integrity.signature->value[0] = 'f';
    CHECK(!verifyIntegrity(p, verifier, true, &error));
    p.integrity.signature.reset();
    CHECK(!verifyIntegrity(p, verifier, true, &error));
    const auto before_failure = serializeProfile(p);
    CHECK(!signProfile(p, "key-1", [](std::string_view, std::string_view) {
        return std::optional<std::string>{};
    }));
    CHECK(serializeProfile(p) == before_failure);
}

void testSemanticFailures() {
    auto p = validProfile();
    p.segments.push_back(p.segments.front());
    p.segments.back().segment_id = "seg-2";
    CHECK(!validateProfile(p, false).valid());
    p = validProfile();
    p.assurance.reliability_quotient = 93;
    CHECK(!validateProfile(p, false).valid());
    p = validProfile();
    p.schema_version = "2.0.0";
    CHECK(checkCompatibility(p).status == CompatibilityStatus::unsupported_schema);
    p = validProfile();
    p.required_capabilities = {"future-feature"};
    CHECK(checkCompatibility(p).status == CompatibilityStatus::unsupported_required_capability);
    p = validProfile();
    p.profile_status = ProfileStatus::revoked;
    CHECK(validateProfile(p, false).valid());
    CHECK(refreshIntegrity(p));
    CHECK(evaluateProfile(p, validRequest(p)).status == EvaluationStatus::revoked);
    p = validProfile();
    p.validity.calibrated_at = "2026-02-30T00:00:00Z";
    CHECK(!validateProfile(p, false).valid());
    p = validProfile();
    p.validity.calibrated_at = "2026-08-15T12:00:00.9Z";
    p.validity.not_valid_after = "2026-08-15T12:00:00Z";
    CHECK(!validateProfile(p, false).valid());
}

void testEvaluationAndFailClosedBoundaries() {
    const auto p = validProfile();
    const auto q = validRequest(p);
    const auto result = evaluateProfile(p, q);
    CHECK(result.status == EvaluationStatus::qualification_capable);
    CHECK(std::abs(result.indicated_error_hz - 7.0) < 1e-9);
    CHECK(std::abs(result.estimated_true_frequency_hz - 10'499'993.0) < 1e-9);
    CHECK(result.target_offset_hz && std::abs(*result.target_offset_hz) < 1e-9);
    auto changed = q;
    changed.indicated_frequency_hz = 12'000'000.0;
    CHECK(evaluateProfile(p, changed).status == EvaluationStatus::outside_frequency_domain);
    changed = q;
    changed.temperature_c = 31.0;
    CHECK(evaluateProfile(p, changed).status == EvaluationStatus::outside_temperature_domain);
    changed = q;
    changed.warmup_seconds = 1199;
    CHECK(evaluateProfile(p, changed).status == EvaluationStatus::insufficient_warmup);
    changed = q;
    changed.evaluated_at = "2028-01-01T00:00:00Z";
    CHECK(evaluateProfile(p, changed).status == EvaluationStatus::profile_expired);
    changed = q;
    changed.evaluated_at = p.validity.not_valid_after;
    CHECK(evaluateProfile(p, changed).status == EvaluationStatus::profile_expired);
    changed = q;
    changed.evaluated_at = "2026-08-15T08:00:00-05:00";
    CHECK(evaluateProfile(p, changed).status == EvaluationStatus::qualification_capable);
    changed = q;
    changed.device.identifier = "other";
    CHECK(evaluateProfile(p, changed).status == EvaluationStatus::identity_mismatch);
    changed = q;
    changed.configuration.sample_rate_hz = 1;
    CHECK(evaluateProfile(p, changed).status == EvaluationStatus::configuration_mismatch);
    changed = q;
    changed.required_reliability_quotient = 93;
    CHECK(evaluateProfile(p, changed).status == EvaluationStatus::assurance_requirement_not_met);
    changed = q;
    changed.maximum_expanded_uncertainty_hz = 1.0;
    CHECK(evaluateProfile(p, changed).status == EvaluationStatus::uncertainty_requirement_not_met);
}

void testAtomicReplacementPreservesKnownGood() {
    const auto root = std::filesystem::temp_directory_path() / "sdrcal-profile-engine-tests";
    std::error_code ignored;
    std::filesystem::remove_all(root, ignored);
    std::filesystem::create_directories(root);
    const auto destination = root / "profile.json";
    auto p = validProfile();
    CHECK(replaceProfileAtomically(destination, p).succeeded());
    std::ifstream first(destination);
    const std::string original((std::istreambuf_iterator<char>(first)), {});
    first.close();
    p.integrity.sha256 = std::string(64, '0');
    CHECK(!replaceProfileAtomically(destination, p).succeeded());
    std::ifstream second(destination);
    const std::string retained((std::istreambuf_iterator<char>(second)), {});
    second.close();
    CHECK(original == retained);
    for (const auto& entry : std::filesystem::directory_iterator(root))
        CHECK(entry.path().filename().string().find(".tmp-native-profile-") == std::string::npos);
    std::filesystem::remove_all(root, ignored);
}

} // namespace

int main() {
    try {
        testCanonicalJsonAndSha256();
        testValidationIntegrityAndSignatureHooks();
        testSemanticFailures();
        testEvaluationAndFailClosedBoundaries();
        testAtomicReplacementPreservesKnownGood();
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
    std::cout << "native profile engine tests passed\n";
    return 0;
}
