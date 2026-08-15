#include "capture/CaptureRecorder.h"

#include "capture/CaptureManifestWriter.h"
#include "capture/CapturePlan.h"
#include "capture/RawIqWriter.h"

#include <algorithm>
#include <atomic>
#include <cctype>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <system_error>

namespace sdrcal::capture {
namespace {

std::string utcNow() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm utc{};
#ifdef _WIN32
    gmtime_s(&utc, &time);
#else
    gmtime_r(&time, &utc);
#endif
    std::ostringstream output;
    output << std::put_time(&utc, "%Y-%m-%dT%H:%M:%SZ");
    return output.str();
}

std::string generatedAttemptId() {
    static std::atomic<std::uint64_t> counter{0};
    const auto ticks = std::chrono::steady_clock::now().time_since_epoch().count();
    std::ostringstream output;
    output << std::hex << ticks << '-' << counter.fetch_add(1, std::memory_order_relaxed);
    return output.str();
}

std::string safeAttemptId(const std::string& requested) {
    if (requested.empty()) {
        return generatedAttemptId();
    }
    const bool safe = std::all_of(requested.begin(), requested.end(), [](const char character) {
        const auto value = static_cast<unsigned char>(character);
        return std::isalnum(value) != 0 || character == '-' || character == '_';
    });
    return safe ? requested : generatedAttemptId();
}

std::filesystem::path withSuffix(const std::filesystem::path& base, const std::string& suffix) {
    return std::filesystem::path(base.string() + suffix);
}

bool publishWithoutOverwrite(
    const std::filesystem::path& temporary,
    const std::filesystem::path& destination,
    std::string& error) {
    std::error_code code;
    if (std::filesystem::exists(destination, code) || code) {
        error = code ? "cannot inspect destination: " + code.message()
                     : "destination already exists: " + destination.string();
        return false;
    }
    std::filesystem::rename(temporary, destination, code);
    if (code) {
        error = "cannot publish artifact: " + code.message();
        return false;
    }
    return true;
}

void removeExact(const std::filesystem::path& path) noexcept {
    std::error_code ignored;
    std::filesystem::remove(path, ignored);
}

void setTerminal(
    CaptureResult& result,
    CaptureStatus status,
    ErrorCategory category,
    std::string message) {
    result.status = status;
    result.error = {category, std::move(message)};
}

} // namespace

