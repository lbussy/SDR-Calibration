#include "core/CarrierEstimator.h"

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
                                      double amplitude = 1.0) {
    std::vector<std::complex<float>> result;
    result.reserve(count);
    const double midpoint = (static_cast<double>(count) - 1.0) / (2.0 * sampleRate);
    for (std::size_t index = 0; index < count; ++index) {
        const double time = static_cast<double>(index) / sampleRate;
        const double centeredTime = time - midpoint;
        const double phase = 2.0 * std::numbers::pi *
                             (midpointFrequency * time + 0.5 * drift * centeredTime * centeredTime);
        result.emplace_back(static_cast<float>(amplitude * std::cos(phase)),
                            static_cast<float>(amplitude * std::sin(phase)));
    }
    return result;
}

void addNoise(std::vector<std::complex<float>>& samples, float standardDeviation) {
    std::mt19937 generator(0x5d'12'caU);
    std::normal_distribution<float> noise(0.0F, standardDeviation);
    for (auto& sample : samples) {
        sample += std::complex<float>{noise(generator), noise(generator)};
    }
}

void testKnownOffsets() {
    constexpr double sampleRate = 8'000.0;
    for (const double frequency : {-1'237.25, -11.0, 0.0, 876.5}) {
        const auto result = estimateCarrier(tone(8'192, sampleRate, frequency), sampleRate);
        CHECK(result.ok());
        CHECK(result.frequency_hz.has_value());
        CHECK(result.drift_hz_per_second.has_value());
        CHECK(std::abs(*result.frequency_hz - frequency) < 0.002);
        CHECK(std::abs(*result.drift_hz_per_second) < 0.01);
        CHECK(result.model_coherence > 0.99999);
    }
}

void testLinearDriftAtMidpoint() {
    constexpr double sampleRate = 4'000.0;
    const auto result = estimateCarrier(tone(12'000, sampleRate, 321.25, 7.5), sampleRate);
    CHECK(result.ok());
    CHECK(std::abs(result.frequency_hz.value() - 321.25) < 0.003);
    CHECK(std::abs(result.drift_hz_per_second.value() - 7.5) < 0.003);
    CHECK(std::abs(result.reference_time_seconds.value() - 1.499875) < 1.0e-9);
}

void testNoise() {
    constexpr double sampleRate = 12'000.0;
    auto samples = tone(24'000, sampleRate, -733.75, 0.4);
    addNoise(samples, 0.08F);
    const auto result = estimateCarrier(samples, sampleRate);
    CHECK(result.ok());
    CHECK(std::abs(result.frequency_hz.value() + 733.75) < 0.3);
    CHECK(std::abs(result.drift_hz_per_second.value() - 0.4) < 0.35);
    CHECK(result.model_coherence > 0.9);
}

void testClipping() {
    constexpr double sampleRate = 10'000.0;
    auto samples = tone(10'000, sampleRate, 1'111.0, -1.25, 2.0);
    for (auto& sample : samples) {
        sample.real(std::clamp(sample.real(), -1.5F, 1.5F));
        sample.imag(std::clamp(sample.imag(), -1.5F, 1.5F));
    }
    const auto result = estimateCarrier(samples, sampleRate);
    CHECK(result.ok());
    CHECK(std::abs(result.frequency_hz.value() - 1'111.0) < 0.1);
    CHECK(std::abs(result.drift_hz_per_second.value() + 1.25) < 0.2);
}

void testInterferenceIsUnsuitable() {
    constexpr double sampleRate = 8'000.0;
    auto samples = tone(16'000, sampleRate, 400.0);
    const auto interferer = tone(16'000, sampleRate, 1'650.0, 0.0, 0.95);
    for (std::size_t index = 0; index < samples.size(); ++index) {
        samples[index] += interferer[index];
    }
    const auto result = estimateCarrier(samples, sampleRate);
    CHECK(result.status == CarrierEstimateStatus::ambiguous_signal);
    CHECK(!result.frequency_hz.has_value());
    CHECK(!result.reason.empty());
    CHECK(result.model_coherence < 0.75);
}

void testUnsuitableInputs() {
    const std::vector<std::complex<float>> shortInput(8, {1.0F, 0.0F});
    CHECK(estimateCarrier(shortInput, 8'000.0).status ==
          CarrierEstimateStatus::insufficient_samples);
    const std::vector<std::complex<float>> silence(128, {0.0F, 0.0F});
    CHECK(estimateCarrier(silence, 8'000.0).status == CarrierEstimateStatus::insufficient_energy);
    auto nonfinite = tone(128, 8'000.0, 100.0);
    nonfinite[40].real(std::numeric_limits<float>::quiet_NaN());
    CHECK(estimateCarrier(nonfinite, 8'000.0).status == CarrierEstimateStatus::nonfinite_sample);
    CHECK(estimateCarrier(tone(128, 8'000.0, 100.0), 0.0).status ==
          CarrierEstimateStatus::invalid_sample_rate);
    CHECK(estimateCarrier(tone(512, 8'000.0, 3'800.0), 8'000.0).status ==
          CarrierEstimateStatus::frequency_out_of_range);
    CHECK(estimateCarrier(tone(8'000, 8'000.0, 0.0, 8'000.0), 8'000.0).status ==
          CarrierEstimateStatus::frequency_out_of_range);
}

void testInvalidOptions() {
    CarrierEstimatorOptions options;
    options.minimum_model_coherence = 1.1;
    CHECK(estimateCarrier(tone(128, 8'000.0, 100.0), 8'000.0, options).status ==
          CarrierEstimateStatus::invalid_options);
    options = {};
    options.robust_iterations = 33;
    CHECK(estimateCarrier(tone(128, 8'000.0, 100.0), 8'000.0, options).status ==
          CarrierEstimateStatus::invalid_options);
    options = {};
    options.minimum_samples = 2;
    CHECK(estimateCarrier(tone(128, 8'000.0, 100.0), 8'000.0, options).status ==
          CarrierEstimateStatus::invalid_options);
}

} // namespace

int main() {
    try {
        testKnownOffsets();
        testLinearDriftAtMidpoint();
        testNoise();
        testClipping();
        testInterferenceIsUnsuitable();
        testUnsuitableInputs();
        testInvalidOptions();
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
    std::cout << "carrier estimator tests passed\n";
    return 0;
}
