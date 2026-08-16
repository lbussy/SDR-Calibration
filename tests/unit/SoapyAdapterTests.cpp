#include "capture/CapturePlan.h"
#include "soapy/SoapyCaptureSession.h"
#include "soapy/SoapyReadTranslator.h"
#include "soapy/SoapyWorkflowBoundary.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <complex>
#include <cstddef>
#include <functional>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace std::chrono_literals;
using namespace sdrcal::capture;
using namespace sdrcal::soapy;

namespace {

constexpr int kSoapyTimeout = -1;
constexpr int kSoapyStreamError = -2;
constexpr int kSoapyCorruption = -3;
constexpr int kSoapyOverflow = -4;
constexpr int kSoapyHasTime = 1 << 2;
constexpr int kSoapyEndAbrupt = 1 << 3;

class TestFailure : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

#define CHECK(condition)                                                                           \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            throw TestFailure(std::string("check failed: ") + #condition);                         \
        }                                                                                          \
    } while (false)

class FakeDevice final : public SoapyDevice {
  public:
    [[nodiscard]] std::string driverKey() const override {
        return driverKey_;
    }
    [[nodiscard]] std::string hardwareKey() const override {
        return hardwareKey_;
    }
    [[nodiscard]] KeywordMap hardwareInfo() const override {
        return hardwareInfo_;
    }
    [[nodiscard]] std::optional<std::string> antenna(std::size_t) const override {
        return antenna_;
    }
    [[nodiscard]] std::optional<std::string> clockSource() const override {
        return clockSource_;
    }
    [[nodiscard]] bool hasFrequencyCorrection(std::size_t) const override {
        return frequencyCorrection_.has_value();
    }
    [[nodiscard]] std::optional<double> frequencyCorrection(std::size_t) const override {
        return frequencyCorrection_;
    }

    void setSampleRate(std::size_t, double value) override {
        operation("set_sample_rate");
        requestedSampleRate_ = value;
    }
    [[nodiscard]] std::optional<double> sampleRate(std::size_t) const override {
        const_cast<FakeDevice*>(this)->operation("get_sample_rate");
        return sampleRate_;
    }
    [[nodiscard]] bool supportsBandwidth(std::size_t) const override {
        const_cast<FakeDevice*>(this)->operation("supports_bandwidth");
        return bandwidthSupported_;
    }
    void setBandwidth(std::size_t, double value) override {
        operation("set_bandwidth");
        requestedBandwidth_ = value;
    }
    [[nodiscard]] std::optional<double> bandwidth(std::size_t) const override {
        const_cast<FakeDevice*>(this)->operation("get_bandwidth");
        return bandwidth_;
    }
    void setFrequency(std::size_t, double value) override {
        operation("set_frequency");
        requestedFrequency_ = value;
        if (antennaAfterFrequency_.has_value()) {
            antenna_ = antennaAfterFrequency_;
        }
    }
    [[nodiscard]] std::optional<double> frequency(std::size_t) const override {
        const_cast<FakeDevice*>(this)->operation("get_frequency");
        return frequency_;
    }
    [[nodiscard]] bool supportsGain(std::size_t) const override {
        const_cast<FakeDevice*>(this)->operation("supports_gain");
        return gainSupported_;
    }
    [[nodiscard]] bool hasGainMode(std::size_t) const override {
        const_cast<FakeDevice*>(this)->operation("has_gain_mode");
        return gainModeSupported_;
    }
    void setAutomaticGain(std::size_t, bool automatic) override {
        operation("set_gain_mode");
        if (!preserveAutomaticGainReadback_) {
            automaticGain_ = automatic;
        }
    }
    [[nodiscard]] std::optional<bool> automaticGain(std::size_t) const override {
        const_cast<FakeDevice*>(this)->operation("get_gain_mode");
        return automaticGain_;
    }
    void setGain(std::size_t, double value) override {
        operation("set_gain");
        requestedGain_ = value;
    }
    [[nodiscard]] std::optional<double> gain(std::size_t) const override {
        const_cast<FakeDevice*>(this)->operation("get_gain");
        return gain_;
    }

