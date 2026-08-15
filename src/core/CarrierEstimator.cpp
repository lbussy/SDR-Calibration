#include "core/CarrierEstimator.h"

#include <algorithm>
#include <cmath>
#include <complex>
#include <limits>
#include <numbers>
#include <vector>

namespace sdrcal::core {
namespace {

struct Increment {
    double phase = 0.0;
    double time = 0.0;
    double base_weight = 0.0;
    double weight = 0.0;
};

struct Fit {
    bool valid = false;
    double intercept = 0.0;
    double slope = 0.0;
};

Fit weightedFit(const std::vector<Increment>& increments) {
    double sumWeight = 0.0;
    double sumTimeSquared = 0.0;
    double sumPhase = 0.0;
    double sumTimePhase = 0.0;
    for (const auto& increment : increments) {
        sumWeight += increment.weight;
        sumTimeSquared += increment.weight * increment.time * increment.time;
        sumPhase += increment.weight * increment.phase;
        sumTimePhase += increment.weight * increment.time * increment.phase;
    }
    if (!(sumWeight > 0.0) || !(sumTimeSquared > 0.0)) {
        return {};
    }
    return {true, sumPhase / sumWeight, sumTimePhase / sumTimeSquared};
}

CarrierEstimate failure(CarrierEstimateStatus status, std::string reason, std::size_t sampleCount) {
    CarrierEstimate result;
    result.status = status;
    result.reason = std::move(reason);
    result.sample_count = sampleCount;
    return result;
}

} // namespace

CarrierEstimate estimateCarrier(std::span<const std::complex<float>> samples, double sampleRateSps,
                                const CarrierEstimatorOptions& options) {
    if (!std::isfinite(sampleRateSps) || sampleRateSps <= 0.0) {
        return failure(CarrierEstimateStatus::invalid_sample_rate,
                       "sample rate must be finite and positive", samples.size());
    }
    if (options.minimum_samples < 3 || !std::isfinite(options.minimum_mean_power) ||
        options.minimum_mean_power < 0.0 || !std::isfinite(options.minimum_model_coherence) ||
        options.minimum_model_coherence < 0.0 || options.minimum_model_coherence > 1.0 ||
        !std::isfinite(options.maximum_absolute_frequency_fraction) ||
        options.maximum_absolute_frequency_fraction <= 0.0 ||
        options.maximum_absolute_frequency_fraction >= 0.5 || options.robust_iterations > 32 ||
        !std::isfinite(options.huber_threshold_radians) || options.huber_threshold_radians <= 0.0) {
        return failure(CarrierEstimateStatus::invalid_options, "estimator options are invalid",
                       samples.size());
    }
    if (samples.size() < options.minimum_samples) {
        return failure(CarrierEstimateStatus::insufficient_samples,
                       "input does not contain the required number of samples", samples.size());
    }

    double totalPower = 0.0;
    for (const auto& sample : samples) {
        const double real = static_cast<double>(sample.real());
        const double imaginary = static_cast<double>(sample.imag());
        if (!std::isfinite(real) || !std::isfinite(imaginary)) {
            return failure(CarrierEstimateStatus::nonfinite_sample,
                           "input contains a non-finite complex component", samples.size());
        }
        totalPower += real * real + imaginary * imaginary;
    }
    const double meanPower = totalPower / static_cast<double>(samples.size());
    if (!std::isfinite(meanPower) || meanPower < options.minimum_mean_power) {
        return failure(CarrierEstimateStatus::insufficient_energy,
                       "input mean power is below the estimator floor", samples.size());
    }

    const double midpoint = (static_cast<double>(samples.size()) - 1.0) / (2.0 * sampleRateSps);
    std::vector<Increment> increments;
    increments.reserve(samples.size() - 1);
    for (std::size_t index = 0; index + 1 < samples.size(); ++index) {
        const std::complex<double> first{static_cast<double>(samples[index].real()),
                                         static_cast<double>(samples[index].imag())};
        const std::complex<double> second{static_cast<double>(samples[index + 1].real()),
                                          static_cast<double>(samples[index + 1].imag())};
        const auto product = std::conj(first) * second;
        const double productMagnitude = std::abs(first) * std::abs(second);
        const double weight = productMagnitude >= options.minimum_mean_power ? 1.0 : 0.0;
        const double time = (static_cast<double>(index) + 0.5) / sampleRateSps - midpoint;
        increments.push_back({std::arg(product), time, weight, weight});
    }

    Fit fit = weightedFit(increments);
    if (!fit.valid) {
        return failure(CarrierEstimateStatus::insufficient_energy,
                       "adjacent samples do not carry usable phase information", samples.size());
    }
    for (std::size_t iteration = 0; iteration < options.robust_iterations; ++iteration) {
        for (auto& increment : increments) {
            const double residual =
                std::remainder(increment.phase - (fit.intercept + fit.slope * increment.time),
                               2.0 * std::numbers::pi);
            const double magnitude = std::abs(residual);
            const double robustWeight = magnitude <= options.huber_threshold_radians
                                            ? 1.0
                                            : options.huber_threshold_radians / magnitude;
            increment.weight = increment.base_weight * robustWeight;
        }
        fit = weightedFit(increments);
        if (!fit.valid) {
            return failure(CarrierEstimateStatus::ambiguous_signal,
                           "robust phase model became singular", samples.size());
        }
    }

    std::complex<double> coherentSum{0.0, 0.0};
    double sumWeight = 0.0;
    double squaredResidualSum = 0.0;
    for (const auto& increment : increments) {
        const double residual = std::remainder(
            increment.phase - (fit.intercept + fit.slope * increment.time), 2.0 * std::numbers::pi);
        coherentSum += increment.base_weight * std::polar(1.0, residual);
        sumWeight += increment.base_weight;
        squaredResidualSum += increment.base_weight * residual * residual;
    }
    const double coherence = sumWeight > 0.0 ? std::abs(coherentSum) / sumWeight : 0.0;
    const double residualRms = sumWeight > 0.0 ? std::sqrt(squaredResidualSum / sumWeight) : 0.0;
    if (!std::isfinite(coherence) || coherence < options.minimum_model_coherence) {
        CarrierEstimate result =
            failure(CarrierEstimateStatus::ambiguous_signal,
                    "phase evolution is not coherent with one drifting carrier", samples.size());
        result.mean_power = meanPower;
        result.model_coherence = coherence;
        result.residual_phase_rms_radians = residualRms;
        return result;
    }

    const double radiansToHz = sampleRateSps / (2.0 * std::numbers::pi);
    const double frequency = fit.intercept * radiansToHz;
    const double drift = fit.slope * radiansToHz;
    const double maximumFrequency = options.maximum_absolute_frequency_fraction * sampleRateSps;
    const double startFrequency = frequency - drift * midpoint;
    const double endFrequency = frequency + drift * midpoint;
    if (!std::isfinite(frequency) || !std::isfinite(drift) ||
        std::abs(startFrequency) > maximumFrequency || std::abs(endFrequency) > maximumFrequency) {
        return failure(CarrierEstimateStatus::frequency_out_of_range,
                       "estimated frequency model crosses the configured Nyquist margin",
                       samples.size());
    }

    CarrierEstimate result;
    result.status = CarrierEstimateStatus::success;
    result.reason = "carrier estimate completed";
    result.frequency_hz = frequency;
    result.drift_hz_per_second = drift;
    result.reference_time_seconds = midpoint;
    result.mean_power = meanPower;
    result.model_coherence = coherence;
    result.residual_phase_rms_radians = residualRms;
    result.sample_count = samples.size();
    return result;
}

} // namespace sdrcal::core
