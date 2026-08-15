#pragma once

#include "capture/CaptureTypes.h"

#include <complex>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace sdrcal::soapy {

[[nodiscard]] capture::ReadResult translateSoapyRead(
    int resultCode,
    int flags,
    std::int64_t timestampNs,
    std::vector<std::complex<float>> buffer,
    std::size_t maximumSamples);

} // namespace sdrcal::soapy
