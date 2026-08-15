#include "core/ObservationAcceptance.h"

#include <cmath>
#include <string>

namespace sdrcal::core {
namespace {

void reject(ObservationAcceptanceResult& result, ObservationRejectionCode code,
            std::string reason) {
    result.rejections.push_back({code, std::move(reason)});
}

bool validPolicy(const ObservationAcceptancePolicy& policy) {
    const ObservationAcceptancePolicy versionOne;
    const bool versionOneValues =
        policy.minimum_duration_seconds == versionOne.minimum_duration_seconds &&
        policy.minimum_signal_to_noise_db == versionOne.minimum_signal_to_noise_db &&
        policy.maximum_clipped_fraction == versionOne.maximum_clipped_fraction &&
        policy.maximum_missing_samples == versionOne.maximum_missing_samples &&
        policy.maximum_discontinuities == versionOne.maximum_discontinuities &&
        policy.maximum_absolute_drift_hz_per_second ==
            versionOne.maximum_absolute_drift_hz_per_second &&
        policy.maximum_frequency_instability_hz == versionOne.maximum_frequency_instability_hz &&
        policy.maximum_interference_to_carrier_db == versionOne.maximum_interference_to_carrier_db;
    const bool versionConsistent = policy.version != versionOne.version || versionOneValues;
    return !policy.version.empty() && versionConsistent &&
           std::isfinite(policy.minimum_duration_seconds) &&
           policy.minimum_duration_seconds > 0.0 &&
           std::isfinite(policy.minimum_signal_to_noise_db) &&
           std::isfinite(policy.maximum_clipped_fraction) &&
           policy.maximum_clipped_fraction >= 0.0 && policy.maximum_clipped_fraction <= 1.0 &&
           std::isfinite(policy.maximum_absolute_drift_hz_per_second) &&
           policy.maximum_absolute_drift_hz_per_second >= 0.0 &&
           std::isfinite(policy.maximum_frequency_instability_hz) &&
           policy.maximum_frequency_instability_hz >= 0.0 &&
           std::isfinite(policy.maximum_interference_to_carrier_db);
}

} // namespace

ObservationAcceptanceResult evaluateObservation(const ObservationDiagnostics& observation,
                                                const ObservationAcceptancePolicy& policy) {
    ObservationAcceptanceResult result;
    result.policy_version = policy.version;
    if (!validPolicy(policy)) {
        result.status = ObservationAcceptanceStatus::invalid_policy;
        reject(result, ObservationRejectionCode::invalid_policy,
               "observation acceptance policy is invalid");
        return result;
    }

    if (!observation.estimate.ok()) {
        reject(result, ObservationRejectionCode::estimator_rejected,
               "carrier estimator did not produce a usable estimate: " +
                   observation.estimate.reason);
    }
    const bool validEstimateOutputs = observation.estimate.frequency_hz.has_value() &&
                                      std::isfinite(*observation.estimate.frequency_hz) &&
                                      observation.estimate.drift_hz_per_second.has_value() &&
                                      std::isfinite(*observation.estimate.drift_hz_per_second) &&
                                      observation.estimate.reference_time_seconds.has_value() &&
                                      std::isfinite(*observation.estimate.reference_time_seconds) &&
                                      *observation.estimate.reference_time_seconds >= 0.0 &&
                                      observation.estimate.sample_count > 0;
    if (!validEstimateOutputs) {
        reject(result, ObservationRejectionCode::invalid_metric,
               "successful estimator output must contain finite frequency, drift, reference time, "
               "and a positive sample count");
    }

    const bool finiteMetrics = std::isfinite(observation.duration_seconds) &&
                               observation.duration_seconds > 0.0 &&
                               std::isfinite(observation.signal_to_noise_db) &&
                               std::isfinite(observation.frequency_instability_hz) &&
                               observation.frequency_instability_hz >= 0.0 &&
                               std::isfinite(observation.interference_to_carrier_db);
    if (!finiteMetrics) {
        reject(result, ObservationRejectionCode::invalid_metric,
               "observation metrics must be finite and duration and instability must be valid");
    } else {
        if (observation.duration_seconds < policy.minimum_duration_seconds) {
            reject(result, ObservationRejectionCode::insufficient_duration,
                   "observation duration is below the policy minimum");
        }
        if (observation.signal_to_noise_db < policy.minimum_signal_to_noise_db) {
            reject(result, ObservationRejectionCode::insufficient_snr,
                   "signal-to-noise ratio is below the policy minimum");
        }
        if (observation.frequency_instability_hz > policy.maximum_frequency_instability_hz) {
            reject(result, ObservationRejectionCode::excessive_instability,
                   "frequency instability exceeds the policy maximum");
        }
        if (observation.interference_to_carrier_db > policy.maximum_interference_to_carrier_db) {
            reject(result, ObservationRejectionCode::excessive_interference,
                   "interference-to-carrier ratio exceeds the policy maximum");
        }
    }

    if (observation.estimate.sample_count == 0) {
        reject(result, ObservationRejectionCode::invalid_metric,
               "estimator sample count must be positive");
    } else {
        const double clippedFraction = static_cast<double>(observation.clipped_samples) /
                                       static_cast<double>(observation.estimate.sample_count);
        if (observation.clipped_samples > observation.estimate.sample_count) {
            reject(result, ObservationRejectionCode::invalid_metric,
                   "clipped sample count exceeds the estimator sample count");
        } else if (clippedFraction > policy.maximum_clipped_fraction) {
            reject(result, ObservationRejectionCode::excessive_clipping,
                   "clipped sample fraction exceeds the policy maximum");
        }
    }
    if (observation.missing_samples > policy.maximum_missing_samples) {
        reject(result, ObservationRejectionCode::missing_samples,
               "missing sample count exceeds the policy maximum");
    }
    if (observation.discontinuities > policy.maximum_discontinuities) {
        reject(result, ObservationRejectionCode::discontinuity,
               "discontinuity count exceeds the policy maximum");
    }

    if (observation.estimate.drift_hz_per_second.has_value() &&
        std::isfinite(*observation.estimate.drift_hz_per_second) &&
        std::abs(*observation.estimate.drift_hz_per_second) >
            policy.maximum_absolute_drift_hz_per_second) {
        reject(result, ObservationRejectionCode::excessive_drift,
               "absolute carrier drift exceeds the policy maximum");
    }

    switch (observation.effective_configuration) {
    case EffectiveConfigurationValidity::verified:
        break;
    case EffectiveConfigurationValidity::unverified:
        reject(result, ObservationRejectionCode::effective_configuration_unverified,
               "effective device configuration is unverified");
        break;
    case EffectiveConfigurationValidity::mismatch:
        reject(result, ObservationRejectionCode::effective_configuration_mismatch,
               "effective device configuration does not satisfy the observation request");
        break;
    case EffectiveConfigurationValidity::invalid:
        reject(result, ObservationRejectionCode::effective_configuration_invalid,
               "effective device configuration is invalid");
        break;
    default:
        reject(result, ObservationRejectionCode::effective_configuration_invalid,
               "effective device configuration state is unknown");
        break;
    }

    switch (observation.reference_suitability) {
    case ReferenceSuitability::suitable:
        break;
    case ReferenceSuitability::unverifiable:
        reject(result, ObservationRejectionCode::reference_unverifiable,
               "reference suitability cannot be verified");
        break;
    case ReferenceSuitability::unsuitable:
        reject(result, ObservationRejectionCode::reference_unsuitable,
               "reference is unsuitable for this observation");
        break;
    default:
        reject(result, ObservationRejectionCode::reference_unverifiable,
               "reference suitability state is unknown");
        break;
    }
    if (!observation.reference_conditions_met) {
        reject(result, ObservationRejectionCode::reference_conditions_not_met,
               "documented reference conditions are not met");
    }

    result.status = result.rejections.empty() ? ObservationAcceptanceStatus::accepted
                                              : ObservationAcceptanceStatus::rejected;
    return result;
}

} // namespace sdrcal::core
