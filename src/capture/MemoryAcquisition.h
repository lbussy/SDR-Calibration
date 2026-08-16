#pragma once

#include "capture/CaptureTypes.h"
#include "capture/SampleSource.h"

#include <cstdint>
#include <functional>
#include <vector>

namespace sdrcal::capture {

struct MemoryAcquisitionLimits {
    std::uint64_t maximum_bytes = 512U * 1024U * 1024U;
};

struct MemoryAcquisitionResult {
    CaptureStatus status = CaptureStatus::failed;
    CaptureError error;
    StreamStatistics stream;
    CleanupResult source_cleanup;
    FinalState final_state = FinalState::unknown;
    std::vector<std::complex<float>> samples;

    [[nodiscard]] bool succeeded() const noexcept {
        return status == CaptureStatus::complete && error.category == ErrorCategory::none &&
               source_cleanup.attempted && source_cleanup.succeeded &&
               final_state == FinalState::known_safe && samples.size() == stream.target_samples;
    }
};

using MemoryCancellationCheck = std::function<bool()>;

[[nodiscard]] std::vector<CaptureError>
validateMemoryAcquisitionPlan(const CapturePlan& plan, const MemoryAcquisitionLimits& limits = {});

[[nodiscard]] std::vector<CaptureError> validateMemoryAcquisitionRequestBeforeDevice(
    const CaptureRequest& request, const ResourceLimits& resource_limits = {},
    const MemoryAcquisitionLimits& memory_limits = {}, const SettingTolerances& tolerances = {});

class MemoryAcquisition {
  public:
    [[nodiscard]] MemoryAcquisitionResult acquire(const CapturePlan& plan, SampleSource& source,
                                                  MemoryCancellationCheck cancelled = {},
                                                  const MemoryAcquisitionLimits& limits = {}) const;
};

} // namespace sdrcal::capture
