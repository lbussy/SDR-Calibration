#include "soapy/SoapyWorkflowBoundary.h"

#include "capture/CapturePlan.h"
#include "profile/CanonicalJson.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

namespace sdrcal::soapy {
namespace {

bool isCancelled(const std::function<bool()>& check) {
    return check && check();
}

bool sameIdentity(const profile::DeviceIdentity& left, const profile::DeviceIdentity& right) {
    return left.driver == right.driver && left.manufacturer == right.manufacturer &&
           left.model == right.model && left.identifier == right.identifier &&
           left.strength == right.strength;
}

bool sameConfiguration(const profile::DeviceConfiguration& left,
                       const profile::DeviceConfiguration& right) {
    return left.clock_source == right.clock_source && left.sample_rate_hz == right.sample_rate_hz &&
           left.bandwidth_hz == right.bandwidth_hz &&
           left.frequency_correction_ppm == right.frequency_correction_ppm &&
           left.driver_version == right.driver_version &&
           left.firmware_version == right.firmware_version &&
           left.antenna_port == right.antenna_port && left.tuner_path == right.tuner_path &&
           profile::canonicalizeJson(left.binding_extension).value ==
               profile::canonicalizeJson(right.binding_extension).value;
}

std::optional<std::string> mapValue(const std::map<std::string, std::string>& values,
                                    const std::string& key) {
    const auto found = values.find(key);
    if (found == values.end() || found->second.empty())
        return std::nullopt;
    return found->second;
}

std::optional<std::string> firstValue(const std::map<std::string, std::string>& values,
                                      std::initializer_list<const char*> keys) {
    for (const auto* key : keys) {
        if (const auto value = mapValue(values, key); value)
            return value;
    }
    return std::nullopt;
}

std::optional<std::int64_t> wholePositive(const std::optional<double>& value) {
    if (!value || !std::isfinite(*value) || *value <= 0.0 ||
        *value > static_cast<double>(std::numeric_limits<std::int64_t>::max()) ||
        std::floor(*value) != *value)
        return std::nullopt;
    return static_cast<std::int64_t>(*value);
}

std::optional<application::DeviceCandidate> mapDevice(const capture::DeviceMetadata& metadata,
                                                      const capture::EffectiveSettings& settings) {
    const auto manufacturer = firstValue(metadata.hardware_info, {"manufacturer", "vendor"});
    const auto model = firstValue(metadata.hardware_info, {"model", "product"});
    const auto rate = wholePositive(settings.sample_rate_sps.effective);
    if (!metadata.driver_key || !manufacturer || !model || !metadata.serial || !rate ||
        !metadata.clock_source || !settings.frequency_correction_ppm ||
        !std::isfinite(*settings.frequency_correction_ppm))
        return std::nullopt;

    application::DeviceCandidate result;
    result.identity = {*metadata.driver_key, *manufacturer, *model, *metadata.serial,
                       profile::IdentityStrength::hardware_serial};
    result.configuration.clock_source = *metadata.clock_source;
    result.configuration.sample_rate_hz = *rate;
    result.configuration.bandwidth_hz = wholePositive(settings.bandwidth_hz.effective);
    result.configuration.frequency_correction_ppm = *settings.frequency_correction_ppm;
    result.configuration.driver_version = metadata.driver_version;
    result.configuration.firmware_version = metadata.firmware_version;
    result.configuration.antenna_port = metadata.antenna;
    result.configuration.tuner_path = metadata.tuner_path;
    for (const auto& [key, value] : metadata.resolved_arguments)
        result.configuration.binding_extension["soapy_argument_" + key] = value;
    if (settings.gain_db.effective)
        result.configuration.binding_extension["effective_gain_db"] = *settings.gain_db.effective;
    if (settings.automatic_gain)
        result.configuration.binding_extension["automatic_gain"] = *settings.automatic_gain;
    return result;
}

std::optional<capture::CapturePlan> preflightPlan(const capture::CaptureRequest& request,
                                                  const capture::ResourceLimits& limits,
                                                  const capture::SettingTolerances& tolerances) {
    auto boundedRequest = request;
    if (boundedRequest.duration_seconds) {
        const double rateMargin =
            std::max(tolerances.sample_rate_sps.absolute,
                     std::abs(request.sample_rate_sps) * tolerances.sample_rate_sps.relative);
        if (!std::isfinite(rateMargin) || rateMargin < 0.0 ||
            request.sample_rate_sps > std::numeric_limits<double>::max() - rateMargin)
            return std::nullopt;
        boundedRequest.sample_rate_sps += rateMargin;
    }
    capture::EffectiveSettings settings;
    settings.center_frequency_hz = {boundedRequest.center_frequency_hz,
                                    boundedRequest.center_frequency_hz,
                                    capture::SettingState::applied_verified};
    settings.sample_rate_sps = {boundedRequest.sample_rate_sps, boundedRequest.sample_rate_sps,
                                capture::SettingState::applied_verified};
    settings.bandwidth_hz = {boundedRequest.bandwidth_hz, boundedRequest.bandwidth_hz,
                             capture::SettingState::applied_verified};
    settings.gain_db = {boundedRequest.gain_db, boundedRequest.gain_db,
                        capture::SettingState::applied_verified};
    const auto planned = capture::makeCapturePlan(boundedRequest, settings, limits);
    return planned.ok() ? planned.plan : std::nullopt;
}

application::AcquisitionResult
failure(std::string reason, application::AcquisitionFailureStage stage,
        application::AcquisitionStatus status = application::AcquisitionStatus::failed) {
    application::AcquisitionResult result;
    result.status = status;
    result.failure_stage = stage;
    result.reason = std::move(reason);
    return result;
}

void copyCleanup(application::AcquisitionResult& result, const SoapyCleanupReport& cleanup) {
    result.deactivation = cleanup.deactivation;
    result.stream_close = cleanup.stream_close;
    result.device_release = cleanup.device_release;
    result.final_device_state_safe = cleanup.final_state == capture::FinalState::known_safe;
}

} // namespace

SoapyWorkflowBoundary::SoapyWorkflowBoundary(SoapyApi& api, SoapyWorkflowOptions options)
    : api_(api), options_(std::move(options)) {}

std::vector<application::DeviceCandidate> SoapyWorkflowBoundary::discover() {
    if (options_.capture_request.enumeration_index)
        return {};
    try {
        const auto matches = api_.enumerate(options_.capture_request.device_arguments);
        if (matches.size() != 1U)
            return {};
        const auto serial = mapValue(matches.front(), "serial");
        if (!serial || *serial != options_.expected_device.identity.identifier)
            return {};
        return {options_.expected_device};
    } catch (...) {
        return {};
    }
}

application::AcquisitionResult SoapyWorkflowBoundary::acquire(
    const application::DeviceCandidate& device, const profile::DeviceConfiguration& requested,
    const application::ObservationRequest& observation, const std::function<bool()>& cancel) {
    if (isCancelled(cancel))
        return failure("acquisition cancelled before preflight",
                       application::AcquisitionFailureStage::preflight,
                       application::AcquisitionStatus::cancelled);
    if (!sameIdentity(device.identity, options_.expected_device.identity) ||
        !sameConfiguration(requested, options_.expected_device.configuration))
        return failure("requested device identity or configuration does not match the boundary",
                       application::AcquisitionFailureStage::identity_configuration);
    if (options_.capture_request.setting_policy != capture::SettingPolicy::strict)
        return failure("live workflow requires strict effective-setting policy",
                       application::AcquisitionFailureStage::preflight);

    auto request = options_.capture_request;
    request.center_frequency_hz = observation.indicated_center_frequency_hz;
    request.sample_rate_sps = static_cast<double>(requested.sample_rate_hz);
    if (requested.bandwidth_hz)
        request.bandwidth_hz = static_cast<double>(*requested.bandwidth_hz);
    const auto provisional =
        preflightPlan(request, options_.resource_limits, options_.setting_tolerances);
    if (!provisional)
        return failure("capture request failed pre-device planning",
                       application::AcquisitionFailureStage::preflight);
    const auto memoryErrors =
        capture::validateMemoryAcquisitionPlan(*provisional, options_.memory_limits);
    if (!memoryErrors.empty())
        return failure("memory preflight failed: " + memoryErrors.front().message,
                       application::AcquisitionFailureStage::preflight);
    if (isCancelled(cancel))
        return failure("acquisition cancelled before device construction",
                       application::AcquisitionFailureStage::preflight,
                       application::AcquisitionStatus::cancelled);

    SoapyCaptureSession session(api_);
    const auto prepared =
        session.prepare(request, options_.setting_tolerances, options_.resource_limits);
    if (!prepared.ready || !prepared.plan) {
        auto result = failure("Soapy preparation failed at " + toString(prepared.error.stage) +
                                  ": " + prepared.error.message,
                              application::AcquisitionFailureStage::preparation);
        copyCleanup(result, session.cleanupReport());
        return result;
    }
    const auto actual = mapDevice(prepared.device, prepared.effective);
    if (!actual || !sameIdentity(actual->identity, device.identity) ||
        !sameConfiguration(actual->configuration, requested)) {
        static_cast<void>(session.cleanup());
        auto result = failure("effective device identity or configuration did not match request",
                              application::AcquisitionFailureStage::identity_configuration);
        copyCleanup(result, session.cleanupReport());
        return result;
    }

    capture::MemoryAcquisition acquisition;
    auto acquired = acquisition.acquire(*prepared.plan, session, cancel, options_.memory_limits);
    application::AcquisitionResult result;
    result.stream_evidence = acquired.stream;
    copyCleanup(result, session.cleanupReport());
    if (!acquired.succeeded()) {
        result.status = acquired.status == capture::CaptureStatus::cancelled
                            ? application::AcquisitionStatus::cancelled
                            : application::AcquisitionStatus::failed;
        result.reason = "bounded acquisition failed: " + acquired.error.message;
        result.acquisition_error = acquired.error;
        result.failure_stage = acquired.error.category == capture::ErrorCategory::cleanup
                                   ? application::AcquisitionFailureStage::cleanup
                                   : application::AcquisitionFailureStage::acquisition;
        return result;
    }
    if (isCancelled(cancel)) {
        result.status = application::AcquisitionStatus::cancelled;
        result.failure_stage = application::AcquisitionFailureStage::analysis;
        result.reason = "acquisition cancelled before analysis";
        return result;
    }

    const double sampleRate = *prepared.effective.sample_rate_sps.effective;
    const auto estimate =
        core::estimateCarrier(acquired.samples, sampleRate, options_.estimator_options);
    if (!estimate.ok()) {
        result.failure_stage = application::AcquisitionFailureStage::estimation;
        result.reason = "carrier estimation failed: " + estimate.reason;
        return result;
    }
    const auto quality = core::analyzeSignalQuality(acquired.samples, sampleRate, estimate,
                                                    options_.signal_quality_options);
    if (!quality.succeeded()) {
        result.failure_stage = application::AcquisitionFailureStage::analysis;
        result.reason = "signal-quality analysis failed: " + quality.reason;
        return result;
    }

    const auto conditions = options_.reference_conditions
                                ? options_.reference_conditions(observation)
                                : ReferenceConditionEvidence{};
    result.status = application::AcquisitionStatus::success;
    result.identity = actual->identity;
    result.configuration = actual->configuration;
    result.samples = std::move(acquired.samples);
    result.carrier_estimate = estimate;
    result.effective_indicated_center_frequency_hz =
        *prepared.effective.center_frequency_hz.effective;
    result.effective_indicated_center_verified =
        prepared.effective.center_frequency_hz.state == capture::SettingState::applied_verified;
    result.sample_rate_sps = sampleRate;
    result.duration_seconds = static_cast<double>(result.samples.size()) / sampleRate;
    result.signal_to_noise_db = quality.signal_to_noise_db;
    result.clipped_samples = quality.clipped_samples;
    result.missing_samples =
        result.stream_evidence.target_samples - result.stream_evidence.written_samples;
    result.discontinuities = result.stream_evidence.discontinuities;
    result.frequency_instability_hz = quality.frequency_instability_hz;
    result.interference_to_carrier_db = quality.interference_to_carrier_db;
    result.effective_configuration = core::EffectiveConfigurationValidity::verified;
    result.reference_conditions_met = conditions.established && !conditions.evidence_id.empty();
    result.reference_conditions_evidence = conditions.evidence_id;
    result.signal_quality_version = quality.version;
    return result;
}

} // namespace sdrcal::soapy