    [[nodiscard]] std::size_t setupRxStream(std::size_t channel) override {
        operation("setup_stream");
        if (mtu_ == 0) {
            throw std::runtime_error("zero MTU");
        }
        setupChannel_ = channel;
        streamCreated_ = true;
        return mtu_;
    }
    void activateRxStream() override {
        operation("activate_stream");
        active_ = true;
    }
    [[nodiscard]] ReadResult readRxStream(std::size_t maximumSamples,
                                          std::chrono::milliseconds timeout) override {
        operation("read_stream");
        lastMaximumSamples_ = maximumSamples;
        lastTimeout_ = timeout;
        if (reads_.empty()) {
            return {ReadStatus::timeout, {}, std::nullopt, "empty fake queue"};
        }
        ReadResult result = std::move(reads_.front());
        reads_.erase(reads_.begin());
        return result;
    }
    void deactivateRxStream() override {
        operation("deactivate_stream");
        active_ = false;
    }
    void closeRxStream() override {
        operation("close_stream");
        streamCreated_ = false;
    }

    void operation(const std::string& name) {
        operations_.push_back(name);
        if (failOperation_ == name) {
            throw std::runtime_error(name + " failed");
        }
        if (unsupportedOperation_ == name) {
            throw OperationError(OperationFailure::unsupported, name + " unsupported");
        }
    }

    std::string driverKey_ = "fake-driver";
    std::string hardwareKey_ = "fake-hardware";
    KeywordMap hardwareInfo_{{"serial", "hardware-serial"}, {"manufacturer", "Test Manufacturer"},
                             {"model", "Fake SDR"},         {"driver_version", "1.2.3"},
                             {"firmware_version", "4.5.6"}, {"tuner_path", "RX-A"}};
    std::optional<std::string> antenna_ = "Antenna A";
    std::optional<std::string> antennaAfterFrequency_;
    std::optional<std::string> clockSource_ = "internal";
    std::optional<double> frequencyCorrection_ = 1.25;
    std::optional<double> sampleRate_ = 2'000'000.0;
    bool bandwidthSupported_ = true;
    std::optional<double> bandwidth_ = 1'800'000.0;
    std::optional<double> frequency_ = 10'000'000.0;
    bool gainSupported_ = true;
    bool gainModeSupported_ = true;
    std::optional<bool> automaticGain_ = false;
    bool preserveAutomaticGainReadback_ = false;
    std::optional<double> gain_ = 20.0;
    std::size_t mtu_ = 4096;
    std::string failOperation_;
    std::string unsupportedOperation_;
    std::vector<std::string> operations_;
    std::vector<ReadResult> reads_;
    std::optional<double> requestedSampleRate_;
    std::optional<double> requestedBandwidth_;
    std::optional<double> requestedFrequency_;
    std::optional<double> requestedGain_;
    std::optional<std::size_t> setupChannel_;
    std::size_t lastMaximumSamples_ = 0;
    std::chrono::milliseconds lastTimeout_{0};
    bool streamCreated_ = false;
    bool active_ = false;
};

class FakeApi final : public SoapyApi {
  public:
    [[nodiscard]] std::vector<KeywordMap> enumerate(const KeywordMap& filter) override {
        ++enumerateCalls_;
        enumeratedFilter_ = filter;
        if (failEnumeration_) {
            throw std::runtime_error("enumeration failed");
        }
        return matches_;
    }
    [[nodiscard]] SoapyDevice* make(const KeywordMap& arguments) override {
        ++makeCalls_;
        madeArguments_ = arguments;
        if (failMake_) {
            throw std::runtime_error("make failed");
        }
        return &device_;
    }
    void unmake(SoapyDevice* device) override {
        ++unmakeCalls_;
        CHECK(device == &device_);
        if (failUnmake_) {
            throw std::runtime_error("unmake failed");
        }
    }
    [[nodiscard]] std::string libraryVersion() const override {
        return "lib-1";
    }
    [[nodiscard]] std::string apiVersion() const override {
        return "api-1";
    }
    [[nodiscard]] std::string abiVersion() const override {
        return "abi-1";
    }

    FakeDevice device_;
    std::vector<KeywordMap> matches_{{{"driver", "fake"}, {"serial", "resolved-1"}}};
    KeywordMap enumeratedFilter_;
    KeywordMap madeArguments_;
    bool failEnumeration_ = false;
    bool failMake_ = false;
    bool failUnmake_ = false;
    std::size_t enumerateCalls_ = 0;
    std::size_t makeCalls_ = 0;
    std::size_t unmakeCalls_ = 0;
};

CaptureRequest request() {
    CaptureRequest value;
    value.device_arguments = {{"driver", "fake"}};
    value.rx_channel = 0;
    value.center_frequency_hz = 10'000'000.0;
    value.sample_rate_sps = 2'000'000.0;
    value.bandwidth_hz = 1'800'000.0;
    value.gain_db = 20.0;
    value.sample_count = 16;
    value.output_path = "capture";
    return value;
}

