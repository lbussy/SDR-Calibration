#pragma once

#include "capture/CaptureTypes.h"

#include <complex>
#include <filesystem>
#include <fstream>
#include <span>
#include <string>

namespace sdrcal::capture {

class RawIqWriter {
public:
    RawIqWriter() = default;
    RawIqWriter(const RawIqWriter&) = delete;
    RawIqWriter& operator=(const RawIqWriter&) = delete;
    ~RawIqWriter();

    [[nodiscard]] bool open(const std::filesystem::path& path, std::string& error);
    [[nodiscard]] bool write(std::span<const std::complex<float>> samples, std::string& error);
    [[nodiscard]] bool finalize(std::string& error);
    void abandon() noexcept;
    [[nodiscard]] std::uint64_t samplesWritten() const { return samplesWritten_; }
    [[nodiscard]] std::uint64_t bytesWritten() const { return bytesWritten_; }

private:
    std::ofstream stream_;
    std::filesystem::path path_;
    std::uint64_t samplesWritten_ = 0;
    std::uint64_t bytesWritten_ = 0;
    bool finalized_ = false;
};

} // namespace sdrcal::capture
