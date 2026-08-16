#include "core/SignalQualityAnalyzer.h"

#include <algorithm>
#include <bit>
#include <cmath>
#include <complex>
#include <limits>
#include <numbers>
#include <utility>
#include <vector>

namespace sdrcal::core {
namespace {

SignalQualityResult failure(SignalQualityStatus status, std::string reason,
                            const SignalQualityOptions& options) {
    SignalQualityResult result;
    result.status = status;
    result.reason = std::move(reason);
    result.version = options.version;
    return result;
}

bool validOptions(const SignalQualityOptions& options) {
    const SignalQualityOptions versionOne;
    const auto& window = options.window_estimator;
    const auto& versionOneWindow = versionOne.window_estimator;
    const bool windowValues =
        window.minimum_samples == versionOneWindow.minimum_samples &&
        window.minimum_mean_power == versionOneWindow.minimum_mean_power &&
        window.minimum_model_coherence == versionOneWindow.minimum_model_coherence &&
        window.maximum_absolute_frequency_fraction ==
            versionOneWindow.maximum_absolute_frequency_fraction &&
        window.robust_iterations == versionOneWindow.robust_iterations &&
        window.huber_threshold_radians == versionOneWindow.huber_threshold_radians;
    const bool versionOneValues =
        options.clipping_rail == versionOne.clipping_rail &&
        options.instability_window_samples == versionOne.instability_window_samples &&
        options.minimum_instability_windows == versionOne.minimum_instability_windows &&
        options.maximum_working_samples == versionOne.maximum_working_samples &&
        options.maximum_fft_samples == versionOne.maximum_fft_samples &&
        options.interference_guard_bins == versionOne.interference_guard_bins &&
        options.power_floor_ratio == versionOne.power_floor_ratio && windowValues;
    const bool versionConsistent = options.version != versionOne.version || versionOneValues;
    const bool validWindow =
        window.minimum_samples >= 3U && std::isfinite(window.minimum_mean_power) &&
        window.minimum_mean_power >= 0.0 && std::isfinite(window.minimum_model_coherence) &&
        window.minimum_model_coherence >= 0.0 && window.minimum_model_coherence <= 1.0 &&
        std::isfinite(window.maximum_absolute_frequency_fraction) &&
        window.maximum_absolute_frequency_fraction > 0.0 &&
        window.maximum_absolute_frequency_fraction < 0.5 && window.robust_iterations <= 32U &&
        std::isfinite(window.huber_threshold_radians) && window.huber_threshold_radians > 0.0;
    return !options.version.empty() && versionConsistent && std::isfinite(options.clipping_rail) &&
           options.clipping_rail > 0.0 && options.clipping_rail <= 1.0 &&
           options.instability_window_samples >= 3U && options.minimum_instability_windows >= 2U &&
           options.maximum_working_samples >= 8U && options.maximum_fft_samples >= 8U &&
           options.instability_window_samples <= options.maximum_working_samples &&
           options.minimum_instability_windows <=
               options.maximum_working_samples / options.instability_window_samples &&
           options.interference_guard_bins >= 1U &&
           options.interference_guard_bins < (options.maximum_fft_samples - 1U) / 2U &&
           std::has_single_bit(options.maximum_fft_samples) &&
           std::isfinite(options.power_floor_ratio) && options.power_floor_ratio > 0.0 &&
           options.power_floor_ratio < 1.0 && validWindow;
}

std::size_t boundedFftSize(std::size_t sampleCount, std::size_t maximum) {
    std::size_t value = 1U;
    while (value <= sampleCount / 2U && value < maximum)
        value *= 2U;
    return value;
}

void fft(std::vector<std::complex<double>>& values) {
    const std::size_t count = values.size();
    for (std::size_t index = 1U, reverse = 0U; index < count; ++index) {
        std::size_t bit = count >> 1U;
        for (; (reverse & bit) != 0U; bit >>= 1U)
            reverse ^= bit;
        reverse ^= bit;
        if (index < reverse)
            std::swap(values[index], values[reverse]);
    }
    for (std::size_t length = 2U; length <= count; length <<= 1U) {
        const auto step = std::polar(1.0, -2.0 * std::numbers::pi / static_cast<double>(length));
        for (std::size_t start = 0U; start < count; start += length) {
            std::complex<double> factor{1.0, 0.0};
            for (std::size_t offset = 0U; offset < length / 2U; ++offset) {
                const auto even = values[start + offset];
                const auto odd = values[start + offset + length / 2U] * factor;
                values[start + offset] = even + odd;
                values[start + offset + length / 2U] = even - odd;
                factor *= step;
            }
        }
    }
}

} // namespace

SignalQualityResult analyzeSignalQuality(std::span<const std::complex<float>> samples,
                                         double sampleRateSps, const CarrierEstimate& estimate,
                                         const SignalQualityOptions& options) {
    if (!validOptions(options))
        return failure(SignalQualityStatus::invalid_options,
                       "signal-quality options are invalid or mislabeled", options);
    if (!std::isfinite(sampleRateSps) || sampleRateSps <= 0.0 || samples.empty() ||
        samples.size() > options.maximum_working_samples)
        return failure(SignalQualityStatus::invalid_input,
                       "samples and sample rate are empty, invalid, or exceed the working bound",
                       options);
    const double expectedMidpoint =
        (static_cast<double>(samples.size()) - 1.0) / (2.0 * sampleRateSps);
    if (!estimate.ok() || !estimate.frequency_hz || !estimate.drift_hz_per_second ||
        !estimate.reference_time_seconds || estimate.sample_count != samples.size() ||
        !std::isfinite(*estimate.frequency_hz) || !std::isfinite(*estimate.drift_hz_per_second) ||
        !std::isfinite(*estimate.reference_time_seconds) || !std::isfinite(estimate.mean_power) ||
        estimate.mean_power <= 0.0 || !std::isfinite(estimate.model_coherence) ||
        estimate.model_coherence < 0.0 || estimate.model_coherence > 1.0 ||
        std::abs(*estimate.reference_time_seconds - expectedMidpoint) >
            std::numeric_limits<double>::epsilon() * std::max(1.0, expectedMidpoint) * 8.0)
        return failure(SignalQualityStatus::invalid_estimate,
                       "carrier estimate is unsuccessful, incomplete, non-finite, or mismatched",
                       options);
    if (options.instability_window_samples < options.window_estimator.minimum_samples)
        return failure(SignalQualityStatus::invalid_options,
                       "instability window is smaller than the estimator minimum", options);

    const double midpoint = *estimate.reference_time_seconds;
    std::vector<std::complex<double>> demodulated;
    demodulated.reserve(samples.size());
    std::complex<double> coherent{0.0, 0.0};
    std::uint64_t clipped = 0U;
    for (std::size_t index = 0U; index < samples.size(); ++index) {
        const double real = static_cast<double>(samples[index].real());
        const double imaginary = static_cast<double>(samples[index].imag());
        if (!std::isfinite(real) || !std::isfinite(imaginary))
            return failure(SignalQualityStatus::invalid_input,
                           "signal-quality input contains a non-finite sample", options);
        if (std::abs(real) >= options.clipping_rail || std::abs(imaginary) >= options.clipping_rail)
            ++clipped;
        const double time = static_cast<double>(index) / sampleRateSps;
        const double centered = time - midpoint;
        const double phase = 2.0 * std::numbers::pi *
                             (*estimate.frequency_hz * time +
                              0.5 * *estimate.drift_hz_per_second * centered * centered);
        const std::complex<double> sample{real, imaginary};
        const auto value = sample * std::polar(1.0, -phase);
        demodulated.push_back(value);
        coherent += value;
    }
    coherent /= static_cast<double>(demodulated.size());
    const double carrierPower = std::norm(coherent);
    if (!std::isfinite(carrierPower) || carrierPower <= 0.0)
        return failure(SignalQualityStatus::invalid_input,
                       "fitted carrier power is not finite and positive", options);
    double residualPower = 0.0;
    for (auto& value : demodulated) {
        value -= coherent;
        residualPower += std::norm(value);
    }
    residualPower /= static_cast<double>(demodulated.size());
    const double floorPower = carrierPower * options.power_floor_ratio;
    const double boundedResidual = std::max(residualPower, floorPower);

    const std::size_t fullWindows = samples.size() / options.instability_window_samples;
    if (fullWindows < options.minimum_instability_windows)
        return failure(SignalQualityStatus::insufficient_windows,
                       "input does not contain the required instability windows", options);
    double instability = 0.0;
    for (std::size_t window = 0U; window < fullWindows; ++window) {
        const std::size_t first = window * options.instability_window_samples;
        const auto windowSamples = samples.subspan(first, options.instability_window_samples);
        const auto windowEstimate =
            estimateCarrier(windowSamples, sampleRateSps, options.window_estimator);
        if (!windowEstimate.ok() || !windowEstimate.frequency_hz ||
            !windowEstimate.reference_time_seconds)
            return failure(SignalQualityStatus::window_estimation_failed,
                           "an instability-window carrier estimate failed", options);
        const double absoluteWindowTime =
            static_cast<double>(first) / sampleRateSps + *windowEstimate.reference_time_seconds;
        const double expected = *estimate.frequency_hz +
                                *estimate.drift_hz_per_second * (absoluteWindowTime - midpoint);
        instability = std::max(instability, std::abs(*windowEstimate.frequency_hz - expected));
    }

    const std::size_t fftSamples = boundedFftSize(demodulated.size(), options.maximum_fft_samples);
    if (fftSamples <= options.interference_guard_bins * 2U + 1U)
        return failure(SignalQualityStatus::invalid_input,
                       "input is too short for the interference FFT guard", options);
    const std::size_t firstFftSample = (demodulated.size() - fftSamples) / 2U;
    std::vector<std::complex<double>> spectrum(fftSamples);
    double windowSum = 0.0;
    for (std::size_t index = 0U; index < fftSamples; ++index) {
        const double weight =
            0.5 - 0.5 * std::cos(2.0 * std::numbers::pi * static_cast<double>(index) /
                                 static_cast<double>(fftSamples - 1U));
        spectrum[index] = demodulated[firstFftSample + index] * weight;
        windowSum += weight;
    }
    fft(spectrum);
    double strongestPower = floorPower;
    for (std::size_t bin = options.interference_guard_bins + 1U;
         bin + options.interference_guard_bins < fftSamples; ++bin) {
        const double amplitude = std::abs(spectrum[bin]) / windowSum;
        strongestPower = std::max(strongestPower, amplitude * amplitude);
    }

    SignalQualityResult result;
    result.status = SignalQualityStatus::success;
    result.reason = "signal-quality analysis completed";
    result.version = options.version;
    result.signal_to_noise_db = 10.0 * std::log10(carrierPower / boundedResidual);
    result.clipped_samples = clipped;
    result.frequency_instability_hz = instability;
    result.interference_to_carrier_db = 10.0 * std::log10(strongestPower / carrierPower);
    result.carrier_power = carrierPower;
    result.residual_power = residualPower;
    result.instability_windows = fullWindows;
    result.fft_samples = fftSamples;
    return result;
}

} // namespace sdrcal::core