std::vector<std::complex<float>> tone(std::size_t count, double sampleRate, double frequency) {
    std::vector<std::complex<float>> samples;
    samples.reserve(count);
    constexpr double pi = 3.14159265358979323846;
    for (std::size_t index = 0; index < count; ++index) {
        const double phase = 2.0 * pi * frequency * static_cast<double>(index) / sampleRate;
        samples.emplace_back(static_cast<float>(0.5 * std::cos(phase)),
                             static_cast<float>(0.5 * std::sin(phase)));
    }
    return samples;
}

sdrcal::application::DeviceCandidate workflowDevice() {
    sdrcal::application::DeviceCandidate value;
    value.identity = {"fake-driver", "Test Manufacturer", "Fake SDR", "resolved-1",
                      sdrcal::profile::IdentityStrength::hardware_serial};
    value.configuration.clock_source = "internal";
    value.configuration.sample_rate_hz = 2'000'000;
    value.configuration.bandwidth_hz = 1'800'000;
    value.configuration.frequency_correction_ppm = 1.25;
    value.configuration.driver_version = "1.2.3";
    value.configuration.firmware_version = "4.5.6";
    value.configuration.antenna_port = "Antenna A";
    value.configuration.tuner_path = "RX-A";
    value.configuration.binding_extension = {
        {"soapy_argument_driver", "fake"},
        {"soapy_argument_serial", "resolved-1"},
        {"effective_gain_db", 20.0},
        {"automatic_gain", false},
    };
    return value;
}

SoapyWorkflowOptions workflowOptions() {
    SoapyWorkflowOptions options;
    options.capture_request = request();
    options.capture_request.sample_count = 8'192;
    options.expected_device = workflowDevice();
    options.reference_conditions = [](const auto& observation) {
        return ReferenceConditionEvidence{true, "conditions:" + observation.observation_id};
    };
    return options;
}

void testSelectionFailures() {
    {
        FakeApi api;
        auto invalid = request();
        invalid.sample_count.reset();
        SoapyCaptureSession session(api);
        const auto result = session.prepare(invalid);
        CHECK(!result.ready);
        CHECK(result.error.stage == PreparationStage::validation);
        CHECK(api.enumerateCalls_ == 0);
    }
    {
        FakeApi api;
        api.failEnumeration_ = true;
        SoapyCaptureSession session(api);
        const auto result = session.prepare(request());
        CHECK(!result.ready);
        CHECK(result.error.stage == PreparationStage::enumeration);
        CHECK(api.makeCalls_ == 0);
    }
    {
        FakeApi api;
        api.matches_.clear();
        SoapyCaptureSession session(api);
        const auto result = session.prepare(request());
        CHECK(!result.ready);
        CHECK(result.error.stage == PreparationStage::selection);
        CHECK(api.makeCalls_ == 0);
    }
    {
        FakeApi api;
        api.matches_.push_back({{"driver", "fake"}, {"serial", "resolved-2"}});
        SoapyCaptureSession session(api);
        const auto result = session.prepare(request());
        CHECK(!result.ready);
        CHECK(result.error.stage == PreparationStage::selection);
        CHECK(api.makeCalls_ == 0);
    }
    {
        FakeApi api;
        auto selected = request();
        selected.enumeration_index = 2;
        SoapyCaptureSession session(api);
        const auto result = session.prepare(selected);
        CHECK(!result.ready);
        CHECK(result.error.stage == PreparationStage::selection);
    }
}

void testIndexedSelectionAndMetadata() {
    FakeApi api;
    api.matches_.push_back({{"driver", "fake"}, {"serial", "resolved-2"}});
    auto selected = request();
    selected.enumeration_index = 1;
    SoapyCaptureSession session(api);
    const auto result = session.prepare(selected);
    CHECK(result.ready);
    CHECK(api.enumeratedFilter_ == selected.device_arguments);
    CHECK(api.madeArguments_ == api.matches_[1]);
    CHECK(result.requested_arguments == selected.device_arguments);
    CHECK(result.resolved_arguments == api.matches_[1]);
    CHECK(result.device.serial == "resolved-2");
    CHECK(result.device.driver_key == "fake-driver");
    CHECK(result.device.hardware_key == "fake-hardware");
    CHECK(result.device.library_version == "lib-1");
    CHECK(result.device.api_version == "api-1");
    CHECK(result.device.abi_version == "abi-1");
    CHECK(result.device.driver_version == "1.2.3");
    CHECK(result.device.firmware_version == "4.5.6");
    CHECK(result.device.tuner_path == "RX-A");
    CHECK(result.stream_mtu == 4096);
    CHECK(result.plan.has_value());
    CHECK(result.plan->stream_mtu == 4096);
    CHECK(session.cleanup().succeeded);
}

