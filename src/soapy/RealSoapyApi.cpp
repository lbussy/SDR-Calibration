#include "soapy/RealSoapyApi.h"

#include "soapy/SoapyReadTranslator.h"

#include <SoapySDR/Constants.h>
#include <SoapySDR/Device.hpp>
#include <SoapySDR/Errors.h>
#include <SoapySDR/Formats.hpp>
#include <SoapySDR/Version.hpp>

#include <algorithm>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>

namespace sdrcal::soapy {
namespace {

SoapySDR::Kwargs toNative(const KeywordMap& values) {
    return {values.begin(), values.end()};
}

KeywordMap fromNative(const SoapySDR::Kwargs& values) {
    return {values.begin(), values.end()};
}

class RealSoapyDevice final : public SoapyDevice {
public:
    explicit RealSoapyDevice(SoapySDR::Device* device) : device_(device) {
        if (device_ == nullptr) {
            throw std::runtime_error("SoapySDR returned a null device");
        }
    }

    ~RealSoapyDevice() override {
        if (device_ != nullptr) {
            try {
                SoapySDR::Device::unmake(device_);
            } catch (...) {
            }
        }
    }

    [[nodiscard]] std::string driverKey() const override { return device_->getDriverKey(); }
    [[nodiscard]] std::string hardwareKey() const override { return device_->getHardwareKey(); }
    [[nodiscard]] KeywordMap hardwareInfo() const override {
        return fromNative(device_->getHardwareInfo());
    }
    [[nodiscard]] std::optional<std::string> antenna(std::size_t channel) const override {
        return device_->getAntenna(SOAPY_SDR_RX, channel);
    }
    [[nodiscard]] std::optional<std::string> clockSource() const override {
        return device_->getClockSource();
    }
    [[nodiscard]] bool hasFrequencyCorrection(std::size_t channel) const override {
        return device_->hasFrequencyCorrection(SOAPY_SDR_RX, channel);
    }
    [[nodiscard]] std::optional<double> frequencyCorrection(std::size_t channel) const override {
        return device_->getFrequencyCorrection(SOAPY_SDR_RX, channel);
    }

    void setSampleRate(std::size_t channel, double value) override {
        device_->setSampleRate(SOAPY_SDR_RX, channel, value);
    }
    [[nodiscard]] std::optional<double> sampleRate(std::size_t channel) const override {
        return device_->getSampleRate(SOAPY_SDR_RX, channel);
    }
    [[nodiscard]] bool supportsBandwidth(std::size_t channel) const override {
        return !device_->getBandwidthRange(SOAPY_SDR_RX, channel).empty();
    }
    void setBandwidth(std::size_t channel, double value) override {
        device_->setBandwidth(SOAPY_SDR_RX, channel, value);
    }
    [[nodiscard]] std::optional<double> bandwidth(std::size_t channel) const override {
        return device_->getBandwidth(SOAPY_SDR_RX, channel);
    }
    void setFrequency(std::size_t channel, double value) override {
        device_->setFrequency(SOAPY_SDR_RX, channel, value);
    }
    [[nodiscard]] std::optional<double> frequency(std::size_t channel) const override {
        return device_->getFrequency(SOAPY_SDR_RX, channel);
    }
    [[nodiscard]] bool supportsGain(std::size_t channel) const override {
        return !device_->listGains(SOAPY_SDR_RX, channel).empty();
    }
    [[nodiscard]] bool hasGainMode(std::size_t channel) const override {
        return device_->hasGainMode(SOAPY_SDR_RX, channel);
    }
    void setAutomaticGain(std::size_t channel, bool automatic) override {
        device_->setGainMode(SOAPY_SDR_RX, channel, automatic);
    }
    [[nodiscard]] std::optional<bool> automaticGain(std::size_t channel) const override {
        return device_->getGainMode(SOAPY_SDR_RX, channel);
    }
    void setGain(std::size_t channel, double value) override {
        device_->setGain(SOAPY_SDR_RX, channel, value);
    }
    [[nodiscard]] std::optional<double> gain(std::size_t channel) const override {
        return device_->getGain(SOAPY_SDR_RX, channel);
    }

