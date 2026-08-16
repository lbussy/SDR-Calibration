#include "capture/MemoryAcquisition.h"

#include <algorithm>
#include <cmath>
#include <exception>
#include <limits>
#include <new>
#include <stdexcept>
#include <string>
#include <utility>

namespace sdrcal::capture {
namespace {

void fail(MemoryAcquisitionResult& result, CaptureStatus status, ErrorCategory category,
          std::string message) {
    result.status = status;
    result.error = {category, std::move(message)};
}

bool nonfinite(const std::vector<std::complex<float>>& samples) {
    return std::ranges::any_of(samples, [](const auto& sample) {
        return !std::isfinite(sample.real()) || !std::isfinite(sample.imag());
    });
}

} // namespace

std::vector<CaptureError> validateMemoryAcquisitionPlan(const CapturePlan& plan,
                                                        const MemoryAcquisitionLimits& limits) {
    std::vector<CaptureError> errors;
    if (plan.target_samples == 0U || plan.target_bytes == 0U ||
        plan.target_samples > std::numeric_limits<std::uint64_t>::max() / kBytesPerComplexSample ||
        plan.target_bytes != plan.target_samples * kBytesPerComplexSample) {
        errors.push_back({ErrorCategory::validation,
                          "memory acquisition target is zero, inconsistent, or overflowed"});
    }
    if (limits.maximum_bytes < kBytesPerComplexSample) {
        errors.push_back({ErrorCategory::validation, "memory byte limit is invalid"});
    } else if (plan.target_samples >
               limits.maximum_bytes / static_cast<std::uint64_t>(sizeof(std::complex<float>))) {
        errors.push_back(
            {ErrorCategory::validation, "memory acquisition target exceeds the memory byte limit"});
    }
    if (plan.target_samples > plan.limits.maximum_sample_count ||
        plan.target_bytes > plan.limits.maximum_raw_bytes) {
        errors.push_back({ErrorCategory::validation,
                          "memory acquisition target exceeds the capture plan limits"});
    }
    if (plan.request.read_timeout.count() <= 0 ||
        plan.request.read_timeout > plan.limits.maximum_read_timeout ||
        plan.limits.maximum_consecutive_timeouts == 0U) {
        errors.push_back(
            {ErrorCategory::validation, "memory acquisition timeout limits are invalid"});
    }
    if (plan.target_samples > static_cast<std::uint64_t>(std::numeric_limits<std::size_t>::max())) {
        errors.push_back(
            {ErrorCategory::validation, "memory acquisition target exceeds addressable storage"});
    }
    return errors;
}

MemoryAcquisitionResult MemoryAcquisition::acquire(const CapturePlan& plan, SampleSource& source,
                                                   MemoryCancellationCheck cancelled,
                                                   const MemoryAcquisitionLimits& limits) const {
    MemoryAcquisitionResult result;
    result.stream.target_samples = plan.target_samples;
    const auto validation = validateMemoryAcquisitionPlan(plan, limits);
    if (!validation.empty()) {
        result.error = validation.front();
    } else if (cancelled && cancelled()) {
        fail(result, CaptureStatus::cancelled, ErrorCategory::cancellation,
             "memory acquisition cancelled before allocation");
    } else {
        try {
            result.samples.reserve(static_cast<std::size_t>(plan.target_samples));
        } catch (const std::bad_alloc&) {
            fail(result, CaptureStatus::failed, ErrorCategory::validation,
                 "memory acquisition allocation failed");
        } catch (const std::length_error&) {
            fail(result, CaptureStatus::failed, ErrorCategory::validation,
                 "memory acquisition allocation exceeds container limits");
        }
    }

    std::uint32_t consecutiveTimeouts = 0U;
    while (result.error.category == ErrorCategory::none &&
           result.samples.size() < plan.target_samples) {
        if (cancelled && cancelled()) {
            fail(result, CaptureStatus::cancelled, ErrorCategory::cancellation,
                 "memory acquisition cancelled");
            break;
        }
        const auto remaining = plan.target_samples - result.samples.size();
        const auto requestCount =
            static_cast<std::size_t>(std::min<std::uint64_t>(remaining, 65'536U));
        ReadResult read;
        try {
            read = source.read(requestCount, plan.request.read_timeout);
        } catch (const std::exception& error) {
            fail(result, result.samples.empty() ? CaptureStatus::failed : CaptureStatus::partial,
                 ErrorCategory::source_error, error.what());
            break;
        } catch (...) {
            fail(result, result.samples.empty() ? CaptureStatus::failed : CaptureStatus::partial,
                 ErrorCategory::source_error, "unknown sample-source exception");
            break;
        }
        ++result.stream.read_calls;
        if (read.samples.size() > requestCount) {
            fail(result, result.samples.empty() ? CaptureStatus::failed : CaptureStatus::partial,
                 ErrorCategory::source_error, "sample source returned more samples than requested");
            break;
        }
        if (read.timestamp_ns) {
            result.stream.timestamps_available = true;
            if (!result.stream.first_timestamp_ns)
                result.stream.first_timestamp_ns = read.timestamp_ns;
            result.stream.last_timestamp_ns = read.timestamp_ns;
        }
        switch (read.status) {
        case ReadStatus::samples:
            consecutiveTimeouts = 0U;
            if (read.samples.empty()) {
                fail(result,
                     result.samples.empty() ? CaptureStatus::failed : CaptureStatus::partial,
                     ErrorCategory::source_error, "sample source returned an empty sample read");
            } else if (nonfinite(read.samples)) {
                fail(result,
                     result.samples.empty() ? CaptureStatus::failed : CaptureStatus::partial,
                     ErrorCategory::source_error, "sample source returned a non-finite sample");
            } else {
                if (read.samples.size() < requestCount)
                    ++result.stream.short_reads;
                result.samples.insert(result.samples.end(), read.samples.begin(),
                                      read.samples.end());
                result.stream.written_samples = static_cast<std::uint64_t>(result.samples.size());
                result.stream.written_bytes =
                    result.stream.written_samples * kBytesPerComplexSample;
            }
            break;
        case ReadStatus::timeout:
            ++result.stream.timeouts;
            ++consecutiveTimeouts;
            if (consecutiveTimeouts >= plan.limits.maximum_consecutive_timeouts)
                fail(result,
                     result.samples.empty() ? CaptureStatus::failed : CaptureStatus::partial,
                     ErrorCategory::timeout_limit, "consecutive timeout limit reached");
            break;
        case ReadStatus::overflow:
            ++result.stream.overflows;
            fail(result, result.samples.empty() ? CaptureStatus::failed : CaptureStatus::partial,
                 ErrorCategory::overflow, read.detail);
            break;
        case ReadStatus::discontinuity:
            ++result.stream.discontinuities;
            fail(result, result.samples.empty() ? CaptureStatus::failed : CaptureStatus::partial,
                 ErrorCategory::discontinuity, read.detail);
            break;
        case ReadStatus::end_of_input:
            fail(result, result.samples.empty() ? CaptureStatus::failed : CaptureStatus::partial,
                 ErrorCategory::end_of_input, read.detail);
            break;
        case ReadStatus::cancelled:
            fail(result, CaptureStatus::cancelled, ErrorCategory::cancellation, read.detail);
            break;
        case ReadStatus::error:
            fail(result, result.samples.empty() ? CaptureStatus::failed : CaptureStatus::partial,
                 ErrorCategory::source_error, read.detail);
            break;
        }
    }

    if (result.error.category == ErrorCategory::none &&
        result.samples.size() == plan.target_samples) {
        result.status = CaptureStatus::complete;
    }

    result.source_cleanup = source.cleanup();
    result.final_state = result.source_cleanup.final_state;
    if (!result.source_cleanup.attempted || !result.source_cleanup.succeeded ||
        result.final_state != FinalState::known_safe) {
        if (result.error.category == ErrorCategory::none) {
            fail(result, result.samples.empty() ? CaptureStatus::failed : CaptureStatus::partial,
                 ErrorCategory::cleanup,
                 result.source_cleanup.detail.empty()
                     ? "sample source cleanup did not establish a known-safe final state"
                     : result.source_cleanup.detail);
        }
    }
    if (!result.succeeded())
        result.samples.clear();
    return result;
}

} // namespace sdrcal::capture
