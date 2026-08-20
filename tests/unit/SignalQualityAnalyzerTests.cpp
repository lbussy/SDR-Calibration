#include "core/SignalQualityAnalyzer.h"

#include <algorithm>
#include <cmath>
#include <complex>
#include <cstddef>
#include <iostream>
#include <limits>
#include <numbers>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

using namespace sdrcal::core;

namespace {

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

std::vector<std::complex<float>> tone(std::size_t count, double sampleRate,
                                      double midpointFrequency, double drift = 0.0,
                                      double amplitude = 0.5, double fmDeviation = 0.0,
                                      double fmRate = 1.0) {
    std::vector<std::complex<float>> result;
    result.reserve(count);
    const double midpoint = (static_cast<double>(count) - 1.0) / (2.0 * sampleRate);
    for (std::size_t index = 0U; index < count; ++index) {
        const double time = static_cast<double>(index) / sampleRate;
        const double centered = time - midpoint;
        const double fmPhase =
            fmDeviation == 0.0
                ? 0.0
                : fmDeviation / fmRate * std::sin(2.0 * std::numbers::pi * fmRate * time);
        const double phase =
            2.0 * std::numbers::pi *
            (midpointFrequency * time + 0.5 * drift * centered * centered + fmPhase);
        result.emplace_back(static_cast<float>(amplitude * std::cos(phase)),
                            static_cast<float>(amplitude * std::sin(phase)));
    }
    return result;
}

void addNoise(std::vector<std::complex<float>>& values, float deviation) {
    std::mt19937 generator(0x51'6e'a1U);
    std::normal_distribution<float> noise(0.0F, deviation);
    for (auto& value : values)
        value += std::complex<float>{noise(generator), noise(generator)};
}

CarrierEstimate estimate(const std::vector<std::complex<float>>& values, double rate,
                         CarrierEstimatorOptions options = {}) {
    const auto result = estimateCarrier(values, rate, options);
    CHECK(result.ok());
    return result;
}

void testCleanToneAndLinearDrift() {
    constexpr double rate = 8'192.0;
    const auto values = tone(8'192U, rate, 700.0, 0.25);
    const auto result = analyzeSignalQuality(values, rate, estimate(values, rate));
    CHECK(result.succeeded());
    CHECK(result.version == "signal-quality-v1");
    CHECK(result.signal_to_noise_db > 120.0);
    CHECK(result.clipped_samples == 0U);
    CHECK(result.frequency_instability_hz < 0.01);
    CHECK(result.interference_to_carrier_db < -120.0);
    CHECK(result.instability_windows == 8U);
    CHECK(result.fft_samples == 8'192U);
}

void testSeededNoise() {
    constexpr double rate = 8'192.0;
    auto values = tone(8'192U, rate, -500.0);
    addNoise(values, 0.01F);
    const auto result = analyzeSignalQuality(values, rate, estimate(values, rate));
    CHECK(result.succeeded());
    CHECK(result.signal_to_noise_db > 29.0);
    CHECK(result.signal_to_noise_db < 33.0);
    CHECK(result.interference_to_carrier_db < -45.0);
    CHECK(result.frequency_instability_hz < 0.2);
}

void testClippingRule() {
    constexpr double rate = 8'192.0;
    auto values = tone(8'192U, rate, 300.0, 0.0, 1.1);
    for (auto& value : values) {
        value.real(std::clamp(value.real(), -1.0F, 1.0F));
        value.imag(std::clamp(value.imag(), -1.0F, 1.0F));
    }
    CarrierEstimatorOptions estimatorOptions;
    estimatorOptions.minimum_model_coherence = 0.5;
    SignalQualityOptions qualityOptions;
    qualityOptions.version = "signal-quality-clipping-fixture-v1";
    qualityOptions.window_estimator = estimatorOptions;
    const auto result = analyzeSignalQuality(values, rate, estimate(values, rate, estimatorOptions),
                                             qualityOptions);
    CHECK(result.succeeded());
    CHECK(result.clipped_samples > 0U);
    CHECK(result.clipped_samples < values.size());
}

void testInterferingTone() {
    constexpr double rate = 8'192.0;
    auto values = tone(8'192U, rate, 400.0);
    const auto interferer = tone(8'192U, rate, 1'100.0, 0.0, 0.05);
    for (std::size_t index = 0U; index < values.size(); ++index)
        values[index] += interferer[index];
    const auto result = analyzeSignalQuality(values, rate, estimate(values, rate));
    CHECK(result.succeeded());
    CHECK(result.interference_to_carrier_db > -20.5);
    CHECK(result.interference_to_carrier_db < -19.5);
    CHECK(result.signal_to_noise_db > 19.5);
    CHECK(result.signal_to_noise_db < 20.5);
}

void testNonlinearInstability() {
    constexpr double rate = 8'192.0;
    const auto values = tone(16'384U, rate, 600.0, 0.0, 0.5, 1.5, 0.5);
    CarrierEstimatorOptions estimatorOptions;
    estimatorOptions.minimum_model_coherence = 0.5;
    SignalQualityOptions options;
    options.version = "signal-quality-instability-fixture-v1";
    options.window_estimator = estimatorOptions;
    const auto carrier = estimate(values, rate, estimatorOptions);
    const auto result = analyzeSignalQuality(values, rate, carrier, options);
    CHECK(result.succeeded());
    CHECK(carrier.model_coherence > 0.99);
    CHECK(result.signal_to_noise_db < 10.0);
    CHECK(result.frequency_instability_hz > 0.5);
}

void testInvalidInputs() {
    constexpr double rate = 8'192.0;
    auto values = tone(8'192U, rate, 300.0);
    const auto validEstimate = estimate(values, rate);
    SignalQualityOptions mislabeled;
    mislabeled.clipping_rail = 0.95;
    CHECK(analyzeSignalQuality(values, rate, validEstimate, mislabeled).status ==
          SignalQualityStatus::invalid_options);
    auto mismatched = validEstimate;
    --mismatched.sample_count;
    CHECK(analyzeSignalQuality(values, rate, mismatched).status ==
          SignalQualityStatus::invalid_estimate);
    mismatched = validEstimate;
    *mismatched.reference_time_seconds += 0.1;
    CHECK(analyzeSignalQuality(values, rate, mismatched).status ==
          SignalQualityStatus::invalid_estimate);
    SignalQualityOptions changedWindow;
    changedWindow.window_estimator.minimum_model_coherence = 0.5;
    CHECK(analyzeSignalQuality(values, rate, validEstimate, changedWindow).status ==
          SignalQualityStatus::invalid_options);
    SignalQualityOptions bounded;
    bounded.version = "signal-quality-small-working-bound-v1";
    bounded.maximum_working_samples = 4'096U;
    CHECK(analyzeSignalQuality(values, rate, validEstimate, bounded).status ==
          SignalQualityStatus::invalid_input);
    values[10].real(std::numeric_limits<float>::quiet_NaN());
    CHECK(analyzeSignalQuality(values, rate, validEstimate).status ==
          SignalQualityStatus::invalid_input);
    const auto shortValues = tone(1'024U, rate, 300.0);
    const auto shortEstimate = estimate(shortValues, rate);
    CHECK(analyzeSignalQuality(shortValues, rate, shortEstimate).status ==
          SignalQualityStatus::insufficient_windows);
}

} // namespace

int main() {
    try {
        testCleanToneAndLinearDrift();
        testSeededNoise();
        testClippingRule();
        testInterferingTone();
        testNonlinearInstability();
        testInvalidInputs();
        std::cout << "signal-quality analyzer tests passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
