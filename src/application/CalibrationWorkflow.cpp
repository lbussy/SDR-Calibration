#include "application/CalibrationWorkflow.h"

#include "profile/CanonicalJson.h"
#include "profile/ProfileEngine.h"
#include "profile/Sha256.h"

#include <algorithm>
#include <cmath>
#include <set>

namespace sdrcal::application {
namespace {
bool sameIdentity(const profile::DeviceIdentity& a, const profile::DeviceIdentity& b) {
    return a.driver == b.driver && a.manufacturer == b.manufacturer && a.model == b.model &&
           a.identifier == b.identifier && a.strength == b.strength;
}

bool sameConfiguration(const profile::DeviceConfiguration& a,
                       const profile::DeviceConfiguration& b) {
    return a.clock_source == b.clock_source && a.sample_rate_hz == b.sample_rate_hz &&
           a.bandwidth_hz == b.bandwidth_hz &&
           a.frequency_correction_ppm == b.frequency_correction_ppm &&
           a.driver_version == b.driver_version && a.firmware_version == b.firmware_version &&
           a.antenna_port == b.antenna_port && a.tuner_path == b.tuner_path &&
           profile::canonicalizeJson(a.binding_extension).value ==
               profile::canonicalizeJson(b.binding_extension).value;
}

bool usableDevice(const DeviceCandidate& device) {
    return !device.identity.driver.empty() && !device.identity.manufacturer.empty() &&
           !device.identity.model.empty() && !device.identity.identifier.empty() &&
           !device.configuration.clock_source.empty() && device.configuration.sample_rate_hz > 0 &&
           std::isfinite(device.configuration.frequency_correction_ppm);
}

profile::JsonObject configurationJson(const profile::DeviceConfiguration& value) {
    profile::JsonObject object{{"binding_extension", value.binding_extension},
                               {"clock_source", value.clock_source},
                               {"frequency_correction_ppm", value.frequency_correction_ppm},
                               {"sample_rate_hz", value.sample_rate_hz}};
    if (value.bandwidth_hz)
        object["bandwidth_hz"] = *value.bandwidth_hz;
    if (value.driver_version)
        object["driver_version"] = *value.driver_version;
    if (value.firmware_version)
        object["firmware_version"] = *value.firmware_version;
    if (value.antenna_port)
        object["antenna_port"] = *value.antenna_port;
    if (value.tuner_path)
        object["tuner_path"] = *value.tuner_path;
    return object;
}

WorkflowResult failure(WorkflowResult result, WorkflowStage stage, std::string reason,
                       WorkflowStatus status = WorkflowStatus::failed) {
    result.status = status;
    result.terminal_stage = stage;
    result.reason = reason;
    result.stages.push_back({stage, false, reason});
    return result;
}

void passed(WorkflowResult& result, WorkflowStage stage, std::string detail) {
    result.stages.push_back({stage, true, std::move(detail)});
}

bool cancelled(const CancellationCheck& check) {
    return check && check();
}

core::ReferenceClass weakestClass(const std::vector<reference::ResolutionResult>& references) {
    auto value = core::ReferenceClass::authority_confirmed;
    int ceiling = 100;
    for (const auto& item : references) {
        if (item.effective_assurance_ceiling < ceiling) {
            ceiling = item.effective_assurance_ceiling;
            value = item.reference->reference_class;
        }
    }
    return value;
}
} // namespace

WorkflowResult CalibrationWorkflow::run(const WorkflowRequest& request,
                                        DeviceWorkflowBoundary& devices,
                                        CancellationCheck cancel) const {
    WorkflowResult result;
    std::set<std::string> observation_ids;
    std::set<std::string> independence_ids;
    if (request.profile_id.empty() || request.calibration_run_id.empty() ||
        request.selected_device_identifier.empty() || request.software_version.empty() ||
        request.requested_configuration.clock_source.empty() ||
        request.requested_configuration.sample_rate_hz <= 0 || request.observations.size() < 2U ||
        request.model_type != core::FrequencyModelType::linear ||
        request.qualification_threshold < 0 || request.qualification_threshold > 100 ||
        !std::ranges::all_of(request.observations, [&](const auto& observation) {
            return !observation.observation_id.empty() && !observation.independence_id.empty() &&
                   !observation.reference_id.empty() &&
                   std::isfinite(observation.indicated_center_frequency_hz) &&
                   observation.indicated_center_frequency_hz > 0.0 &&
                   observation_ids.insert(observation.observation_id).second &&
                   independence_ids.insert(observation.independence_id).second;
        })) {
        return failure(
            std::move(result), WorkflowStage::validate_request,
            "workflow identity, unique observations, linear model, and bounds are required");
    }
    passed(result, WorkflowStage::validate_request, "workflow request accepted");
    if (cancelled(cancel))
        return failure(std::move(result), WorkflowStage::discover_device, "workflow cancelled",
                       WorkflowStatus::cancelled);

    const auto discovered = devices.discover();
    std::vector<DeviceCandidate> matches;
    std::ranges::copy_if(discovered, std::back_inserter(matches), [&](const auto& device) {
        return device.identity.identifier == request.selected_device_identifier;
    });
    if (matches.size() != 1U)
        return failure(std::move(result), WorkflowStage::discover_device,
                       "selected device identity is missing or ambiguous");
    const auto device = matches.front();
    if (!usableDevice(device))
        return failure(std::move(result), WorkflowStage::discover_device,
                       "selected device identity or configuration is invalid");
    passed(result, WorkflowStage::discover_device, "exactly one device selected");

    std::string registry_error;
    if (!reference::verifyRegistryIntegrity(request.global_registry, request.registry_verifier,
                                            true, &registry_error))
        return failure(std::move(result), WorkflowStage::resolve_references,
                       "global registry authentication failed: " + registry_error);

    std::vector<reference::ResolutionResult> references;
    references.reserve(request.observations.size());
    for (const auto& observation : request.observations) {
        const auto resolved =
            reference::resolveReference(request.global_registry, request.local_references,
                                        observation.reference_id, request.created_at);
        if (!resolved.usable())
            return failure(std::move(result), WorkflowStage::resolve_references,
                           "reference " + observation.reference_id +
                               " rejected: " + resolved.reason);
        references.push_back(resolved);
    }
    passed(result, WorkflowStage::resolve_references, "all references resolved");
    if (cancelled(cancel))
        return failure(std::move(result), WorkflowStage::acquire_observations, "workflow cancelled",
                       WorkflowStatus::cancelled);

    std::vector<AcquisitionResult> acquisitions;
    acquisitions.reserve(request.observations.size());
    for (const auto& observation : request.observations) {
        if (cancelled(cancel))
            return failure(std::move(result), WorkflowStage::acquire_observations,
                           "workflow cancelled", WorkflowStatus::cancelled);
        auto acquired =
            devices.acquire(device, request.requested_configuration, observation, cancel);
        if (acquired.status == AcquisitionStatus::cancelled)
            return failure(std::move(result), WorkflowStage::acquire_observations,
                           "acquisition cancelled", WorkflowStatus::cancelled);
        if (acquired.status != AcquisitionStatus::success || !acquired.final_device_state_safe)
            return failure(std::move(result), WorkflowStage::acquire_observations,
                           acquired.reason.empty() ? "acquisition failed or device state is unsafe"
                                                   : acquired.reason);
        if (!sameIdentity(device.identity, acquired.identity) ||
            !sameConfiguration(device.configuration, acquired.configuration))
            return failure(std::move(result), WorkflowStage::acquire_observations,
                           "effective device identity or configuration changed");
        if (!acquired.effective_indicated_center_verified ||
            !std::isfinite(acquired.effective_indicated_center_frequency_hz) ||
            acquired.effective_indicated_center_frequency_hz <= 0.0 ||
            !std::isfinite(acquired.sample_rate_sps) || acquired.sample_rate_sps <= 0.0 ||
            acquired.sample_rate_sps != static_cast<double>(acquired.configuration.sample_rate_hz))
            return failure(std::move(result), WorkflowStage::acquire_observations,
                           "effective center frequency or sample rate is unverified or invalid");
        acquisitions.push_back(std::move(acquired));
    }
    passed(result, WorkflowStage::acquire_observations,
           "all bounded acquisitions completed with safe final state");

    std::vector<core::ModelObservation> model_observations;
    for (std::size_t index = 0; index < acquisitions.size(); ++index) {
        const auto& acquired = acquisitions[index];
        const auto estimate = core::estimateCarrier(acquired.samples, acquired.sample_rate_sps,
                                                    request.estimator_options);
        core::ObservationDiagnostics diagnostics;
        diagnostics.estimate = estimate;
        diagnostics.duration_seconds = acquired.duration_seconds;
        diagnostics.signal_to_noise_db = acquired.signal_to_noise_db;
        diagnostics.clipped_samples = acquired.clipped_samples;
        diagnostics.missing_samples = acquired.missing_samples;
        diagnostics.discontinuities = acquired.discontinuities;
        diagnostics.frequency_instability_hz = acquired.frequency_instability_hz;
        diagnostics.interference_to_carrier_db = acquired.interference_to_carrier_db;
        diagnostics.effective_configuration = acquired.effective_configuration;
        diagnostics.reference_suitability = references[index].usable()
                                                ? core::ReferenceSuitability::suitable
                                                : core::ReferenceSuitability::unverifiable;
        diagnostics.reference_conditions_met = acquired.reference_conditions_met;
        const auto acceptance = core::evaluateObservation(diagnostics, request.acceptance_policy);
        if (!acceptance.accepted()) {
            const auto detail = acceptance.rejections.empty()
                                    ? "observation rejected"
                                    : acceptance.rejections.front().reason;
            return failure(std::move(result), WorkflowStage::estimate_and_accept,
                           "observation " + request.observations[index].observation_id +
                               " rejected: " + detail);
        }
        const double indicated_frequency =
            acquired.effective_indicated_center_frequency_hz + *estimate.frequency_hz;
        model_observations.push_back(
            {indicated_frequency, references[index].reference->nominal_frequency_hz, true,
             acceptance.policy_version, request.observations[index].independence_id});
    }
    passed(result, WorkflowStage::estimate_and_accept, "all observations accepted");
    if (cancelled(cancel))
        return failure(std::move(result), WorkflowStage::fit_model, "workflow cancelled",
                       WorkflowStatus::cancelled);

    const auto fit = core::fitLinearModel(model_observations);
    if (!fit.succeeded())
        return failure(std::move(result), WorkflowStage::fit_model, fit.reason);
    passed(result, WorkflowStage::fit_model, "linear model fitted within observation domain");
    if (cancelled(cancel))
        return failure(std::move(result), WorkflowStage::calculate_uncertainty,
                       "workflow cancelled", WorkflowStatus::cancelled);

    const auto uncertainty = core::calculateUncertainty(request.uncertainty_budget);
    if (!uncertainty.succeeded())
        return failure(std::move(result), WorkflowStage::calculate_uncertainty, uncertainty.reason);
    passed(result, WorkflowStage::calculate_uncertainty, "uncertainty budget calculated");
    if (cancelled(cancel))
        return failure(std::move(result), WorkflowStage::calculate_assurance, "workflow cancelled",
                       WorkflowStatus::cancelled);

    std::vector<core::AssuranceComponent> assurance_input;
    for (const auto& component : request.assurance_components)
        assurance_input.push_back({component.name, component.score});
    const auto reference_class = weakestClass(references);
    const auto assurance = core::calculateReliabilityQuotient(reference_class, assurance_input);
    if (!assurance.succeeded())
        return failure(std::move(result), WorkflowStage::calculate_assurance, assurance.reason);
    passed(result, WorkflowStage::calculate_assurance,
           "reliability quotient calculated under reference ceiling");
    if (cancelled(cancel))
        return failure(std::move(result), WorkflowStage::build_evidence, "workflow cancelled",
                       WorkflowStatus::cancelled);

    profile::JsonArray observation_json;
    for (std::size_t index = 0; index < model_observations.size(); ++index) {
        const auto& observation = model_observations[index];
        observation_json.emplace_back(
            profile::JsonObject{{"effective_center_frequency_hz",
                                 acquisitions[index].effective_indicated_center_frequency_hz},
                                {"independence_id", observation.independence_id},
                                {"indicated_frequency_hz", observation.indicated_frequency_hz},
                                {"requested_center_frequency_hz",
                                 request.observations[index].indicated_center_frequency_hz},
                                {"true_frequency_hz", observation.estimated_true_frequency_hz}});
    }
    const auto summary_json = profile::canonicalizeJson(profile::JsonObject{
        {"model_intercept_error_hz", fit.model.intercept_error_hz},
        {"model_slope_ppm", fit.model.slope_ppm},
        {"observations", observation_json},
        {"profile_id", request.profile_id},
        {"effective_configuration", configurationJson(device.configuration)},
        {"requested_configuration", configurationJson(request.requested_configuration)},
        {"reliability_quotient", assurance.quotient},
        {"schema_name", "sdr-calibration-workflow-summary"},
        {"schema_version", "1.0.0"}});
    if (!summary_json.success)
        return failure(std::move(result), WorkflowStage::build_evidence, summary_json.error);
    result.evidence_summary = summary_json.value;
    auto evidence_request = request.evidence;
    evidence_request.created_at = request.created_at;
    evidence_request.source_identity = request.calibration_run_id;
    evidence_request.atomic_write_completed = false;
    evidence_request.artifacts.push_back({"workflow-summary",
                                          evidence::ArtifactClass::reduced_evidence,
                                          "application/json",
                                          profile::sha256Hex(result.evidence_summary),
                                          result.evidence_summary.size(),
                                          {},
                                          false});
    const auto evidence = evidence::buildEvidenceManifest(evidence_request);
    if (!evidence.valid())
        return failure(std::move(result), WorkflowStage::build_evidence,
                       evidence.issues.front().message);
    result.evidence_manifest = evidence.manifest_json;
    passed(result, WorkflowStage::build_evidence, "bounded evidence candidate generated");
    if (cancelled(cancel))
        return failure(std::move(result), WorkflowStage::build_profile, "workflow cancelled",
                       WorkflowStatus::cancelled);

    profile::CalibrationProfile candidate;
    candidate.profile_id = request.profile_id;
    candidate.created_at = request.created_at;
    candidate.device = device.identity;
    candidate.configuration = device.configuration;
    candidate.profile_status =
        uncertainty.material_components_complete &&
                assurance.quotient >= request.qualification_threshold && assurance.quotient >= 90 &&
                device.identity.strength != profile::IdentityStrength::insufficient
            ? profile::ProfileStatus::qualification_capable
            : profile::ProfileStatus::informational_only;
    profile::FrequencySegment segment;
    segment.segment_id = "segment-1";
    segment.minimum_frequency_hz = fit.model.minimum_frequency_hz;
    segment.maximum_frequency_hz = fit.model.maximum_frequency_hz;
    segment.model_type = profile::SegmentModelType::linear;
    segment.reference_frequency_hz = fit.model.reference_frequency_hz;
    segment.intercept_error_hz = fit.model.intercept_error_hz;
    segment.slope_ppm = fit.model.slope_ppm;
    segment.uncertainty.kind = profile::UncertaintyKind::expanded;
    segment.uncertainty.coverage_factor = uncertainty.coverage_factor;
    segment.uncertainty.base_hz = uncertainty.expanded_uncertainty_hz;
    segment.uncertainty.included_components.reserve(request.uncertainty_budget.components.size());
    for (const auto& component : request.uncertainty_budget.components)
        segment.uncertainty.included_components.push_back(component.name);
    segment.uncertainty.excluded_components =
        request.uncertainty_budget.excluded_or_unknown_material_components;
    candidate.segments = {segment};
    candidate.assurance.scoring_policy_version = assurance.scoring_version;
    candidate.assurance.reliability_quotient = assurance.quotient;
    candidate.assurance.reference_class = reference_class;
    candidate.assurance.reference_score_ceiling = assurance.reference_ceiling;
    candidate.assurance.components = request.assurance_components;
    for (const auto& component : request.assurance_components)
        if (component.score == assurance.quotient)
            candidate.assurance.limiting_components.push_back(component.name);
    if (assurance.reference_ceiling == assurance.quotient)
        candidate.assurance.limiting_components.push_back("reference_class_ceiling");
    std::ranges::sort(candidate.assurance.limiting_components);
    candidate.assurance.limiting_components.erase(
        std::ranges::unique(candidate.assurance.limiting_components).begin(),
        candidate.assurance.limiting_components.end());
    candidate.validity = {request.created_at, request.not_valid_after,
                          request.minimum_warmup_seconds, request.temperature};
    candidate.provenance.calibration_run_id = request.calibration_run_id;
    candidate.provenance.software = {request.software_name, request.software_version};
    candidate.provenance.reference_registry = profile::RegistryRecord{
        1, request.global_registry.registry_version, request.global_registry.integrity.sha256};
    std::set<std::string> added_references;
    for (const auto& resolved : references) {
        const auto& item = *resolved.reference;
        if (!added_references.insert(item.reference_id).second)
            continue;
        const auto description = resolved.local_overlay_selected
                                     ? "local overlay: " + item.location_or_connection
                                     : item.location_or_connection;
        candidate.provenance.reference_set.push_back(
            {item.reference_id, item.reference_class, item.nominal_frequency_hz,
             item.assurance_ceiling, item.frequency_uncertainty_hz, description,
             item.evidence.front().sha256});
    }
    candidate.provenance.evidence_bundle =
        profile::EvidenceBundleRecord{"application/vnd.sdr-calibration.evidence+json",
                                      profile::sha256Hex(result.evidence_manifest),
                                      {}};
    for (const auto& observation : request.observations)
        candidate.provenance.observation_ids.push_back(observation.observation_id);
    std::string profile_error;
    if (!profile::refreshIntegrity(candidate, &profile_error) ||
        !profile::validateProfile(candidate).valid())
        return failure(std::move(result), WorkflowStage::build_profile,
                       profile_error.empty() ? "generated profile failed validation"
                                             : profile_error);
    passed(result, WorkflowStage::build_profile,
           "authoritative native profile candidate validated");

    if (cancelled(cancel))
        return failure(std::move(result), WorkflowStage::export_interoperability,
                       "workflow cancelled", WorkflowStatus::cancelled);

    if (request.interoperability) {
        auto exported =
            wsjtx::exportSettings(candidate, request.interoperability->wsjtx_version,
                                  request.interoperability->configuration_name, request.created_at);
        if (exported.succeeded()) {
            candidate.adapters.push_back(*exported.adapter_record);
            if (!profile::refreshIntegrity(candidate, &profile_error) ||
                !profile::validateProfile(candidate).valid())
                return failure(std::move(result), WorkflowStage::export_interoperability,
                               "profile became invalid while recording interoperability");
            passed(result, WorkflowStage::export_interoperability,
                   "lossy WSJT-X projection generated separately");
        } else {
            result.warnings.push_back("WSJT-X export failed: " + exported.reason);
            result.stages.push_back(
                {WorkflowStage::export_interoperability, false, exported.reason});
        }
        result.interoperability = std::move(exported);
    }
    result.profile = std::move(candidate);
    result.status = WorkflowStatus::success;
    result.terminal_stage = WorkflowStage::complete;
    result.reason = "deterministic calibration workflow completed";
    passed(result, WorkflowStage::complete, result.reason);
    return result;
}

} // namespace sdrcal::application