void testNoSerialInferenceFromLabel() {
    FakeApi api;
    api.matches_ = {{{"driver", "fake"}, {"label", "Fake serial-looking label"}}};
    api.device_.hardwareInfo_.erase("serial");
    SoapyCaptureSession session(api);
    const auto result = session.prepare(request());
    CHECK(result.ready);
    CHECK(!result.device.serial.has_value());
    session.cleanup();
}

void testConfigurationOrderAndRead() {
    FakeApi api;
    api.device_.antennaAfterFrequency_ = "Configured Antenna";
    api.device_.reads_.push_back({ReadStatus::samples, {{1.0F, -1.0F}, {2.0F, -2.0F}}, 1234, {}});
    SoapyCaptureSession session(api);
    const auto prepared = session.prepare(request());
    CHECK(prepared.ready);
    const std::vector<std::string> expected{
        "set_sample_rate", "get_sample_rate", "supports_bandwidth", "set_bandwidth",
        "get_bandwidth",   "set_frequency",   "get_frequency",      "supports_gain",
        "has_gain_mode",   "set_gain_mode",   "get_gain_mode",      "set_gain",
        "get_gain",        "setup_stream",    "activate_stream"};
    CHECK(api.device_.operations_ == expected);
    CHECK(prepared.effective.sample_rate_sps.state == SettingState::applied_verified);
    CHECK(prepared.effective.bandwidth_hz.state == SettingState::applied_verified);
    CHECK(prepared.effective.center_frequency_hz.state == SettingState::applied_verified);
    CHECK(prepared.effective.gain_db.state == SettingState::applied_verified);
    CHECK(prepared.effective.automatic_gain == false);
    CHECK(prepared.device.antenna == "Configured Antenna");

    const auto read = session.read(2, 25ms);
    CHECK(read.status == ReadStatus::samples);
    CHECK(read.samples.size() == 2);
    CHECK(read.timestamp_ns == 1234);
    CHECK(api.device_.lastMaximumSamples_ == 2);
    CHECK(api.device_.lastTimeout_ == 25ms);
    const auto cleanup = session.cleanup();
    CHECK(cleanup.succeeded);
    CHECK(session.cleanupReport().deactivation.succeeded);
    CHECK(session.cleanupReport().stream_close.succeeded);
    CHECK(session.cleanupReport().device_release.succeeded);
    CHECK(session.cleanupReport().final_state == FinalState::known_safe);
    CHECK(api.unmakeCalls_ == 1);
    CHECK(session.cleanup().succeeded);
    CHECK(api.unmakeCalls_ == 1);
}

void testUnrequestedEffectiveConfigurationIsRecordedWithoutApplication() {
    FakeApi api;
    auto minimal = request();
    minimal.bandwidth_hz.reset();
    minimal.gain_db.reset();
    api.device_.automaticGain_ = true;
    SoapyCaptureSession session(api);
    const auto result = session.prepare(minimal);
    CHECK(result.ready);
    CHECK(!result.effective.bandwidth_hz.requested.has_value());
    CHECK(result.effective.bandwidth_hz.effective == 1'800'000.0);
    CHECK(result.effective.bandwidth_hz.state == SettingState::applied_verified);
    CHECK(!result.effective.gain_db.requested.has_value());
    CHECK(result.effective.gain_db.effective == 20.0);
    CHECK(result.effective.gain_db.state == SettingState::applied_verified);
    CHECK(result.effective.automatic_gain == true);
    CHECK(api.device_.operations_.end() == std::find(api.device_.operations_.begin(),
                                                     api.device_.operations_.end(),
                                                     "set_bandwidth"));
    CHECK(api.device_.operations_.end() == std::find(api.device_.operations_.begin(),
                                                     api.device_.operations_.end(),
                                                     "set_gain_mode"));
    CHECK(api.device_.operations_.end() ==
          std::find(api.device_.operations_.begin(), api.device_.operations_.end(), "set_gain"));
    session.cleanup();
}

