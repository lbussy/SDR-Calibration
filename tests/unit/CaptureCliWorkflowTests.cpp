#include "cli/CaptureCliArguments.h"
#include "cli/CaptureCliWorkflow.h"

#include <algorithm>
#include <chrono>
#include <complex>
#include <filesystem>
#include <functional>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace sdrcal::capture;
using namespace sdrcal::cli;
using namespace sdrcal::soapy;

namespace {
class TestFailure : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};
#define CHECK(condition)                                                                       \
    do {                                                                                       \
        if (!(condition)) {                                                                    \
            throw TestFailure(std::string("check failed: ") + #condition);                    \
        }                                                                                      \
    } while (false)

class FakeDevice final : public SoapyDevice {
public:
    std::string driverKey() const override { return "fake"; }
    std::string hardwareKey() const override { return "fake-hardware"; }
    KeywordMap hardwareInfo() const override { return {{"serial", "test-only"}}; }
    std::optional<std::string> antenna(std::size_t) const override { return "fake-input"; }
    std::optional<std::string> clockSource() const override { return "fake-clock"; }
    bool hasFrequencyCorrection(std::size_t) const override { return false; }
    std::optional<double> frequencyCorrection(std::size_t) const override { return {}; }
    void setSampleRate(std::size_t, double value) override { sampleRate_ = value; }
    std::optional<double> sampleRate(std::size_t) const override { return sampleRate_; }
    bool supportsBandwidth(std::size_t) const override { return true; }
    void setBandwidth(std::size_t, double value) override { bandwidth_ = value; }
    std::optional<double> bandwidth(std::size_t) const override { return bandwidth_; }
    void setFrequency(std::size_t, double value) override { frequency_ = value; }
    std::optional<double> frequency(std::size_t) const override { return frequency_; }
    bool supportsGain(std::size_t) const override { return true; }
    bool hasGainMode(std::size_t) const override { return true; }
    void setAutomaticGain(std::size_t, bool automatic) override { automaticGain_ = automatic; }
    std::optional<bool> automaticGain(std::size_t) const override { return automaticGain_; }
    void setGain(std::size_t, double value) override { gain_ = value; }
    std::optional<double> gain(std::size_t) const override { return gain_; }
    std::size_t setupRxStream(std::size_t) override { return 16; }
    void activateRxStream() override { active_ = true; }
    ReadResult readRxStream(std::size_t maximum, std::chrono::milliseconds) override {
        const std::size_t count = std::min(maximum, remaining_);
        remaining_ -= count;
        return {ReadStatus::samples,
                std::vector<std::complex<float>>(count, {1.0F, -1.0F}),
                std::nullopt,
                {}};
    }
    void deactivateRxStream() override { active_ = false; }
    void closeRxStream() override {}

    std::optional<double> sampleRate_;
    std::optional<double> bandwidth_;
    std::optional<double> frequency_;
    std::optional<double> gain_;
    std::optional<bool> automaticGain_ = true;
    std::size_t remaining_ = 4;
    bool active_ = false;
};

class FakeApi final : public SoapyApi {
public:
    std::vector<KeywordMap> enumerate(const KeywordMap&) override {
        ++enumerateCalls_;
        return noMatches_ ? std::vector<KeywordMap>{}
                          : std::vector<KeywordMap>{{{"driver", "fake"}, {"serial", "test-only"}}};
    }
    SoapyDevice* make(const KeywordMap&) override { return &device_; }
    void unmake(SoapyDevice* device) override { CHECK(device == &device_); }
    std::string libraryVersion() const override { return "fake-library"; }
    std::string apiVersion() const override { return "fake-api"; }
    std::string abiVersion() const override { return "fake-abi"; }

    FakeDevice device_;
    std::size_t enumerateCalls_ = 0;
    bool noMatches_ = false;
};

std::filesystem::path uniqueOutput() {
    const auto tick = std::chrono::steady_clock::now().time_since_epoch().count();
    return std::filesystem::temp_directory_path() /
           ("sdrcal-cli-workflow-" + std::to_string(tick));
}

std::vector<std::string> arguments(const std::filesystem::path& output) {
    return {"--device", "driver=fake", "--frequency-hz", "10000000", "--sample-rate",
            "2000000", "--bandwidth", "1800000", "--gain", "20", "--samples", "4",
            "--output", output.string()};
}

void testInvalidRequestNeverTouchesApi() {
    FakeApi api;
    std::ostringstream output;
    std::ostringstream error;
    const auto parsed = parseCaptureArguments({"--frequency-hz", "1"});
    CHECK(runCaptureCommand(parsed, api, output, error) == ExitStatus::usage);
    CHECK(api.enumerateCalls_ == 0);
}

void testPreparationFailureIsReported() {
    FakeApi api;
    api.noMatches_ = true;
    std::ostringstream output;
    std::ostringstream error;
    CHECK(runCaptureCommand(parseCaptureArguments(arguments(uniqueOutput())), api, output, error) ==
          ExitStatus::preparation);
    CHECK(api.enumerateCalls_ == 1);
    CHECK(error.str().find("selection") != std::string::npos);
}

void testCompleteWorkflowReportsRequestedAndEffective() {
    const auto base = uniqueOutput();
    FakeApi api;
    std::ostringstream output;
    std::ostringstream error;
    CHECK(runCaptureCommand(parseCaptureArguments(arguments(base)), api, output, error) ==
          ExitStatus::success);
    CHECK(output.str().find("Requested settings:") != std::string::npos);
    CHECK(output.str().find("Effective settings:") != std::string::npos);
    CHECK(output.str().find("status: complete") != std::string::npos);
    CHECK(std::filesystem::file_size(base.string() + ".cf32") == 32);
    CHECK(std::filesystem::exists(base.string() + ".capture.json"));
    std::filesystem::remove(base.string() + ".cf32");
    std::filesystem::remove(base.string() + ".capture.json");
}

void testCancellationIsNotSuccess() {
    const auto base = uniqueOutput();
    FakeApi api;
    std::ostringstream output;
    std::ostringstream error;
    CHECK(runCaptureCommand(
              parseCaptureArguments(arguments(base)), api, output, error, []() { return true; }) ==
          ExitStatus::capture);
    CHECK(error.str().find("cancellation") != std::string::npos);
    const auto parent = base.parent_path();
    const auto prefix = base.filename().string() + ".incomplete-";
    for (const auto& entry : std::filesystem::directory_iterator(parent)) {
        if (entry.path().filename().string().starts_with(prefix)) {
            std::filesystem::remove(entry.path());
        }
    }
}
} // namespace

int main() {
    const std::vector<std::pair<std::string, std::function<void()>>> tests{
        {"invalid request is pre-device", testInvalidRequestNeverTouchesApi},
        {"preparation failure", testPreparationFailureIsReported},
        {"complete workflow", testCompleteWorkflowReportsRequestedAndEffective},
        {"cancellation", testCancellationIsNotSuccess},
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
