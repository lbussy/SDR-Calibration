#include "capture/CapturePlan.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace sdrcal::capture {
namespace {

void addError(std::vector<CaptureError>& errors, std::string message) {
    errors.push_back({ErrorCategory::validation, std::move(message)});
}

bool finitePositive(double value) { return std::isfinite(value) && value > 0.0; }

} // namespace

bool withinTolerance(double requested, double effective, const NumericTolerance& tolerance) {
    if (!std::isfinite(requested) || !std::isfinite(effective) || tolerance.absolute < 0.0 ||
        tolerance.relative < 0.0) {
        return false;
    }
    const double allowed = std::max(tolerance.absolute, tolerance.relative * std::abs(requested));
    return std::abs(effective - requested) <= allowed;
}

EffectiveSetting classifyEffectiveSetting(
    std::optional<double> requested,
    std::optional<double> effective,
    bool supported,
    bool applicationSucceeded,
    const NumericTolerance& tolerance) {
    if (effective.has_value() && !std::isfinite(*effective)) {
        effective.reset();
    }
    if (!requested.has_value()) {
        return {requested, effective, effective.has_value() ? SettingState::applied_verified
                                                            : SettingState::applied_unverified};
    }
    if (!supported) {
        return {requested, effective, SettingState::unsupported};
    }
    if (!applicationSucceeded) {
        return {requested, effective, SettingState::failed};
    }
    if (!effective.has_value()) {
        return {requested, effective, SettingState::applied_unverified};
    }
    return {requested,
            effective,
            withinTolerance(*requested, *effective, tolerance) ? SettingState::applied_verified
                                                                : SettingState::applied_changed};
}

std::vector<CaptureError> evaluateEffectiveSettings(
    const EffectiveSettings& settings,
    SettingPolicy policy) {
    std::vector<CaptureError> errors;
    const auto evaluate = [&](const EffectiveSetting& setting, const std::string& name) {
        if (!setting.requested.has_value()) {
            return;
        }
        const bool fatal = setting.state == SettingState::failed ||
                           setting.state == SettingState::unsupported ||
                           (policy == SettingPolicy::strict &&
                            setting.state != SettingState::applied_verified);
        if (fatal) {
            errors.push_back(
                {ErrorCategory::effective_setting, name + " is " + toString(setting.state)});
        }
    };
    evaluate(settings.center_frequency_hz, "center_frequency_hz");
    evaluate(settings.sample_rate_sps, "sample_rate_sps");
    evaluate(settings.bandwidth_hz, "bandwidth_hz");
    evaluate(settings.gain_db, "gain_db");
    return errors;
}

std::vector<CaptureError> validateCaptureRequestBeforeDevice(
    const CaptureRequest& request,
    const ResourceLimits& limits) {
    std::vector<CaptureError> errors;
    if (request.output_path.empty()) {
        addError(errors, "output_path is required");
    }
    for (const auto& [key, value] : request.device_arguments) {
        static_cast<void>(value);
        if (key.empty()) {
            addError(errors, "device argument keys must not be empty");
            break;
        }
    }
    if (!finitePositive(request.center_frequency_hz)) {
        addError(errors, "center_frequency_hz must be finite and positive");
    }
    if (!finitePositive(request.sample_rate_sps)) {
        addError(errors, "sample_rate_sps must be finite and positive");
    }
    if (request.bandwidth_hz.has_value() && !finitePositive(*request.bandwidth_hz)) {
        addError(errors, "bandwidth_hz must be finite and positive");
    }
    if (request.gain_db.has_value() && !std::isfinite(*request.gain_db)) {
        addError(errors, "gain_db must be finite");
    }
    if (request.duration_seconds.has_value() == request.sample_count.has_value()) {
        addError(errors, "exactly one of duration_seconds or sample_count is required");
    }
    if (request.duration_seconds.has_value() && !finitePositive(*request.duration_seconds)) {
        addError(errors, "duration_seconds must be finite and positive");
    }
    if (request.duration_seconds.has_value() &&
        *request.duration_seconds > limits.maximum_duration_seconds) {
        addError(errors, "duration_seconds exceeds the configured limit");
    }
    if (request.sample_count.has_value() && *request.sample_count == 0) {
        addError(errors, "sample_count must be positive");
    }
    if (request.read_timeout.count() <= 0 || request.read_timeout > limits.maximum_read_timeout) {
        addError(errors, "read_timeout is outside the configured bounds");
    }
    if (limits.maximum_consecutive_timeouts == 0 || limits.maximum_sample_count == 0 ||
        limits.maximum_raw_bytes < kBytesPerComplexSample ||
        !finitePositive(limits.maximum_duration_seconds)) {
        addError(errors, "resource limits are invalid");
    }
    if (request.sample_count.has_value() && *request.sample_count > limits.maximum_sample_count) {
        addError(errors, "sample_count exceeds the configured limit");
    }
    if (request.sample_count.has_value() &&
        *request.sample_count > limits.maximum_raw_bytes / kBytesPerComplexSample) {
        addError(errors, "sample_count exceeds the configured byte limit");
    }
    return errors;
}

