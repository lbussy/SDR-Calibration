#include "soapy/SoapyReadTranslator.h"

#include <SoapySDR/Constants.h>
#include <SoapySDR/Errors.h>

#include <optional>
#include <string>
#include <utility>

namespace sdrcal::soapy {

capture::ReadResult translateSoapyRead(
    int resultCode,
    int flags,
    std::int64_t timestampNs,
    std::vector<std::complex<float>> buffer,
    std::size_t maximumSamples) {
    const auto timestamp = (flags & SOAPY_SDR_HAS_TIME) != 0
                               ? std::optional<std::int64_t>(timestampNs)
                               : std::nullopt;
    if (resultCode > 0) {
        const auto count = static_cast<std::size_t>(resultCode);
        if (count > maximumSamples || count > buffer.size()) {
            return {capture::ReadStatus::error,
                    {},
                    timestamp,
                    "SoapySDR returned more samples than requested"};
        }
        buffer.resize(count);
        if ((flags & SOAPY_SDR_END_ABRUPT) != 0) {
            return {capture::ReadStatus::discontinuity,
                    std::move(buffer),
                    timestamp,
                    "SoapySDR marked the read as ending abruptly"};
        }
        return {capture::ReadStatus::samples, std::move(buffer), timestamp, {}};
    }

    buffer.clear();
    switch (resultCode) {
    case SOAPY_SDR_TIMEOUT:
        return {capture::ReadStatus::timeout, {}, timestamp, "SoapySDR read timeout"};
    case SOAPY_SDR_OVERFLOW:
        return {capture::ReadStatus::overflow, {}, timestamp, "SoapySDR RX overflow"};
    case SOAPY_SDR_CORRUPTION:
        return {capture::ReadStatus::discontinuity,
                {},
                timestamp,
                "SoapySDR reported corrupted RX data"};
    case 0:
        return {capture::ReadStatus::error, {}, timestamp, "SoapySDR returned zero samples"};
    default:
        return {capture::ReadStatus::error,
                {},
                timestamp,
                "SoapySDR read failed: " + std::string(SoapySDR_errToStr(resultCode))};
    }
}

} // namespace sdrcal::soapy