    [[nodiscard]] std::size_t setupRxStream(std::size_t channel) override {
        if (stream_ != nullptr) {
            throw std::runtime_error("RX stream is already configured");
        }
        stream_ = device_->setupStream(SOAPY_SDR_RX, SOAPY_SDR_CF32, {channel});
        if (stream_ == nullptr) {
            throw std::runtime_error("SoapySDR returned a null RX stream");
        }
        try {
            const std::size_t mtu = device_->getStreamMTU(stream_);
            if (mtu == 0) {
                throw std::runtime_error("SoapySDR returned a zero stream MTU");
            }
            return mtu;
        } catch (...) {
            try {
                device_->closeStream(stream_);
            } catch (...) {
            }
            stream_ = nullptr;
            throw;
        }
    }

    void activateRxStream() override {
        if (stream_ == nullptr) {
            throw std::runtime_error("RX stream is not configured");
        }
        const int result = device_->activateStream(stream_);
        if (result != 0) {
            throw std::runtime_error(
                "SoapySDR activateStream failed: " + std::string(SoapySDR_errToStr(result)));
        }
        active_ = true;
    }

    [[nodiscard]] capture::ReadResult readRxStream(
        std::size_t maximumSamples,
        std::chrono::milliseconds timeout) override {
        if (stream_ == nullptr || !active_) {
            return {capture::ReadStatus::error, {}, std::nullopt, "RX stream is not active"};
        }
        std::vector<std::complex<float>> samples(maximumSamples);
        void* buffers[] = {samples.data()};
        int flags = 0;
        long long timestamp = 0;
        const auto timeoutMicroseconds =
            std::chrono::duration_cast<std::chrono::microseconds>(timeout);
        const auto limitedTimeout = std::min<std::int64_t>(
            timeoutMicroseconds.count(),
            static_cast<std::int64_t>(std::numeric_limits<long>::max()));
        const int count = device_->readStream(
            stream_, buffers, maximumSamples, flags, timestamp, static_cast<long>(limitedTimeout));
        return translateSoapyRead(count, flags, timestamp, std::move(samples), maximumSamples);
    }

    void deactivateRxStream() override {
        if (!active_) {
            return;
        }
        const int result = device_->deactivateStream(stream_);
        if (result != 0) {
            throw std::runtime_error(
                "SoapySDR deactivateStream failed: " + std::string(SoapySDR_errToStr(result)));
        }
        active_ = false;
    }

    void closeRxStream() override {
        if (stream_ == nullptr) {
            return;
        }
        device_->closeStream(stream_);
        stream_ = nullptr;
    }

    void release() {
        if (device_ != nullptr) {
            SoapySDR::Device::unmake(device_);
            device_ = nullptr;
        }
    }

private:
    SoapySDR::Device* device_ = nullptr;
    SoapySDR::Stream* stream_ = nullptr;
    bool active_ = false;
};

} // namespace

std::vector<KeywordMap> RealSoapyApi::enumerate(const KeywordMap& filter) {
    std::vector<KeywordMap> result;
    for (const auto& entry : SoapySDR::Device::enumerate(toNative(filter))) {
        result.push_back(fromNative(entry));
    }
    return result;
}

SoapyDevice* RealSoapyApi::make(const KeywordMap& arguments) {
    return new RealSoapyDevice(SoapySDR::Device::make(toNative(arguments)));
}

void RealSoapyApi::unmake(SoapyDevice* device) {
    auto* real = dynamic_cast<RealSoapyDevice*>(device);
    if (real == nullptr) {
        throw std::invalid_argument("device was not created by RealSoapyApi");
    }
    real->release();
    delete real;
}

std::string RealSoapyApi::libraryVersion() const { return SoapySDR::getLibVersion(); }
std::string RealSoapyApi::apiVersion() const { return SoapySDR::getAPIVersion(); }
std::string RealSoapyApi::abiVersion() const { return SoapySDR::getABIVersion(); }

} // namespace sdrcal::soapy
