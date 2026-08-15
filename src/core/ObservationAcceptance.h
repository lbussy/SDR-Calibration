#pragma once

#include "core/CarrierEstimator.h"

#include <cstdint>
#include <string>
#include <vector>

namespace sdrcal::core {

enum class EffectiveConfigurationValidity { verified, unverified, mismatch, invalid };

enum class ReferenceSuitability { suitable, unverifiable, unsuitable };

enum class ObservationAcceptanceStatus { accepted, rejected, invalid_policy };

enum class ObservationRejectionCode {
    invalid_policy,
    estimator_rejected,
    invalid_metric,
    insufficient_duration,
    insufficient_snr,
    excessive_clipping,
    missing_samples,
    discontinuity,
    excessive_drift,
    excessive_instability,
    excessive_interference,
    effective_configuration_unverified,
    effective_configuration_mismatch,
    effective_configuration_invalid,
    reference_unverifiable,
    reference_unsuitable,
    reference_conditions_not_met,
};

struct ObservationDiagnostics {
    CarrierEstimate estimate;
    double duration_seconds = 0.0;
    double signal_to_noise_db = 0.0;
    std::uint64_t clipped_samples = 0;
    std::uint64_t missing_samples = 0;
    std::uint64_t discontinuities = 0;
    double frequency_instability_hz = 0.0;
    double interference_to_carrier_db = 0.0;
    EffectiveConfigurationValidity effective_configuration =
        EffectiveConfigurationValidity::unverified;
    ReferenceSuitability reference_suitability = ReferenceSuitability::unverifiable;
    bool reference_conditions_met = false;
};

struct ObservationAcceptancePolicy {
    std::string version = "observation-acceptance-v1";
    double minimum_duration_seconds = 1.0;
    double minimum_signal_to_noise_db = 10.0;
    double maximum_clipped_fraction = 0.0;
    std::uint64_t maximum_missing_samples = 0;
    std::uint64_t maximum_discontinuities = 0;
    double maximum_absolute_drift_hz_per_second = 1.0;
    double maximum_frequency_instability_hz = 0.5;
    double maximum_interference_to_carrier_db = -10.0;
};

struct ObservationRejection {
    ObservationRejectionCode code = ObservationRejectionCode::invalid_metric;
    std::string reason;
};

struct ObservationAcceptanceResult {
    ObservationAcceptanceStatus status = ObservationAcceptanceStatus::rejected;
    std::string policy_version;
    std::vector<ObservationRejection> rejections;

    [[nodiscard]] bool accepted() const noexcept {
        return status == ObservationAcceptanceStatus::accepted;
    }
};

[[nodiscard]] ObservationAcceptanceResult
evaluateObservation(const ObservationDiagnostics& observation,
                    const ObservationAcceptancePolicy& policy = {});

} // namespace sdrcal::core
