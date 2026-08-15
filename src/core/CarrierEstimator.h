#pragma once

#include <complex>
#include <cstddef>
#include <optional>
#include <span>
#include <string>

namespace sdrcal::core {

enum class CarrierEstimateStatus {
    success,
    invalid_options,
    invalid_sample_rate,
    insufficient_samples,
    nonfinite_sample,
    insufficient_energy,
    ambiguous_signal,
    frequency_out_of_range,
};

struct CarrierEstimatorOptions {
    std::size_t minimum_samples = 64;
    double minimum_mean_power = 1.0e-12;
    double minimum_model_coherence = 0.75;
    double maximum_absolute_frequency_fraction = 0.45;
    std::size_t robust_iterations = 4;
    double huber_threshold_radians = 0.35;
};

struct CarrierEstimate {
    CarrierEstimateStatus status = CarrierEstimateStatus::insufficient_samples;
    std::string reason;
    std::optional<double> frequency_hz;
    std::optional<double> drift_hz_per_second;
    std::optional<double> reference_time_seconds;
    double mean_power = 0.0;
    double model_coherence = 0.0;
    double residual_phase_rms_radians = 0.0;
    std::size_t sample_count = 0;

    [[nodiscard]] bool ok() const noexcept {
        return status == CarrierEstimateStatus::success;
    }
};

[[nodiscard]] CarrierEstimate estimateCarrier(std::span<const std::complex<float>> samples,
                                              double sampleRateSps,
                                              const CarrierEstimatorOptions& options = {});

} // namespace sdrcal::core
