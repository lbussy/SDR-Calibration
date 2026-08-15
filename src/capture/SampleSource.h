#pragma once

#include "capture/CaptureTypes.h"

#include <chrono>
#include <cstddef>

namespace sdrcal::capture {

class SampleSource {
public:
    virtual ~SampleSource() = default;
    virtual ReadResult read(std::size_t maximumSamples, std::chrono::milliseconds timeout) = 0;
    virtual CleanupResult cleanup() noexcept = 0;
};

} // namespace sdrcal::capture