void testStrictAndPermissiveReadback() {
    {
        FakeApi api;
        api.device_.sampleRate_ = 1'900'000.0;
        SoapyCaptureSession session(api);
        const auto result = session.prepare(request());
        CHECK(!result.ready);
        CHECK(result.error.stage == PreparationStage::effective_policy);
        CHECK(result.effective.sample_rate_sps.state == SettingState::applied_changed);
        CHECK(api.device_.operations_.end() == std::find(api.device_.operations_.begin(),
                                                         api.device_.operations_.end(),
                                                         "setup_stream"));
        CHECK(api.unmakeCalls_ == 1);
    }
    {
        FakeApi api;
        api.device_.sampleRate_ = 1'900'000.0;
        auto permissive = request();
        permissive.setting_policy = SettingPolicy::permissive;
        SoapyCaptureSession session(api);
        const auto result = session.prepare(permissive);
        CHECK(result.ready);
        CHECK(result.effective.sample_rate_sps.state == SettingState::applied_changed);
        session.cleanup();
    }
    {
        FakeApi api;
        api.device_.bandwidthSupported_ = false;
        auto permissive = request();
        permissive.setting_policy = SettingPolicy::permissive;
        SoapyCaptureSession session(api);
        const auto result = session.prepare(permissive);
        CHECK(!result.ready);
        CHECK(result.effective.bandwidth_hz.state == SettingState::unsupported);
    }
}

void testIndependentCleanupFailures() {
    {
        FakeApi api;
        SoapyCaptureSession session(api);
        CHECK(session.prepare(request()).ready);
        api.device_.failOperation_ = "close_stream";
        const auto cleanup = session.cleanup();
        CHECK(!cleanup.succeeded);
        CHECK(session.cleanupReport().deactivation.succeeded);
        CHECK(!session.cleanupReport().stream_close.succeeded);
        CHECK(session.cleanupReport().device_release.succeeded);
        CHECK(session.cleanupReport().final_state == FinalState::known_safe);
    }
    {
        FakeApi api;
        SoapyCaptureSession session(api);
        CHECK(session.prepare(request()).ready);
        api.failUnmake_ = true;
        const auto cleanup = session.cleanup();
        CHECK(!cleanup.succeeded);
        CHECK(session.cleanupReport().deactivation.succeeded);
        CHECK(session.cleanupReport().stream_close.succeeded);
        CHECK(!session.cleanupReport().device_release.succeeded);
        CHECK(session.cleanupReport().final_state == FinalState::known_open);
    }
}

void testEffectiveRatePlanningPrecedesStreamSetup() {
    FakeApi api;
    auto duration = request();
    duration.sample_count.reset();
    duration.duration_seconds = 120.0;
    api.device_.sampleRate_ = 100'000'000.0;
    duration.setting_policy = SettingPolicy::permissive;
    SoapyCaptureSession session(api);
    const auto result = session.prepare(duration);
    CHECK(!result.ready);
    CHECK(result.error.stage == PreparationStage::planning);
    CHECK(api.device_.operations_.end() == std::find(api.device_.operations_.begin(),
                                                     api.device_.operations_.end(),
                                                     "setup_stream"));
    CHECK(api.unmakeCalls_ == 1);
}

void testConstructionConfigurationAndLifecycleFailures() {
    {
        FakeApi api;
        api.failMake_ = true;
        SoapyCaptureSession session(api);
        const auto result = session.prepare(request());
        CHECK(!result.ready);
        CHECK(result.error.stage == PreparationStage::construction);
        CHECK(api.unmakeCalls_ == 0);
    }
    {
        FakeApi api;
        api.device_.failOperation_ = "set_sample_rate";
        SoapyCaptureSession session(api);
        const auto result = session.prepare(request());
        CHECK(!result.ready);
        CHECK(result.error.stage == PreparationStage::configuration);
        CHECK(result.effective.sample_rate_sps.state == SettingState::failed);
        CHECK(api.unmakeCalls_ == 1);
    }
    {
        FakeApi api;
        api.device_.mtu_ = 0;
        SoapyCaptureSession session(api);
        const auto result = session.prepare(request());
        CHECK(!result.ready);
        CHECK(result.error.stage == PreparationStage::stream_setup);
        CHECK(api.unmakeCalls_ == 1);
    }
    {
        FakeApi api;
        api.device_.failOperation_ = "setup_stream";
        SoapyCaptureSession session(api);
        const auto result = session.prepare(request());
        CHECK(!result.ready);
        CHECK(result.error.stage == PreparationStage::stream_setup);
        CHECK(api.unmakeCalls_ == 1);
    }
    {
        FakeApi api;
        api.device_.failOperation_ = "activate_stream";
        SoapyCaptureSession session(api);
        const auto result = session.prepare(request());
        CHECK(!result.ready);
        CHECK(result.error.stage == PreparationStage::activation);
        CHECK(session.cleanupReport().stream_close.succeeded);
        CHECK(api.unmakeCalls_ == 1);
    }
}

