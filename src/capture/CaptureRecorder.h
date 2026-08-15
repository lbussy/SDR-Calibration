#pragma once

#include "capture/CaptureTypes.h"
#include "capture/SampleSource.h"

#include <functional>
#include <string>

namespace sdrcal::capture {

using CancellationCheck = std::function<bool()>;

class CaptureRecorder {
public:
    [[nodiscard]] CaptureResult record(
        const CapturePlan& plan,
        const DeviceMetadata& device,
        SampleSource& source,
        CancellationCheck cancelled = {},
        std::string attemptId = {}) const;
};

} // namespace sdrcal::capture
