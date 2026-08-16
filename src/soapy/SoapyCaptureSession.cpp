#include "soapy/SoapyCaptureSession.h"

#include <exception>
#include <sstream>
#include <utility>

namespace sdrcal::soapy {
namespace {

std::optional<std::string> mapValue(const KeywordMap& values, const std::string& key) {
    const auto iterator = values.find(key);
    if (iterator == values.end() || iterator->second.empty()) {
        return std::nullopt;
    }
    return iterator->second;
}

std::optional<std::string> firstMapValue(const KeywordMap& values,
                                         std::initializer_list<const char*> keys) {
    for (const char* key : keys) {
        if (auto value = mapValue(values, key); value.has_value()) {
            return value;
        }
    }
    return std::nullopt;
}

template <typename Callable> std::optional<std::string> optionalText(Callable&& callable) {
    try {
        std::string value = callable();
        return value.empty() ? std::nullopt : std::optional<std::string>(std::move(value));
    } catch (...) {
        return std::nullopt;
    }
}

template <typename Callable> auto optionalValue(Callable&& callable) -> decltype(callable()) {
    try {
        return callable();
    } catch (...) {
        return {};
    }
}

capture::SettingState failedState(const std::exception& error) {
    const auto* operation = dynamic_cast<const OperationError*>(&error);
    if (operation != nullptr && operation->failure() == OperationFailure::unsupported) {
        return capture::SettingState::unsupported;
    }
    return capture::SettingState::failed;
}

} // namespace

SoapyCaptureSession::~SoapyCaptureSession() {
    cleanupAll();
}

std::optional<double>
SoapyCaptureSession::applyAndRead(const std::function<void()>& apply,
                                  const std::function<std::optional<double>()>& readback,
                                  bool& applicationSucceeded, bool& supported, std::string& error) {
    try {
        apply();
        applicationSucceeded = true;
    } catch (const OperationError& exception) {
        supported = exception.failure() != OperationFailure::unsupported;
        error = exception.what();
        return std::nullopt;
    } catch (const std::exception& exception) {
        error = exception.what();
        return std::nullopt;
    } catch (...) {
        error = "unknown exception applying setting";
        return std::nullopt;
    }

    try {
        return readback();
    } catch (const std::exception& exception) {
        error = exception.what();
        return std::nullopt;
    } catch (...) {
        error = "unknown exception reading effective setting";
        return std::nullopt;
    }
}

PreparationResult SoapyCaptureSession::prepare(const capture::CaptureRequest& request,
                                               const capture::SettingTolerances& tolerances,
                                               const capture::ResourceLimits& limits) {
    PreparationResult result;
    result.requested_arguments = request.device_arguments;
    if (device_ != nullptr || streamCreated_ || active_ || cleaned_) {
        result.error = {PreparationStage::construction, "capture session is single-use"};
        return result;
    }
    const auto validationErrors = capture::validateCaptureRequestBeforeDevice(request, limits);
    if (!validationErrors.empty()) {
        result.error = {PreparationStage::validation, validationErrors.front().message};
        return result;
    }

    std::vector<KeywordMap> matches;
    try {
        matches = api_.enumerate(request.device_arguments);
    } catch (const std::exception& exception) {
        result.error = {PreparationStage::enumeration, exception.what()};
        return result;
    } catch (...) {
        result.error = {PreparationStage::enumeration, "unknown enumeration failure"};
        return result;
    }

    if (matches.empty()) {
        result.error = {PreparationStage::selection, "device selection matched no devices"};
        return result;
    }
    std::size_t selectedIndex = 0;
    if (request.enumeration_index.has_value()) {
        if (*request.enumeration_index >= matches.size()) {
            result.error = {PreparationStage::selection,
                            "device enumeration index is out of range"};
            return result;
        }
        selectedIndex = *request.enumeration_index;
    } else if (matches.size() != 1) {
        result.error = {PreparationStage::selection, "device selection is ambiguous"};
        return result;
    }
    result.resolved_arguments = matches[selectedIndex];

    try {
        device_ = api_.make(result.resolved_arguments);
        if (device_ == nullptr) {
            throw std::runtime_error("Soapy API returned a null device");
        }
    } catch (const std::exception& exception) {
        result.error = {PreparationStage::construction, exception.what()};
        return result;
    } catch (...) {
        result.error = {PreparationStage::construction, "unknown device construction failure"};
        return result;
    }

    result.device.resolved_arguments = result.resolved_arguments;
    result.device.driver_key = optionalText([&]() { return device_->driverKey(); });
    result.device.hardware_key = optionalText([&]() { return device_->hardwareKey(); });
    try {
        result.device.hardware_info = device_->hardwareInfo();
    } catch (...) {
    }
    result.device.serial = firstMapValue(result.resolved_arguments, {"serial"});
    if (!result.device.serial.has_value()) {
        result.device.serial = firstMapValue(result.device.hardware_info, {"serial"});
    }
    result.device.library_version = optionalText([&]() { return api_.libraryVersion(); });
    result.device.api_version = optionalText([&]() { return api_.apiVersion(); });
    result.device.abi_version = optionalText([&]() { return api_.abiVersion(); });
    result.device.driver_version =
        firstMapValue(result.device.hardware_info, {"driver_version", "driverVersion"});
    result.device.firmware_version =
        firstMapValue(result.device.hardware_info, {"firmware_version", "firmwareVersion"});
    result.device.tuner_path =
        firstMapValue(result.device.hardware_info, {"tuner_path", "tunerPath"});
    auto configure = [&](const std::string& name, std::optional<double> requested, bool supported,
                         const std::function<void()>& apply,
                         const std::function<std::optional<double>()>& readback,
                         const capture::NumericTolerance& tolerance,
                         capture::EffectiveSetting& destination) -> bool {
        if (!requested.has_value()) {
            std::optional<double> effective;
            if (supported) {
                try {
                    effective = readback();
                } catch (...) {
                }
            }
            destination =
                capture::classifyEffectiveSetting(requested, effective, supported, true, tolerance);
            return true;
        }
        if (!supported) {
            destination = {requested, std::nullopt, capture::SettingState::unsupported};
            return true;
        }
        bool applicationSucceeded = false;
        bool remainedSupported = true;
        std::string error;
        const auto effective =
            applyAndRead(apply, readback, applicationSucceeded, remainedSupported, error);
        destination = capture::classifyEffectiveSetting(requested, effective, remainedSupported,
                                                        applicationSucceeded, tolerance);
        if (destination.state == capture::SettingState::failed) {
            result.error = {PreparationStage::configuration, name + ": " + error};
            return false;
        }
        return true;
    };

    if (!configure(
            "sample_rate_sps", request.sample_rate_sps, true,
            [&]() { device_->setSampleRate(request.rx_channel, request.sample_rate_sps); },
            [&]() { return device_->sampleRate(request.rx_channel); }, tolerances.sample_rate_sps,
            result.effective.sample_rate_sps)) {
        cleanupAll();
        return result;
    }

    bool bandwidthSupported = false;
    try {
        bandwidthSupported = device_->supportsBandwidth(request.rx_channel);
    } catch (const std::exception& exception) {
        if (request.bandwidth_hz.has_value()) {
            result.error = {PreparationStage::configuration,
                            "bandwidth_hz capability: " + std::string(exception.what())};
            cleanupAll();
            return result;
        }
    }
    if (!configure(
            "bandwidth_hz", request.bandwidth_hz, bandwidthSupported,
            [&]() { device_->setBandwidth(request.rx_channel, *request.bandwidth_hz); },
            [&]() { return device_->bandwidth(request.rx_channel); }, tolerances.bandwidth_hz,
            result.effective.bandwidth_hz)) {
        cleanupAll();
        return result;
    }

    if (!configure(
            "center_frequency_hz", request.center_frequency_hz, true,
            [&]() { device_->setFrequency(request.rx_channel, request.center_frequency_hz); },
            [&]() { return device_->frequency(request.rx_channel); },
            tolerances.center_frequency_hz, result.effective.center_frequency_hz)) {
        cleanupAll();
        return result;
    }

    bool gainSupported = false;
    bool gainModeSupported = false;
    try {
        gainSupported = device_->supportsGain(request.rx_channel);
        gainModeSupported = gainSupported && device_->hasGainMode(request.rx_channel);
        if (gainModeSupported) {
            if (request.gain_db.has_value()) {
                device_->setAutomaticGain(request.rx_channel, false);
            }
            result.effective.automatic_gain = device_->automaticGain(request.rx_channel);
        }
    } catch (const std::exception& exception) {
        if (request.gain_db.has_value()) {
            result.error = {PreparationStage::configuration,
                            "gain mode: " + std::string(exception.what())};
            result.effective.gain_db = {request.gain_db, std::nullopt, failedState(exception)};
            cleanupAll();
            return result;
        }
    }
    if (!configure(
            "gain_db", request.gain_db, gainSupported,
            [&]() { device_->setGain(request.rx_channel, *request.gain_db); },
            [&]() { return device_->gain(request.rx_channel); }, tolerances.gain_db,
            result.effective.gain_db)) {
        cleanupAll();
        return result;
    }
    if (request.gain_db.has_value() && gainModeSupported) {
        if (!result.effective.automatic_gain.has_value()) {
            result.effective.gain_db.state = capture::SettingState::applied_unverified;
        } else if (*result.effective.automatic_gain) {
            result.effective.gain_db.state = capture::SettingState::failed;
        }
    }

    result.device.antenna = optionalValue([&]() { return device_->antenna(request.rx_channel); });
    try {
        result.device.clock_source = device_->clockSource();
        if (result.device.clock_source && !result.device.clock_source->empty()) {
            result.device.clock_source_reported = true;
        } else {
            result.device.clock_source.reset();
            if (device_->clockSources().empty()) {
                result.device.clock_source = "soapy-driver-default";
                result.device.clock_source_reported = false;
            }
        }
    } catch (...) {
        result.device.clock_source.reset();
        result.device.clock_source_reported.reset();
    }
    try {
        result.device.frequency_correction_supported =
            device_->hasFrequencyCorrection(request.rx_channel);
        if (*result.device.frequency_correction_supported) {
            result.effective.frequency_correction_ppm =
                device_->frequencyCorrection(request.rx_channel);
        } else {
            result.effective.frequency_correction_ppm = 0.0;
        }
    } catch (...) {
        result.device.frequency_correction_supported.reset();
    }

    if (request.setting_policy == capture::SettingPolicy::strict) {
        const auto unverified = [](const capture::EffectiveSetting& setting) {
            return setting.requested.has_value() &&
                   setting.state == capture::SettingState::applied_unverified;
        };
        if (unverified(result.effective.sample_rate_sps) ||
            unverified(result.effective.bandwidth_hz) ||
            unverified(result.effective.center_frequency_hz) ||
            unverified(result.effective.gain_db)) {
            result.error = {PreparationStage::readback,
                            "an explicitly requested setting could not be verified"};
            cleanupAll();
            return result;
        }
    }

    const auto policyErrors =
        capture::evaluateEffectiveSettings(result.effective, request.setting_policy);
    if (!policyErrors.empty()) {
        std::ostringstream message;
        for (std::size_t index = 0; index < policyErrors.size(); ++index) {
            if (index != 0) {
                message << "; ";
            }
            message << policyErrors[index].message;
        }
        result.error = {PreparationStage::effective_policy, message.str()};
        cleanupAll();
        return result;
    }

    auto planned = capture::makeCapturePlan(request, result.effective, limits);
    if (!planned.ok()) {
        result.error = {PreparationStage::planning, planned.errors.empty()
                                                        ? "capture planning failed"
                                                        : planned.errors.front().message};
        cleanupAll();
        return result;
    }
    result.plan = std::move(planned.plan);

    try {
        result.stream_mtu = device_->setupRxStream(request.rx_channel);
        streamCreated_ = true;
        result.plan->stream_format = result.stream_format;
        result.plan->stream_mtu = result.stream_mtu;
    } catch (const std::exception& exception) {
        result.error = {PreparationStage::stream_setup, exception.what()};
        cleanupAll();
        return result;
    }
    try {
        device_->activateRxStream();
        active_ = true;
    } catch (const std::exception& exception) {
        result.error = {PreparationStage::activation, exception.what()};
        cleanupAll();
        return result;
    }

    result.ready = true;
    return result;
}

capture::ReadResult SoapyCaptureSession::read(std::size_t maximumSamples,
                                              std::chrono::milliseconds timeout) {
    if (!active_ || device_ == nullptr) {
        return {capture::ReadStatus::error, {}, std::nullopt, "Soapy RX session is not active"};
    }
    if (maximumSamples == 0) {
        return {capture::ReadStatus::error, {}, std::nullopt, "sample request must be positive"};
    }
    try {
        capture::ReadResult result = device_->readRxStream(maximumSamples, timeout);
        if (result.samples.size() > maximumSamples) {
            return {capture::ReadStatus::error,
                    {},
                    result.timestamp_ns,
                    "Soapy device facade returned more samples than requested"};
        }
        return result;
    } catch (const std::exception& exception) {
        return {capture::ReadStatus::error, {}, std::nullopt, exception.what()};
    } catch (...) {
        return {capture::ReadStatus::error, {}, std::nullopt, "unknown Soapy read failure"};
    }
}

void SoapyCaptureSession::cleanupAll() noexcept {
    if (cleaned_) {
        return;
    }
    cleaned_ = true;

    if (active_ && device_ != nullptr) {
        cleanup_.deactivation.attempted = true;
        try {
            device_->deactivateRxStream();
            active_ = false;
            cleanup_.deactivation.succeeded = true;
            cleanup_.deactivation.detail = "RX stream deactivated";
        } catch (const std::exception& exception) {
            cleanup_.deactivation.detail = exception.what();
        } catch (...) {
            cleanup_.deactivation.detail = "unknown stream deactivation failure";
        }
    }

    if (streamCreated_ && device_ != nullptr) {
        cleanup_.stream_close.attempted = true;
        try {
            device_->closeRxStream();
            streamCreated_ = false;
            cleanup_.stream_close.succeeded = true;
            cleanup_.stream_close.detail = "RX stream closed";
        } catch (const std::exception& exception) {
            cleanup_.stream_close.detail = exception.what();
        } catch (...) {
            cleanup_.stream_close.detail = "unknown stream close failure";
        }
    }

    if (device_ != nullptr) {
        cleanup_.device_release.attempted = true;
        try {
            api_.unmake(device_);
            device_ = nullptr;
            cleanup_.device_release.succeeded = true;
            cleanup_.device_release.detail = "Soapy device released";
        } catch (const std::exception& exception) {
            cleanup_.device_release.detail = exception.what();
        } catch (...) {
            cleanup_.device_release.detail = "unknown device release failure";
        }
    }

    if (device_ == nullptr) {
        cleanup_.final_state = capture::FinalState::known_safe;
    } else if (!active_ && !streamCreated_) {
        cleanup_.final_state = capture::FinalState::known_open;
    } else {
        cleanup_.final_state = capture::FinalState::unknown;
    }
}

capture::CleanupResult SoapyCaptureSession::cleanup() noexcept {
    cleanupAll();
    const bool attempted = cleanup_.deactivation.attempted || cleanup_.stream_close.attempted ||
                           cleanup_.device_release.attempted;
    const auto operationSucceeded = [](const capture::CleanupResult& operation) {
        return !operation.attempted || operation.succeeded;
    };
    const bool succeeded = cleanup_.final_state == capture::FinalState::known_safe &&
                           operationSucceeded(cleanup_.deactivation) &&
                           operationSucceeded(cleanup_.stream_close) &&
                           operationSucceeded(cleanup_.device_release);
    std::ostringstream detail;
    if (cleanup_.deactivation.attempted) {
        detail << "deactivation=" << (cleanup_.deactivation.succeeded ? "ok" : "failed") << "; ";
    }
    if (cleanup_.stream_close.attempted) {
        detail << "stream_close=" << (cleanup_.stream_close.succeeded ? "ok" : "failed") << "; ";
    }
    if (cleanup_.device_release.attempted) {
        detail << "device_release=" << (cleanup_.device_release.succeeded ? "ok" : "failed");
    }
    return {attempted, succeeded, detail.str(), cleanup_.final_state};
}

std::string toString(PreparationStage stage) {
    switch (stage) {
    case PreparationStage::none:
        return "none";
    case PreparationStage::validation:
        return "validation";
    case PreparationStage::enumeration:
        return "enumeration";
    case PreparationStage::selection:
        return "selection";
    case PreparationStage::construction:
        return "construction";
    case PreparationStage::configuration:
        return "configuration";
    case PreparationStage::readback:
        return "readback";
    case PreparationStage::effective_policy:
        return "effective_policy";
    case PreparationStage::planning:
        return "planning";
    case PreparationStage::stream_setup:
        return "stream_setup";
    case PreparationStage::activation:
        return "activation";
    }
    return "unknown";
}

} // namespace sdrcal::soapy