void testCleanupContinuesAfterFailures() {
    FakeApi api;
    SoapyCaptureSession session(api);
    CHECK(session.prepare(request()).ready);
    api.device_.failOperation_ = "deactivate_stream";
    api.failUnmake_ = true;
    const auto cleanup = session.cleanup();
    CHECK(!cleanup.succeeded);
    CHECK(session.cleanupReport().deactivation.attempted);
    CHECK(!session.cleanupReport().deactivation.succeeded);
    CHECK(session.cleanupReport().stream_close.attempted);
    CHECK(session.cleanupReport().stream_close.succeeded);
    CHECK(session.cleanupReport().device_release.attempted);
    CHECK(!session.cleanupReport().device_release.succeeded);
    CHECK(session.cleanupReport().final_state == FinalState::unknown);
}

void testFailedStepCanStillReachKnownSafeState() {
    FakeApi api;
    SoapyCaptureSession session(api);
    CHECK(session.prepare(request()).ready);
    api.device_.failOperation_ = "deactivate_stream";
    const auto cleanup = session.cleanup();
    CHECK(!cleanup.succeeded);
    CHECK(cleanup.final_state == FinalState::known_safe);
    CHECK(session.cleanupReport().stream_close.succeeded);
    CHECK(session.cleanupReport().device_release.succeeded);
}

void testGainModeMustBeVerified() {
    {
        FakeApi api;
        api.device_.automaticGain_.reset();
        api.device_.preserveAutomaticGainReadback_ = true;
        SoapyCaptureSession session(api);
        const auto result = session.prepare(request());
        CHECK(!result.ready);
        CHECK(result.error.stage == PreparationStage::readback);
        CHECK(result.effective.gain_db.state == SettingState::applied_unverified);
    }
    {
        FakeApi api;
        api.device_.automaticGain_ = true;
        api.device_.preserveAutomaticGainReadback_ = true;
        auto permissive = request();
        permissive.setting_policy = SettingPolicy::permissive;
        SoapyCaptureSession session(api);
        const auto result = session.prepare(permissive);
        CHECK(!result.ready);
        CHECK(result.effective.gain_db.state == SettingState::failed);
    }
}

void testReadGuardAndExceptionTranslation() {
    {
        FakeApi api;
        SoapyCaptureSession session(api);
        CHECK(session.prepare(request()).ready);
        api.device_.reads_.push_back(
            {ReadStatus::samples, {{1.0F, 1.0F}, {2.0F, 2.0F}, {3.0F, 3.0F}}, std::nullopt, {}});
        const auto result = session.read(2, 10ms);
        CHECK(result.status == ReadStatus::error);
        CHECK(result.samples.empty());
        session.cleanup();
    }
    {
        FakeApi api;
        SoapyCaptureSession session(api);
        CHECK(session.prepare(request()).ready);
        api.device_.failOperation_ = "read_stream";
        const auto result = session.read(2, 10ms);
        CHECK(result.status == ReadStatus::error);
        CHECK(result.detail == "read_stream failed");
        api.device_.failOperation_.clear();
        session.cleanup();
    }
}

void testNativeReadTranslation() {
    std::vector<std::complex<float>> buffer{{1.0F, -1.0F}, {2.0F, -2.0F}};
    auto result = translateSoapyRead(2, kSoapyHasTime, 42, buffer, 2);
    CHECK(result.status == ReadStatus::samples);
    CHECK(result.samples.size() == 2);
    CHECK(result.timestamp_ns == 42);

    result = translateSoapyRead(1, kSoapyEndAbrupt, 0, buffer, 2);
    CHECK(result.status == ReadStatus::discontinuity);
    CHECK(result.samples.size() == 1);
    CHECK(!result.timestamp_ns.has_value());

    CHECK(translateSoapyRead(kSoapyTimeout, 0, 0, {}, 2).status == ReadStatus::timeout);
    CHECK(translateSoapyRead(kSoapyOverflow, 0, 0, {}, 2).status == ReadStatus::overflow);
    CHECK(translateSoapyRead(kSoapyCorruption, 0, 0, {}, 2).status == ReadStatus::discontinuity);
    CHECK(translateSoapyRead(kSoapyStreamError, 0, 0, {}, 2).status == ReadStatus::error);
    CHECK(translateSoapyRead(-999, 0, 0, {}, 2).status == ReadStatus::error);
    CHECK(translateSoapyRead(0, 0, 0, {}, 2).status == ReadStatus::error);
    CHECK(translateSoapyRead(3, 0, 0, buffer, 2).status == ReadStatus::error);
}