ValidationResult makeCapturePlan(
    const CaptureRequest& request,
    const EffectiveSettings& effective,
    const ResourceLimits& limits) {
    ValidationResult result;
    result.errors = validateCaptureRequestBeforeDevice(request, limits);

    const auto settingErrors = evaluateEffectiveSettings(effective, request.setting_policy);
    result.errors.insert(result.errors.end(), settingErrors.begin(), settingErrors.end());
    if (!result.errors.empty()) {
        return result;
    }

    std::uint64_t target = 0;
    std::string rounding = "explicit_sample_count";
    if (request.sample_count.has_value()) {
        target = *request.sample_count;
    } else {
        if (!effective.sample_rate_sps.effective.has_value() ||
            !finitePositive(*effective.sample_rate_sps.effective)) {
            addError(
                result.errors,
                "duration capture requires a finite positive effective sample rate");
            return result;
        }
        const long double product = static_cast<long double>(*request.duration_seconds) *
                                    static_cast<long double>(*effective.sample_rate_sps.effective);
        if (!std::isfinite(product) ||
            product > static_cast<long double>(std::numeric_limits<std::uint64_t>::max())) {
            addError(result.errors, "duration-to-sample conversion overflowed");
            return result;
        }
        target = static_cast<std::uint64_t>(std::ceil(product));
        rounding = "ceil_duration_times_effective_sample_rate";
    }

    if (target == 0 || target > limits.maximum_sample_count) {
        addError(result.errors, "target sample count exceeds the configured limit");
        return result;
    }
    if (target > std::numeric_limits<std::uint64_t>::max() / kBytesPerComplexSample) {
        addError(result.errors, "target byte count overflowed");
        return result;
    }
    const std::uint64_t bytes = target * kBytesPerComplexSample;
    if (bytes > limits.maximum_raw_bytes) {
        addError(result.errors, "target byte count exceeds the configured limit");
        return result;
    }

    CapturePlan plan;
    plan.request = request;
    plan.effective = effective;
    plan.limits = limits;
    plan.target_samples = target;
    plan.target_bytes = bytes;
    plan.rounding_rule = rounding;
    result.plan = std::move(plan);
    return result;
}

std::string toString(SettingPolicy value) {
    return value == SettingPolicy::strict ? "strict" : "permissive";
}

std::string toString(SettingState value) {
    switch (value) {
    case SettingState::applied_verified: return "applied_verified";
    case SettingState::applied_changed: return "applied_changed";
    case SettingState::applied_unverified: return "applied_unverified";
    case SettingState::unsupported: return "unsupported";
    case SettingState::failed: return "failed";
    }
    throw std::logic_error("unknown setting state");
}

std::string toString(CaptureStatus value) {
    switch (value) {
    case CaptureStatus::complete: return "complete";
    case CaptureStatus::partial: return "partial";
    case CaptureStatus::cancelled: return "cancelled";
    case CaptureStatus::failed: return "failed";
    }
    throw std::logic_error("unknown capture status");
}

std::string toString(ErrorCategory value) {
    switch (value) {
    case ErrorCategory::none: return "none";
    case ErrorCategory::validation: return "validation";
    case ErrorCategory::effective_setting: return "effective_setting";
    case ErrorCategory::timeout_limit: return "timeout_limit";
    case ErrorCategory::overflow: return "overflow";
    case ErrorCategory::discontinuity: return "discontinuity";
    case ErrorCategory::end_of_input: return "end_of_input";
    case ErrorCategory::source_error: return "source_error";
    case ErrorCategory::cancellation: return "cancellation";
    case ErrorCategory::raw_open: return "raw_open";
    case ErrorCategory::raw_write: return "raw_write";
    case ErrorCategory::raw_finalize: return "raw_finalize";
    case ErrorCategory::manifest_write: return "manifest_write";
    case ErrorCategory::publication: return "publication";
    case ErrorCategory::cleanup: return "cleanup";
    }
    throw std::logic_error("unknown error category");
}

std::string toString(FinalState value) {
    switch (value) {
    case FinalState::known_safe: return "known_safe";
    case FinalState::known_open: return "known_open";
    case FinalState::unknown: return "unknown";
    }
    throw std::logic_error("unknown final state");
}

} // namespace sdrcal::capture
