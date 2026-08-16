#include "capture/MemoryAcquisition.h"

#include <chrono>
#include <complex>
#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace std::chrono_literals;
using namespace sdrcal::capture;

namespace {

class TestFailure : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

#define CHECK(condition)                                                                           \
    do {                                                                                           \
        if (!(condition))                                                                          \
            throw TestFailure(std::string("check failed: ") + #condition);                         \
    } while (false)

class FakeSource final : public SampleSource {
  public:
    explicit FakeSource(std::vector<ReadResult> reads) : reads_(std::move(reads)) {}

    ReadResult read(std::size_t maximumSamples, std::chrono::milliseconds timeout) override {
        requested_.push_back(maximumSamples);
        timeouts_.push_back(timeout);
        if (throwOnRead_)
            throw std::runtime_error("fake read exception");
        if (next_ >= reads_.size())
            return {ReadStatus::end_of_input, {}, std::nullopt, "fixture exhausted"};
        return reads_[next_++];
    }

    CleanupResult cleanup() noexcept override {
        ++cleanupCalls_;
        return cleanupResult_;
    }

    std::vector<std::size_t> requested_;
    std::vector<std::chrono::milliseconds> timeouts_;
    CleanupResult cleanupResult_{true, true, "known safe", FinalState::known_safe};
    bool throwOnRead_ = false;
    std::size_t cleanupCalls_ = 0U;

  private:
    std::vector<ReadResult> reads_;
    std::size_t next_ = 0U;
};

std::vector<std::complex<float>> samples(std::size_t count, float start = 1.0F) {
    std::vector<std::complex<float>> result;
    result.reserve(count);
    for (std::size_t index = 0; index < count; ++index) {
        const auto value = start + static_cast<float>(index);
        result.emplace_back(value, -value);
    }
    return result;
}

CapturePlan plan(std::uint64_t count = 4U) {
    CapturePlan result;
    result.target_samples = count;
    result.target_bytes = count * kBytesPerComplexSample;
    result.request.read_timeout = 10ms;
    result.limits.maximum_sample_count = 32U;
    result.limits.maximum_raw_bytes = 32U * kBytesPerComplexSample;
    result.limits.maximum_consecutive_timeouts = 2U;
    result.limits.maximum_read_timeout = 100ms;
    return result;
}

void testCompleteAndShortReads() {
    FakeSource source({{ReadStatus::samples, samples(2), 100, ""},
                       {ReadStatus::samples, samples(2, 3.0F), 200, ""}});
    const auto result = MemoryAcquisition{}.acquire(plan(), source);
    CHECK(result.succeeded());
    CHECK(result.samples == samples(4));
    CHECK(result.stream.target_samples == 4U);
    CHECK(result.stream.written_samples == 4U);
    CHECK(result.stream.written_bytes == 32U);
    CHECK(result.stream.short_reads == 1U);
    CHECK(result.stream.read_calls == 2U);
    CHECK(result.stream.first_timestamp_ns == 100);
    CHECK(result.stream.last_timestamp_ns == 200);
    CHECK(source.requested_ == std::vector<std::size_t>({4U, 2U}));
    CHECK(source.timeouts_ == std::vector<std::chrono::milliseconds>({10ms, 10ms}));
    CHECK(source.cleanupCalls_ == 1U);
}

void testPreflightLimits() {
    auto invalid = plan();
    invalid.target_bytes = 31U;
    CHECK(!validateMemoryAcquisitionPlan(invalid).empty());

    FakeSource source({{ReadStatus::samples, samples(4), std::nullopt, ""}});
    const auto limited = MemoryAcquisition{}.acquire(plan(), source, {}, {24U});
    CHECK(!limited.succeeded());
    CHECK(limited.error.category == ErrorCategory::validation);
    CHECK(limited.samples.empty());
    CHECK(source.requested_.empty());
    CHECK(source.cleanupCalls_ == 1U);
}

void testNonfiniteAndOversizedReads() {
    auto invalidSamples = samples(2);
    invalidSamples[1] = {std::numeric_limits<float>::infinity(), 0.0F};
    FakeSource nonfiniteSource(
        {{ReadStatus::samples, std::move(invalidSamples), std::nullopt, ""}});
    const auto nonfinite = MemoryAcquisition{}.acquire(plan(2U), nonfiniteSource);
    CHECK(nonfinite.status == CaptureStatus::failed);
    CHECK(nonfinite.error.category == ErrorCategory::source_error);
    CHECK(nonfinite.samples.empty());
    CHECK(nonfiniteSource.cleanupCalls_ == 1U);

    FakeSource oversizedSource({{ReadStatus::samples, samples(5), std::nullopt, ""}});
    const auto oversized = MemoryAcquisition{}.acquire(plan(), oversizedSource);
    CHECK(oversized.status == CaptureStatus::failed);
    CHECK(oversized.error.category == ErrorCategory::source_error);
    CHECK(oversized.samples.empty());
}

void testTimeoutAndDiscontinuity() {
    FakeSource timeoutSource({{ReadStatus::timeout, {}, std::nullopt, "timeout"},
                              {ReadStatus::timeout, {}, std::nullopt, "timeout"}});
    const auto timeout = MemoryAcquisition{}.acquire(plan(), timeoutSource);
    CHECK(timeout.status == CaptureStatus::failed);
    CHECK(timeout.error.category == ErrorCategory::timeout_limit);
    CHECK(timeout.stream.timeouts == 2U);
    CHECK(timeout.samples.empty());

    FakeSource discontinuitySource(
        {{ReadStatus::samples, samples(2), std::nullopt, ""},
         {ReadStatus::discontinuity, {}, std::nullopt, "timestamp gap"}});
    const auto discontinuity = MemoryAcquisition{}.acquire(plan(), discontinuitySource);
    CHECK(discontinuity.status == CaptureStatus::partial);
    CHECK(discontinuity.error.category == ErrorCategory::discontinuity);
    CHECK(discontinuity.stream.discontinuities == 1U);
    CHECK(discontinuity.stream.written_samples == 2U);
    CHECK(discontinuity.samples.empty());
}

void testCancellationAndException() {
    FakeSource beforeSource({{ReadStatus::samples, samples(4), std::nullopt, ""}});
    const auto before = MemoryAcquisition{}.acquire(plan(), beforeSource, [] { return true; });
    CHECK(before.status == CaptureStatus::cancelled);
    CHECK(beforeSource.requested_.empty());
    CHECK(beforeSource.cleanupCalls_ == 1U);

    FakeSource betweenSource({{ReadStatus::samples, samples(2), std::nullopt, ""},
                              {ReadStatus::samples, samples(2), std::nullopt, ""}});
    std::size_t checks = 0U;
    const auto between =
        MemoryAcquisition{}.acquire(plan(), betweenSource, [&] { return ++checks == 3U; });
    CHECK(between.status == CaptureStatus::cancelled);
    CHECK(between.stream.written_samples == 2U);
    CHECK(between.samples.empty());
    CHECK(betweenSource.cleanupCalls_ == 1U);

    FakeSource exceptionSource({});
    exceptionSource.throwOnRead_ = true;
    const auto exception = MemoryAcquisition{}.acquire(plan(), exceptionSource);
    CHECK(exception.status == CaptureStatus::failed);
    CHECK(exception.error.category == ErrorCategory::source_error);
    CHECK(exception.samples.empty());
    CHECK(exceptionSource.cleanupCalls_ == 1U);
}

void testCleanupMustEstablishKnownSafe() {
    FakeSource unattemptedCleanup({{ReadStatus::samples, samples(4), std::nullopt, ""}});
    unattemptedCleanup.cleanupResult_ = {false, true, "not attempted", FinalState::known_safe};
    const auto unattempted = MemoryAcquisition{}.acquire(plan(), unattemptedCleanup);
    CHECK(unattempted.status == CaptureStatus::partial);
    CHECK(unattempted.error.category == ErrorCategory::cleanup);
    CHECK(unattempted.samples.empty());

    FakeSource failedCleanup({{ReadStatus::samples, samples(4), std::nullopt, ""}});
    failedCleanup.cleanupResult_ = {true, false, "release failed", FinalState::unknown};
    const auto failed = MemoryAcquisition{}.acquire(plan(), failedCleanup);
    CHECK(failed.status == CaptureStatus::partial);
    CHECK(failed.error.category == ErrorCategory::cleanup);
    CHECK(failed.samples.empty());

    FakeSource unknownCleanup({{ReadStatus::samples, samples(4), std::nullopt, ""}});
    unknownCleanup.cleanupResult_ = {true, true, "release returned", FinalState::unknown};
    const auto unknown = MemoryAcquisition{}.acquire(plan(), unknownCleanup);
    CHECK(unknown.status == CaptureStatus::partial);
    CHECK(unknown.error.category == ErrorCategory::cleanup);
    CHECK(unknown.samples.empty());
}

} // namespace

int main() {
    try {
        testCompleteAndShortReads();
        testPreflightLimits();
        testNonfiniteAndOversizedReads();
        testTimeoutAndDiscontinuity();
        testCancellationAndException();
        testCleanupMustEstablishKnownSafe();
        std::cout << "memory acquisition tests passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