void testWorkflowBoundaryComposesEvidenceAndCleanup() {
    FakeApi api;
    api.device_.reads_.push_back(
        {ReadStatus::samples, tone(8'192, 2'000'000.0, 100'000.0), 1'000, {}});
    auto options = workflowOptions();
    SoapyWorkflowBoundary boundary(api, options);
    const auto discovered = boundary.discover();
    CHECK(discovered.size() == 1);
    CHECK(discovered.front().identity.identifier == "resolved-1");

    const sdrcal::application::ObservationRequest observation{"observation-1", "independence-1",
                                                              "reference-1", 10'000'000.0};
    const auto result = boundary.acquire(discovered.front(), options.expected_device.configuration,
                                         observation, {});
    CHECK(result.status == sdrcal::application::AcquisitionStatus::success);
    CHECK(result.samples.size() == 8'192);
    CHECK(result.carrier_estimate.ok());
    CHECK(result.carrier_estimate.sample_count == result.samples.size());
    CHECK(result.signal_quality_version == "signal-quality-v1");
    CHECK(result.stream_evidence.target_samples == 8'192);
    CHECK(result.stream_evidence.written_samples == 8'192);
    CHECK(result.missing_samples == 0);
    CHECK(result.discontinuities == 0);
    CHECK(result.reference_conditions_met);
    CHECK(result.reference_conditions_evidence == "conditions:observation-1");
    CHECK(result.deactivation.succeeded);
    CHECK(result.stream_close.succeeded);
    CHECK(result.device_release.succeeded);
    CHECK(result.final_device_state_safe);
    CHECK(api.unmakeCalls_ == 1);
}

void testWorkflowBoundaryFailsBeforeConstructionAndRejectsIndexIdentity() {
    {
        FakeApi api;
        auto options = workflowOptions();
        options.memory_limits.maximum_bytes = 8;
        SoapyWorkflowBoundary boundary(api, options);
        const auto result =
            boundary.acquire(options.expected_device, options.expected_device.configuration,
                             {"observation-1", "independence-1", "reference-1", 10'000'000.0}, {});
        CHECK(result.status == sdrcal::application::AcquisitionStatus::failed);
        CHECK(result.reason.find("memory preflight") != std::string::npos);
        CHECK(api.makeCalls_ == 0);
    }
    {
        FakeApi api;
        auto options = workflowOptions();
        options.capture_request.setting_policy = SettingPolicy::permissive;
        SoapyWorkflowBoundary boundary(api, options);
        const auto result =
            boundary.acquire(options.expected_device, options.expected_device.configuration,
                             {"observation-1", "independence-1", "reference-1", 10'000'000.0}, {});
        CHECK(result.failure_stage == sdrcal::application::AcquisitionFailureStage::preflight);
        CHECK(api.makeCalls_ == 0);
    }
    {
        FakeApi api;
        auto options = workflowOptions();
        options.capture_request.enumeration_index = 0;
        SoapyWorkflowBoundary boundary(api, options);
        CHECK(boundary.discover().empty());
        CHECK(api.enumerateCalls_ == 0);
    }
}

void testWorkflowBoundaryMaintainsMultiObservationIdentity() {
    FakeApi api;
    api.device_.reads_.push_back(
        {ReadStatus::samples, tone(8'192, 2'000'000.0, 100'000.0), 1'000, {}});
    api.device_.reads_.push_back(
        {ReadStatus::samples, tone(8'192, 2'000'000.0, 110'000.0), 2'000, {}});
    auto options = workflowOptions();
    SoapyWorkflowBoundary boundary(api, options);
    const auto first =
        boundary.acquire(options.expected_device, options.expected_device.configuration,
                         {"observation-1", "independence-1", "reference-1", 10'000'000.0}, {});
    const auto second =
        boundary.acquire(options.expected_device, options.expected_device.configuration,
                         {"observation-2", "independence-2", "reference-2", 10'000'000.0}, {});
    CHECK(first.status == sdrcal::application::AcquisitionStatus::success);
    CHECK(second.status == sdrcal::application::AcquisitionStatus::success);
    CHECK(first.identity.identifier == second.identity.identifier);
    CHECK(first.configuration.sample_rate_hz == second.configuration.sample_rate_hz);
    CHECK(api.unmakeCalls_ == 2);
}

void testWorkflowBoundaryFailsClosedOnIdentityAndCleanup() {
    {
        FakeApi api;
        api.device_.hardwareInfo_["manufacturer"] = "Changed Manufacturer";
        auto options = workflowOptions();
        SoapyWorkflowBoundary boundary(api, options);
        const auto result =
            boundary.acquire(options.expected_device, options.expected_device.configuration,
                             {"observation-1", "independence-1", "reference-1", 10'000'000.0}, {});
        CHECK(result.status == sdrcal::application::AcquisitionStatus::failed);
        CHECK(result.reason.find("identity or configuration") != std::string::npos);
        CHECK(result.device_release.succeeded);
        CHECK(result.final_device_state_safe);
    }
    {
        FakeApi api;
        api.device_.reads_.push_back(
            {ReadStatus::samples, tone(8'192, 2'000'000.0, 100'000.0), 1'000, {}});
        api.failUnmake_ = true;
        auto options = workflowOptions();
        SoapyWorkflowBoundary boundary(api, options);
        const auto result =
            boundary.acquire(options.expected_device, options.expected_device.configuration,
                             {"observation-1", "independence-1", "reference-1", 10'000'000.0}, {});
        CHECK(result.status == sdrcal::application::AcquisitionStatus::failed);
        CHECK(!result.device_release.succeeded);
        CHECK(!result.final_device_state_safe);
        CHECK(result.samples.empty());
    }
}

void testWorkflowBoundaryCancellationAndStreamFailure() {
    {
        FakeApi api;
        auto options = workflowOptions();
        SoapyWorkflowBoundary boundary(api, options);
        const auto result = boundary.acquire(
            options.expected_device, options.expected_device.configuration,
            {"observation-1", "independence-1", "reference-1", 10'000'000.0}, [] { return true; });
        CHECK(result.status == sdrcal::application::AcquisitionStatus::cancelled);
        CHECK(api.makeCalls_ == 0);
    }
    {
        FakeApi api;
        api.device_.reads_.push_back(
            {ReadStatus::discontinuity, {}, 1'000, "timestamp discontinuity"});
        auto options = workflowOptions();
        SoapyWorkflowBoundary boundary(api, options);
        const auto result =
            boundary.acquire(options.expected_device, options.expected_device.configuration,
                             {"observation-1", "independence-1", "reference-1", 10'000'000.0}, {});
        CHECK(result.status == sdrcal::application::AcquisitionStatus::failed);
        CHECK(result.stream_evidence.discontinuities == 1);
        CHECK(result.samples.empty());
        CHECK(result.final_device_state_safe);
    }
}

} // namespace

int main() {
    const std::vector<std::pair<std::string, std::function<void()>>> tests{
        {"selection failures", testSelectionFailures},
        {"indexed selection and metadata", testIndexedSelectionAndMetadata},
        {"no serial inference", testNoSerialInferenceFromLabel},
        {"configuration order and read", testConfigurationOrderAndRead},
        {"unrequested effective configuration",
         testUnrequestedEffectiveConfigurationIsRecordedWithoutApplication},
        {"strict and permissive readback", testStrictAndPermissiveReadback},
        {"effective rate planning precedes stream setup",
         testEffectiveRatePlanningPrecedesStreamSetup},
        {"construction configuration lifecycle failures",
         testConstructionConfigurationAndLifecycleFailures},
        {"cleanup continues after failures", testCleanupContinuesAfterFailures},
        {"failed step can reach known safe", testFailedStepCanStillReachKnownSafeState},
        {"independent cleanup failures", testIndependentCleanupFailures},
        {"gain mode must be verified", testGainModeMustBeVerified},
        {"read guard and exception translation", testReadGuardAndExceptionTranslation},
        {"native read translation", testNativeReadTranslation},
        {"workflow evidence and cleanup", testWorkflowBoundaryComposesEvidenceAndCleanup},
        {"workflow preflight and index identity",
         testWorkflowBoundaryFailsBeforeConstructionAndRejectsIndexIdentity},
        {"workflow identity and cleanup failures",
         testWorkflowBoundaryFailsClosedOnIdentityAndCleanup},
        {"workflow cancellation and stream failure",
         testWorkflowBoundaryCancellationAndStreamFailure},
        {"workflow multi-observation identity",
         testWorkflowBoundaryMaintainsMultiObservationIdentity},
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