CaptureResult CaptureRecorder::record(
    const CapturePlan& plan,
    const DeviceMetadata& device,
    SampleSource& source,
    CancellationCheck cancelled,
    std::string attemptId) const {
    CaptureResult result;
    result.stream.target_samples = plan.target_samples;
    result.utc_start = utcNow();
    const auto monotonicStart = std::chrono::steady_clock::now();
    attemptId = safeAttemptId(attemptId);

    const auto completeRaw = withSuffix(plan.request.output_path, ".cf32");
    const auto completeManifest = withSuffix(plan.request.output_path, ".capture.json");
    const auto temporaryRaw = withSuffix(plan.request.output_path, ".cf32.part");
    const auto temporaryManifest = withSuffix(plan.request.output_path, ".capture.json.part");
    const auto incompleteBase = withSuffix(plan.request.output_path, ".incomplete-" + attemptId);
    const auto incompleteRaw = withSuffix(incompleteBase, ".cf32");
    const auto incompleteManifest = withSuffix(incompleteBase, ".capture.json");
    const auto incompleteManifestTemporary = withSuffix(incompleteBase, ".capture.json.part");

    RawIqWriter writer;
    std::string error;
    if (std::filesystem::exists(completeRaw) || std::filesystem::exists(completeManifest) ||
        std::filesystem::exists(temporaryRaw) || std::filesystem::exists(temporaryManifest)) {
        setTerminal(result, CaptureStatus::failed, ErrorCategory::publication,
                    "complete or temporary destination already exists");
    } else if (!writer.open(temporaryRaw, error)) {
        setTerminal(result, CaptureStatus::failed, ErrorCategory::raw_open, error);
    } else {
        std::uint32_t consecutiveTimeouts = 0;
        while (result.stream.written_samples < plan.target_samples &&
               result.error.category == ErrorCategory::none) {
            if (cancelled && cancelled()) {
                setTerminal(
                    result,
                    CaptureStatus::cancelled,
                    ErrorCategory::cancellation,
                    "capture cancelled");
                break;
            }
            const auto remaining = plan.target_samples - result.stream.written_samples;
            const auto requestCount = static_cast<std::size_t>(
                std::min<std::uint64_t>(remaining, 65'536));
            ReadResult read = source.read(requestCount, plan.request.read_timeout);
            ++result.stream.read_calls;

            if (read.samples.size() > requestCount) {
                setTerminal(result, CaptureStatus::partial, ErrorCategory::source_error,
                            "source returned more samples than requested");
                break;
            }
            if (read.timestamp_ns.has_value()) {
                result.stream.timestamps_available = true;
                if (!result.stream.first_timestamp_ns.has_value()) {
                    result.stream.first_timestamp_ns = read.timestamp_ns;
                }
                result.stream.last_timestamp_ns = read.timestamp_ns;
            }

            switch (read.status) {
            case ReadStatus::samples:
                consecutiveTimeouts = 0;
                if (read.samples.empty()) {
                    setTerminal(result, CaptureStatus::partial, ErrorCategory::source_error,
                                "source returned an empty sample read");
                    break;
                }
                if (read.samples.size() < requestCount) {
                    ++result.stream.short_reads;
                }
                if (!writer.write(read.samples, error)) {
                    setTerminal(result, CaptureStatus::partial, ErrorCategory::raw_write, error);
                    break;
                }
                result.stream.written_samples = writer.samplesWritten();
                result.stream.written_bytes = writer.bytesWritten();
                break;
            case ReadStatus::timeout:
                ++result.stream.timeouts;
                ++consecutiveTimeouts;
                if (consecutiveTimeouts >= plan.limits.maximum_consecutive_timeouts) {
                    setTerminal(result, CaptureStatus::partial, ErrorCategory::timeout_limit,
                                "consecutive timeout limit reached");
                }
                break;
            case ReadStatus::overflow:
                ++result.stream.overflows;
                setTerminal(result, CaptureStatus::partial, ErrorCategory::overflow, read.detail);
                break;
            case ReadStatus::discontinuity:
                ++result.stream.discontinuities;
                setTerminal(
                    result, CaptureStatus::partial, ErrorCategory::discontinuity, read.detail);
                break;
            case ReadStatus::end_of_input:
                setTerminal(
                    result, CaptureStatus::partial, ErrorCategory::end_of_input, read.detail);
                break;
            case ReadStatus::cancelled:
                setTerminal(
                    result, CaptureStatus::cancelled, ErrorCategory::cancellation, read.detail);
                break;
            case ReadStatus::error:
                setTerminal(
                    result, CaptureStatus::partial, ErrorCategory::source_error, read.detail);
                break;
            }
        }

        if (result.error.category == ErrorCategory::none &&
            result.stream.written_samples == plan.target_samples) {
            result.status = CaptureStatus::complete;
        } else if (result.stream.written_samples == 0 && result.status == CaptureStatus::partial) {
            result.status = CaptureStatus::failed;
        }

        result.raw_cleanup.attempted = true;
        if (writer.finalize(error)) {
            result.raw_cleanup.succeeded = true;
            result.raw_cleanup.detail = "raw output flushed and closed";
        } else {
            result.raw_cleanup.detail = error;
            if (result.error.category == ErrorCategory::none) {
                setTerminal(result, CaptureStatus::partial, ErrorCategory::raw_finalize, error);
            }
        }
    }

    result.source_cleanup = source.cleanup();
    result.final_state = result.source_cleanup.final_state;
    if (!result.source_cleanup.succeeded) {
        if (result.error.category == ErrorCategory::none) {
            setTerminal(
                result,
                result.stream.written_samples > 0 ? CaptureStatus::partial : CaptureStatus::failed,
                ErrorCategory::cleanup,
                result.source_cleanup.detail);
        }
    } else {
        if (result.final_state == FinalState::unknown) {
            result.final_state = FinalState::known_safe;
        }
    }
    result.utc_end = utcNow();
    result.monotonic_elapsed_seconds =
        std::chrono::duration<double>(std::chrono::steady_clock::now() - monotonicStart).count();

    std::filesystem::path rawPublication;
    std::filesystem::path manifestPublication;
    std::filesystem::path manifestTemporary;
    if (result.status == CaptureStatus::complete) {
        rawPublication = completeRaw;
        manifestPublication = completeManifest;
        manifestTemporary = temporaryManifest;
    } else if (result.stream.written_samples > 0 && result.raw_cleanup.succeeded) {
        rawPublication = incompleteRaw;
        manifestPublication = incompleteManifest;
        manifestTemporary = incompleteManifestTemporary;
    }

    if (!rawPublication.empty()) {
        if (!publishWithoutOverwrite(temporaryRaw, rawPublication, error)) {
            removeExact(temporaryRaw);
            setTerminal(result, CaptureStatus::failed, ErrorCategory::publication, error);
        } else {
            result.atomic_raw_publication = true;
            result.raw_path = rawPublication;
            const std::string json =
                CaptureManifestWriter::serialize(plan, device, result, rawPublication);
            if (!CaptureManifestWriter::write(manifestTemporary, json, error) ||
                !publishWithoutOverwrite(manifestTemporary, manifestPublication, error)) {
                removeExact(manifestTemporary);
                removeExact(rawPublication);
                result.raw_path.clear();
                result.atomic_raw_publication = false;
                setTerminal(result, CaptureStatus::failed, ErrorCategory::manifest_write, error);
            } else {
                result.atomic_manifest_publication = true;
                result.manifest_path = manifestPublication;
                result.manifest_cleanup = {true, true, "manifest flushed, closed, and published"};
            }
        }
    } else {
        removeExact(temporaryRaw);
        removeExact(temporaryManifest);
    }

    return result;
}

} // namespace sdrcal::capture
