#include "capture/CaptureManifestWriter.h"
#include "capture/CapturePlan.h"
#include "capture/CaptureRecorder.h"
#include "capture/RawIqWriter.h"

#include <array>
#include <chrono>
#include <complex>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
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

#define CHECK(condition)                                                                       \
    do {                                                                                       \
        if (!(condition)) {                                                                    \
            throw TestFailure(std::string("check failed: ") + #condition);                     \
        }                                                                                      \
    } while (false)

class TemporaryDirectory {
public:
    explicit TemporaryDirectory(const std::string& name) {
        static std::uint64_t sequence = 0;
        path_ = std::filesystem::temp_directory_path() /
                ("sdrcal-capture-test-" + name + "-" + std::to_string(++sequence));
        std::filesystem::create_directories(path_);
    }
    ~TemporaryDirectory() {
        std::error_code ignored;
        std::filesystem::remove_all(path_, ignored);
    }
    [[nodiscard]] const std::filesystem::path& path() const { return path_; }

private:
    std::filesystem::path path_;
};

class FakeSource final : public SampleSource {
public:
    explicit FakeSource(std::vector<ReadResult> results) : results_(std::move(results)) {}

    ReadResult read(std::size_t maximumSamples, std::chrono::milliseconds) override {
        requested_.push_back(maximumSamples);
        if (index_ >= results_.size()) {
            return {ReadStatus::end_of_input, {}, std::nullopt, "fixture exhausted"};
        }
        return results_[index_++];
    }

    CleanupResult cleanup() noexcept override {
        cleanupCalled_ = true;
        return cleanupResult_;
    }

    CleanupResult cleanupResult_{true, true, "fake source closed"};
    bool cleanupCalled_ = false;
    std::vector<std::size_t> requested_;

private:
    std::vector<ReadResult> results_;
    std::size_t index_ = 0;
};

EffectiveSetting verified(std::optional<double> requested, std::optional<double> effective) {
    return {requested, effective, SettingState::applied_verified};
}

EffectiveSettings effectiveSettings(double rate = 10.0) {
    EffectiveSettings settings;
    settings.center_frequency_hz = verified(1'000'000.0, 1'000'000.0);
    settings.sample_rate_sps = verified(rate, rate);
    settings.bandwidth_hz = {std::nullopt, std::nullopt, SettingState::applied_unverified};
    settings.gain_db = {std::nullopt, std::nullopt, SettingState::applied_unverified};
    return settings;
}

CaptureRequest requestFor(const std::filesystem::path& base, std::uint64_t samples = 4) {
    CaptureRequest request;
    request.device_arguments = {{"driver", "fake"}};
    request.center_frequency_hz = 1'000'000.0;
    request.sample_rate_sps = 10.0;
    request.sample_count = samples;
    request.output_path = base;
    request.read_timeout = 10ms;
    return request;
}

CapturePlan planFor(const std::filesystem::path& base, std::uint64_t samples = 4) {
    auto result = makeCapturePlan(requestFor(base, samples), effectiveSettings());
    CHECK(result.ok());
    return *result.plan;
}

std::vector<std::complex<float>> samples(std::size_t count, float start = 1.0F) {
    std::vector<std::complex<float>> result;
    result.reserve(count);
    for (std::size_t index = 0; index < count; ++index) {
        const auto value = start + static_cast<float>(index);
        result.emplace_back(value, -value);
    }
    return result;
}

std::string readText(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}

void testValidationAndPlanning() {
    TemporaryDirectory directory("planning");
    CaptureRequest request = requestFor(directory.path() / "capture");
    request.sample_count.reset();
    request.duration_seconds = 0.21;
    auto planned = makeCapturePlan(request, effectiveSettings(10.0));
    CHECK(planned.ok());
    CHECK(planned.plan->target_samples == 3);
    CHECK(planned.plan->target_bytes == 24);
    CHECK(planned.plan->rounding_rule == "ceil_duration_times_effective_sample_rate");

    request.sample_count = 3;
    planned = makeCapturePlan(request, effectiveSettings(10.0));
    CHECK(!planned.ok());

    request.duration_seconds.reset();
    request.sample_count = 268'435'457;
    planned = makeCapturePlan(request, effectiveSettings(10.0));
    CHECK(!planned.ok());
}

void testSettingPolicyAndTolerance() {
    CHECK(withinTolerance(1'000'000.0, 1'000'000.5, {1.0, 1.0e-6}));
    CHECK(!withinTolerance(1'000'000.0, 1'000'002.0, {1.0, 1.0e-6}));
    CHECK(classifyEffectiveSetting(10.0, 10.05, true, true, {0.1, 0.0}).state ==
          SettingState::applied_verified);
    CHECK(classifyEffectiveSetting(10.0, 10.2, true, true, {0.1, 0.0}).state ==
          SettingState::applied_changed);
    CHECK(classifyEffectiveSetting(10.0, std::nullopt, true, true, {0.1, 0.0}).state ==
          SettingState::applied_unverified);
    CHECK(classifyEffectiveSetting(10.0, std::nullopt, false, false, {0.1, 0.0}).state ==
          SettingState::unsupported);
    auto settings = effectiveSettings();
    settings.sample_rate_sps.state = SettingState::applied_changed;
    CHECK(evaluateEffectiveSettings(settings, SettingPolicy::strict).size() == 1);
    CHECK(evaluateEffectiveSettings(settings, SettingPolicy::permissive).empty());
    settings.sample_rate_sps.state = SettingState::failed;
    CHECK(evaluateEffectiveSettings(settings, SettingPolicy::permissive).size() == 1);
}

void testRawEncoding() {
    TemporaryDirectory directory("raw");
    const auto path = directory.path() / "raw.cf32";
    RawIqWriter writer;
    std::string error;
    CHECK(writer.open(path, error));
    const std::array<std::complex<float>, 1> values{std::complex<float>{1.0F, -2.0F}};
    CHECK(writer.write(values, error));
    CHECK(writer.finalize(error));
    CHECK(writer.samplesWritten() == 1);
    CHECK(writer.bytesWritten() == 8);
    std::ifstream input(path, std::ios::binary);
    const std::vector<unsigned char> bytes{
        std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
    const std::vector<unsigned char> expected{0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0xc0};
    CHECK(bytes == expected);
}

void testWriterFailureStates() {
    TemporaryDirectory directory("writer-failures");
    RawIqWriter writer;
    std::string error;
    CHECK(!writer.open(directory.path() / "missing" / "raw.cf32", error));

    error.clear();
    CHECK(writer.open(directory.path() / "raw.cf32", error));
    CHECK(writer.finalize(error));
    const std::array<std::complex<float>, 1> values{std::complex<float>{1.0F, 1.0F}};
    CHECK(!writer.write(values, error));
    CHECK(!writer.finalize(error));

    CHECK(!CaptureManifestWriter::write(
        directory.path() / "missing" / "manifest.json", "{}\n", error));
}

void testExactCaptureAndManifest() {
    TemporaryDirectory directory("complete");
    const auto base = directory.path() / "capture";
    FakeSource source({{ReadStatus::samples, samples(4), 123, ""}});
    const auto result = CaptureRecorder{}.record(planFor(base), {}, source, {}, "complete");
    CHECK(result.status == CaptureStatus::complete);
    CHECK(result.stream.written_samples == 4);
    CHECK(result.stream.written_bytes == 32);
    CHECK(result.atomic_raw_publication);
    CHECK(result.atomic_manifest_publication);
    CHECK(std::filesystem::file_size(base.string() + ".cf32") == 32);
    const std::string json = readText(base.string() + ".capture.json");
    CHECK(json.find("\"status\": \"complete\"") != std::string::npos);
    CHECK(json.find("\"written_samples\": 4") != std::string::npos);
    CHECK(json.find("\"byte_count\": 32") != std::string::npos);
    CHECK(source.requested_.size() == 1);
    CHECK(source.requested_[0] == 4);
    CHECK(source.cleanupCalled_);
}

void testShortReadsAndExactBound() {
    TemporaryDirectory directory("short");
    const auto base = directory.path() / "capture";
    FakeSource source({
        {ReadStatus::samples, samples(2), std::nullopt, ""},
        {ReadStatus::samples, samples(2, 3.0F), std::nullopt, ""},
    });
    const auto result = CaptureRecorder{}.record(planFor(base), {}, source, {}, "short");
    CHECK(result.status == CaptureStatus::complete);
    CHECK(result.stream.short_reads == 1);
    CHECK(source.requested_ == std::vector<std::size_t>({4, 2}));
}

void testSourceCannotExceedRequest() {
    TemporaryDirectory directory("excess");
    const auto base = directory.path() / "capture";
    FakeSource source({{ReadStatus::samples, samples(5), std::nullopt, ""}});
    const auto result = CaptureRecorder{}.record(planFor(base), {}, source, {}, "excess");
    CHECK(result.status == CaptureStatus::failed);
    CHECK(result.error.category == ErrorCategory::source_error);
    CHECK(!std::filesystem::exists(base.string() + ".cf32"));
}

void testEmptyReadAndEarlyEnd() {
    TemporaryDirectory directory("empty-read");
    const auto emptyBase = directory.path() / "empty";
    FakeSource emptySource({{ReadStatus::samples, {}, std::nullopt, ""}});
    const auto emptyResult =
        CaptureRecorder{}.record(planFor(emptyBase), {}, emptySource, {}, "empty");
    CHECK(emptyResult.status == CaptureStatus::failed);
    CHECK(emptyResult.error.category == ErrorCategory::source_error);

    const auto endBase = directory.path() / "end";
    FakeSource endSource({
        {ReadStatus::samples, samples(1), std::nullopt, ""},
        {ReadStatus::end_of_input, {}, std::nullopt, "end"},
    });
    const auto endResult = CaptureRecorder{}.record(planFor(endBase), {}, endSource, {}, "end");
    CHECK(endResult.status == CaptureStatus::partial);
    CHECK(endResult.error.category == ErrorCategory::end_of_input);
}

void testTimeoutLimit() {
    TemporaryDirectory directory("timeout");
    const auto base = directory.path() / "capture";
    std::vector<ReadResult> reads(10, {ReadStatus::timeout, {}, std::nullopt, "timeout"});
    FakeSource source(std::move(reads));
    const auto result = CaptureRecorder{}.record(planFor(base), {}, source, {}, "timeout");
    CHECK(result.status == CaptureStatus::failed);
    CHECK(result.error.category == ErrorCategory::timeout_limit);
    CHECK(result.stream.timeouts == 10);
    CHECK(result.stream.read_calls == 10);
}

void testPartialCapture() {
    TemporaryDirectory directory("partial");
    const auto base = directory.path() / "capture";
    FakeSource source({
        {ReadStatus::samples, samples(2), std::nullopt, ""},
        {ReadStatus::discontinuity, {}, std::nullopt, "gap"},
    });
    const auto result = CaptureRecorder{}.record(planFor(base), {}, source, {}, "partial-id");
    CHECK(result.status == CaptureStatus::partial);
    CHECK(result.error.category == ErrorCategory::discontinuity);
    CHECK(result.stream.discontinuities == 1);
    CHECK(result.raw_path.filename() == "capture.incomplete-partial-id.cf32");
    CHECK(result.manifest_path.filename() == "capture.incomplete-partial-id.capture.json");
    CHECK(std::filesystem::file_size(result.raw_path) == 16);
}

void testOverflowCapture() {
    TemporaryDirectory directory("overflow");
    const auto base = directory.path() / "capture";
    FakeSource source({
        {ReadStatus::samples, samples(1), std::nullopt, ""},
        {ReadStatus::overflow, {}, std::nullopt, "overflow"},
    });
    const auto result = CaptureRecorder{}.record(planFor(base), {}, source, {}, "overflow-id");
    CHECK(result.status == CaptureStatus::partial);
    CHECK(result.error.category == ErrorCategory::overflow);
    CHECK(result.stream.overflows == 1);
    CHECK(result.stream.written_samples == 1);
}

void testCancellation() {
    TemporaryDirectory directory("cancel");
    const auto base = directory.path() / "capture";
    FakeSource source({
        {ReadStatus::samples, samples(2), std::nullopt, ""},
        {ReadStatus::samples, samples(2), std::nullopt, ""},
    });
    int checks = 0;
    const auto result = CaptureRecorder{}.record(
        planFor(base), {}, source, [&checks]() { return ++checks > 1; }, "cancel-id");
    CHECK(result.status == CaptureStatus::cancelled);
    CHECK(result.stream.written_samples == 2);
    CHECK(std::filesystem::exists(result.raw_path));
}

void testExistingArtifactIsPreserved() {
    TemporaryDirectory directory("preserve");
    const auto base = directory.path() / "capture";
    {
        std::ofstream existing(base.string() + ".cf32");
        existing << "known-good";
    }
    FakeSource source({{ReadStatus::samples, samples(4), std::nullopt, ""}});
    const auto result = CaptureRecorder{}.record(planFor(base), {}, source, {}, "preserve");
    CHECK(result.status == CaptureStatus::failed);
    CHECK(result.error.category == ErrorCategory::publication);
    CHECK(readText(base.string() + ".cf32") == "known-good");
}

void testUnsafeAttemptIdCannotAlterPath() {
    TemporaryDirectory directory("unsafe-id");
    const auto base = directory.path() / "capture";
    FakeSource source({
        {ReadStatus::samples, samples(1), std::nullopt, ""},
        {ReadStatus::end_of_input, {}, std::nullopt, "end"},
    });
    const auto result = CaptureRecorder{}.record(planFor(base, 2), {}, source, {}, "../escape");
    CHECK(result.status == CaptureStatus::partial);
    CHECK(result.raw_path.parent_path() == directory.path());
    CHECK(result.raw_path.filename().string().find("escape") == std::string::npos);
}

void testCleanupFailureChangesOutcome() {
    TemporaryDirectory directory("cleanup");
    const auto base = directory.path() / "capture";
    FakeSource source({{ReadStatus::samples, samples(4), std::nullopt, ""}});
    source.cleanupResult_ = {true, false, "cleanup failed"};
    const auto result = CaptureRecorder{}.record(planFor(base), {}, source, {}, "cleanup");
    CHECK(result.status == CaptureStatus::partial);
    CHECK(result.error.category == ErrorCategory::cleanup);
    CHECK(result.final_state == FinalState::unknown);
}

void testManifestEscapingAndFiniteNumbers() {
    TemporaryDirectory directory("manifest");
    auto plan = planFor(directory.path() / "capture", 1);
    plan.request.purpose = "quote \" slash \\ newline\n";
    CaptureResult result;
    result.status = CaptureStatus::complete;
    result.stream.target_samples = 1;
    result.stream.written_samples = 1;
    result.stream.written_bytes = 8;
    result.utc_start = "2026-08-09T00:00:00Z";
    result.utc_end = "2026-08-09T00:00:01Z";
    const std::string json = CaptureManifestWriter::serialize(plan, {}, result, "capture.cf32");
    CHECK(json.find("quote \\\" slash \\\\ newline\\n") != std::string::npos);
    plan.request.center_frequency_hz = std::numeric_limits<double>::infinity();
    bool threw = false;
    try {
        static_cast<void>(CaptureManifestWriter::serialize(plan, {}, result, "capture.cf32"));
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    CHECK(threw);
}

} // namespace

int main(int argc, char** argv) {
    if (argc == 3 && std::string(argv[1]) == "--emit-manifest") {
        CapturePlan plan = planFor("capture", 1);
        CaptureResult result;
        result.status = CaptureStatus::complete;
        result.stream.target_samples = 1;
        result.stream.written_samples = 1;
        result.stream.written_bytes = 8;
        result.utc_start = "2026-08-09T00:00:00Z";
        result.utc_end = "2026-08-09T00:00:01Z";
        std::string error;
        const std::string json = CaptureManifestWriter::serialize(plan, {}, result, "capture.cf32");
        return CaptureManifestWriter::write(argv[2], json, error) ? 0 : 1;
    }

    const std::vector<std::pair<std::string, std::function<void()>>> tests{
        {"validation and planning", testValidationAndPlanning},
        {"setting policy and tolerance", testSettingPolicyAndTolerance},
        {"raw encoding", testRawEncoding},
        {"writer failure states", testWriterFailureStates},
        {"exact capture and manifest", testExactCaptureAndManifest},
        {"short reads and exact bound", testShortReadsAndExactBound},
        {"source cannot exceed request", testSourceCannotExceedRequest},
        {"empty read and early end", testEmptyReadAndEarlyEnd},
        {"timeout limit", testTimeoutLimit},
        {"partial capture", testPartialCapture},
        {"overflow capture", testOverflowCapture},
        {"cancellation", testCancellation},
        {"existing artifact preserved", testExistingArtifactIsPreserved},
        {"unsafe attempt id", testUnsafeAttemptIdCannotAlterPath},
        {"cleanup failure", testCleanupFailureChangesOutcome},
        {"manifest escaping", testManifestEscapingAndFiniteNumbers},
    };

    std::size_t failures = 0;
    for (const auto& [name, test] : tests) {
        try {
            test();
            std::cout << "PASS: " << name << '\n';
        } catch (const std::exception& exception) {
            ++failures;
            std::cerr << "FAIL: " << name << ": " << exception.what() << '\n';
        }
    }
    return failures == 0 ? 0 : 1;
}
