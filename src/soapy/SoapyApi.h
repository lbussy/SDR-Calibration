#pragma once

#include "capture/CaptureTypes.h"

#include <chrono>
#include <complex>
#include <cstddef>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace sdrcal::soapy {

using KeywordMap = std::map<std::string, std::string>;

enum class OperationFailure { unsupported, failed };

class OperationError : public std::runtime_error {
  public:
    OperationError(OperationFailure failure, const std::string& message)
        : std::runtime_error(message), failure_(failure) {}

    [[nodiscard]] OperationFailure failure() const noexcept {
        return failure_;
    }

  private:
    OperationFailure failure_;
};

class SoapyDevice {
  public:
    virtual ~SoapyDevice() = default;

    [[nodiscard]] virtual std::string driverKey() const = 0;
    [[nodiscard]] virtual std::string hardwareKey() const = 0;
    [[nodiscard]] virtual KeywordMap hardwareInfo() const = 0;
    [[nodiscard]] virtual std::optional<std::string> antenna(std::size_t channel) const = 0;
    [[nodiscard]] virtual std::optional<std::string> clockSource() const = 0;
    [[nodiscard]] virtual std::vector<std::string> clockSources() const = 0;
    [[nodiscard]] virtual bool hasFrequencyCorrection(std::size_t channel) const = 0;
    [[nodiscard]] virtual std::optional<double> frequencyCorrection(std::size_t channel) const = 0;

    virtual void setSampleRate(std::size_t channel, double value) = 0;
    [[nodiscard]] virtual std::optional<double> sampleRate(std::size_t channel) const = 0;
    [[nodiscard]] virtual bool supportsBandwidth(std::size_t channel) const = 0;
    virtual void setBandwidth(std::size_t channel, double value) = 0;
    [[nodiscard]] virtual std::optional<double> bandwidth(std::size_t channel) const = 0;
    virtual void setFrequency(std::size_t channel, double value) = 0;
    [[nodiscard]] virtual std::optional<double> frequency(std::size_t channel) const = 0;
    [[nodiscard]] virtual bool supportsGain(std::size_t channel) const = 0;
    [[nodiscard]] virtual bool hasGainMode(std::size_t channel) const = 0;
    virtual void setAutomaticGain(std::size_t channel, bool automatic) = 0;
    [[nodiscard]] virtual std::optional<bool> automaticGain(std::size_t channel) const = 0;
    virtual void setGain(std::size_t channel, double value) = 0;
    [[nodiscard]] virtual std::optional<double> gain(std::size_t channel) const = 0;

    [[nodiscard]] virtual std::size_t setupRxStream(std::size_t channel) = 0;
    virtual void activateRxStream() = 0;
    [[nodiscard]] virtual capture::ReadResult readRxStream(std::size_t maximumSamples,
                                                           std::chrono::milliseconds timeout) = 0;
    virtual void deactivateRxStream() = 0;
    virtual void closeRxStream() = 0;
};

class SoapyApi {
  public:
    virtual ~SoapyApi() = default;
    [[nodiscard]] virtual std::vector<KeywordMap> enumerate(const KeywordMap& filter) = 0;
    [[nodiscard]] virtual SoapyDevice* make(const KeywordMap& arguments) = 0;
    // Releases the underlying device and destroys the facade on success.
    virtual void unmake(SoapyDevice* device) = 0;
    [[nodiscard]] virtual std::string libraryVersion() const = 0;
    [[nodiscard]] virtual std::string apiVersion() const = 0;
    [[nodiscard]] virtual std::string abiVersion() const = 0;
};

} // namespace sdrcal::soapy
