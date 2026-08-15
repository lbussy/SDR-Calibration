#pragma once

#include "capture/CapturePlan.h"
#include "capture/SampleSource.h"
#include "soapy/SoapyApi.h"

#include <optional>
#include <functional>
#include <string>
#include <vector>

namespace sdrcal::soapy {

enum class PreparationStage {
    none,
    validation,
    enumeration,
    selection,
    construction,
    configuration,
    readback,
    effective_policy,
    planning,
    stream_setup,
    activation
};

struct PreparationError {
    PreparationStage stage = PreparationStage::none;
    std::string message;
};

struct PreparationResult {
    bool ready = false;
    KeywordMap requested_arguments;
    KeywordMap resolved_arguments;
    capture::DeviceMetadata device;
    capture::EffectiveSettings effective;
    std::optional<capture::CapturePlan> plan;
    std::string stream_format = "CF32";
    std::optional<std::size_t> stream_mtu;
    PreparationError error;
};

struct SoapyCleanupReport {
    capture::CleanupResult deactivation;
    capture::CleanupResult stream_close;
    capture::CleanupResult device_release;
    capture::FinalState final_state = capture::FinalState::unknown;
};

class SoapyCaptureSession final : public capture::SampleSource {
public:
    // The injected API must outlive the session.
    explicit SoapyCaptureSession(SoapyApi& api) : api_(api) {}
    SoapyCaptureSession(const SoapyCaptureSession&) = delete;
    SoapyCaptureSession& operator=(const SoapyCaptureSession&) = delete;
    ~SoapyCaptureSession() override;

    [[nodiscard]] PreparationResult prepare(
        const capture::CaptureRequest& request,
        const capture::SettingTolerances& tolerances = {},
        const capture::ResourceLimits& limits = {});
    capture::ReadResult read(
        std::size_t maximumSamples,
        std::chrono::milliseconds timeout) override;
    capture::CleanupResult cleanup() noexcept override;
    [[nodiscard]] const SoapyCleanupReport& cleanupReport() const noexcept { return cleanup_; }
    [[nodiscard]] bool ready() const noexcept { return active_; }

private:
    [[nodiscard]] std::optional<double> applyAndRead(
        const std::function<void()>& apply,
        const std::function<std::optional<double>()>& readback,
        bool& applicationSucceeded,
        bool& supported,
        std::string& error);
    void cleanupAll() noexcept;

    SoapyApi& api_;
    SoapyDevice* device_ = nullptr;
    bool streamCreated_ = false;
    bool active_ = false;
    bool cleaned_ = false;
    SoapyCleanupReport cleanup_;
};

[[nodiscard]] std::string toString(PreparationStage stage);

} // namespace sdrcal::soapy
