#pragma once

#include "core/CarrierEstimator.h"

#include <complex>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>

namespace sdrcal::core {

enum class SignalQualityStatus {
    success,
    invalid_options,
    invalid_input,
    invalid_estimate,
    insufficient_windows,
    window_estimation_failed,
};

struct SignalQualityOptions {
    std::string version = "signal-quality-v1";
    double clipping_rail = 0.999;
    std::size_t instability_window_samples = 1024U;
    std::size_t minimum_instability_windows = 2U;
    std::size_t maximum_working_samples = 16'777'216U;
    std::size_t maximum_fft_samples = 65'536U;
    std::size_t interference_guard_bins = 2U;
    double power_floor_ratio = 1.0e-15;
    CarrierEstimatorOptions window_estimator;
};

struct SignalQualityResult {
    SignalQualityStatus status = SignalQualityStatus::invalid_input;
    std::string reason;
    std::string version;
    double signal_to_noise_db = 0.0;
    std::uint64_t clipped_samples = 0U;
    double frequency_instability_hz = 0.0;
    double interference_to_carrier_db = 0.0;
    double carrier_power = 0.0;
    double residual_power = 0.0;
    std::size_t instability_windows = 0U;
    std::size_t fft_samples = 0U;

    [[nodiscard]] bool succeeded() const noexcept {
        return status == SignalQualityStatus::success;
    }
};

[[nodiscard]] SignalQualityResult analyzeSignalQuality(std::span<const std::complex<float>> samples,
                                                       double sampleRateSps,
                                                       const CarrierEstimate& estimate,
                                                       const SignalQualityOptions& options = {});

} // namespace sdrcal::core
