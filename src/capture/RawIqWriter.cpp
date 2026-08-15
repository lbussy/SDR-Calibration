#include "capture/RawIqWriter.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <limits>

namespace sdrcal::capture {
namespace {

static_assert(sizeof(float) == 4);
static_assert(std::numeric_limits<float>::is_iec559);

void encodeFloatLittleEndian(float value, std::byte* output) {
    const std::uint32_t bits = std::bit_cast<std::uint32_t>(value);
    output[0] = static_cast<std::byte>(bits & 0xffU);
    output[1] = static_cast<std::byte>((bits >> 8U) & 0xffU);
    output[2] = static_cast<std::byte>((bits >> 16U) & 0xffU);
    output[3] = static_cast<std::byte>((bits >> 24U) & 0xffU);
}

} // namespace

RawIqWriter::~RawIqWriter() { abandon(); }

bool RawIqWriter::open(const std::filesystem::path& path, std::string& error) {
    if (stream_.is_open()) {
        error = "raw writer is already open";
        return false;
    }
    path_ = path;
    stream_.open(path_, std::ios::binary | std::ios::out | std::ios::trunc);
    if (!stream_) {
        error = "cannot open raw output: " + path_.string();
        return false;
    }
    return true;
}

bool RawIqWriter::write(std::span<const std::complex<float>> samples, std::string& error) {
    if (!stream_.is_open() || finalized_) {
        error = "raw writer is not writable";
        return false;
    }
    std::array<std::byte, 8 * 1024> buffer{};
    std::size_t offset = 0;
    while (offset < samples.size()) {
        const std::size_t count = std::min<std::size_t>(1024, samples.size() - offset);
        for (std::size_t index = 0; index < count; ++index) {
            encodeFloatLittleEndian(samples[offset + index].real(), buffer.data() + index * 8);
            encodeFloatLittleEndian(samples[offset + index].imag(), buffer.data() + index * 8 + 4);
        }
        const auto byteCount = static_cast<std::streamsize>(count * 8);
        stream_.write(reinterpret_cast<const char*>(buffer.data()), byteCount);
        if (!stream_) {
            error = "failed writing raw output";
            return false;
        }
        samplesWritten_ += static_cast<std::uint64_t>(count);
        bytesWritten_ += static_cast<std::uint64_t>(count) * kBytesPerComplexSample;
        offset += count;
    }
    return true;
}

bool RawIqWriter::finalize(std::string& error) {
    if (!stream_.is_open() || finalized_) {
        error = "raw writer is not open for finalization";
        return false;
    }
    stream_.flush();
    if (!stream_) {
        error = "failed flushing raw output";
        return false;
    }
    stream_.close();
    if (stream_.fail()) {
        error = "failed closing raw output";
        return false;
    }
    finalized_ = true;
    return true;
}

void RawIqWriter::abandon() noexcept {
    if (stream_.is_open()) {
        stream_.close();
    }
}

} // namespace sdrcal::capture
