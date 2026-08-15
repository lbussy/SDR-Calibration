#pragma once

#include "capture/CaptureTypes.h"

#include <string>
#include <vector>

namespace sdrcal::capture {

[[nodiscard]] std::vector<CaptureError> validateCaptureRequestBeforeDevice(
    const CaptureRequest& request,
    const ResourceLimits& limits = {});

[[nodiscard]] ValidationResult makeCapturePlan(
    const CaptureRequest& request,
    const EffectiveSettings& effective,
    const ResourceLimits& limits = {});

[[nodiscard]] bool withinTolerance(
    double requested,
    double effective,
    const NumericTolerance& tolerance);

[[nodiscard]] EffectiveSetting classifyEffectiveSetting(
    std::optional<double> requested,
    std::optional<double> effective,
    bool supported,
    bool applicationSucceeded,
    const NumericTolerance& tolerance);

[[nodiscard]] std::vector<CaptureError> evaluateEffectiveSettings(
    const EffectiveSettings& settings,
    SettingPolicy policy);

[[nodiscard]] std::string toString(SettingPolicy value);
[[nodiscard]] std::string toString(SettingState value);
[[nodiscard]] std::string toString(CaptureStatus value);
[[nodiscard]] std::string toString(ErrorCategory value);
[[nodiscard]] std::string toString(FinalState value);

} // namespace sdrcal::capture
