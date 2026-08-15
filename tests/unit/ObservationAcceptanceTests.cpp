#include "core/ObservationAcceptance.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

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

ObservationDiagnostics acceptableObservation() {
    ObservationDiagnostics observation;
    observation.estimate.status = CarrierEstimateStatus::success;
    observation.estimate.reason = "carrier estimate completed";
    observation.estimate.frequency_hz = 25.0;
    observation.estimate.drift_hz_per_second = 0.1;
    observation.estimate.reference_time_seconds = 5.0;
    observation.estimate.sample_count = 10'000;
    observation.duration_seconds = 10.0;
    observation.signal_to_noise_db = 20.0;
    observation.frequency_instability_hz = 0.1;
    observation.interference_to_carrier_db = -20.0;
    observation.effective_configuration = EffectiveConfigurationValidity::verified;
    observation.reference_suitability = ReferenceSuitability::suitable;
    observation.reference_conditions_met = true;
    return observation;
}

bool hasCode(const ObservationAcceptanceResult& result, ObservationRejectionCode code) {
    return std::ranges::any_of(result.rejections,
                               [code](const auto& rejection) { return rejection.code == code; });
}

void testAcceptsBoundaryValues() {
    auto observation = acceptableObservation();
    observation.duration_seconds = 1.0;
    observation.signal_to_noise_db = 10.0;
    observation.frequency_instability_hz = 0.5;
    observation.interference_to_carrier_db = -10.0;
    observation.estimate.drift_hz_per_second = -1.0;

    const auto result = evaluateObservation(observation);
    CHECK(result.accepted());
    CHECK(result.policy_version == "observation-acceptance-v1");
    CHECK(result.rejections.empty());
}

void testCollectsQualityFailures() {
    auto observation = acceptableObservation();
    observation.signal_to_noise_db = 9.9;
    observation.clipped_samples = 1;
    observation.missing_samples = 1;
    observation.discontinuities = 1;
    observation.estimate.drift_hz_per_second = 1.01;
    observation.frequency_instability_hz = 0.51;
    observation.interference_to_carrier_db = -9.9;

    const auto result = evaluateObservation(observation);
    CHECK(result.status == ObservationAcceptanceStatus::rejected);
    CHECK(hasCode(result, ObservationRejectionCode::insufficient_snr));
    CHECK(hasCode(result, ObservationRejectionCode::excessive_clipping));
    CHECK(hasCode(result, ObservationRejectionCode::missing_samples));
    CHECK(hasCode(result, ObservationRejectionCode::discontinuity));
    CHECK(hasCode(result, ObservationRejectionCode::excessive_drift));
    CHECK(hasCode(result, ObservationRejectionCode::excessive_instability));
    CHECK(hasCode(result, ObservationRejectionCode::excessive_interference));
    for (const auto& rejection : result.rejections) {
        CHECK(!rejection.reason.empty());
    }
}

void testFailsClosedOnProvenance() {
    auto observation = acceptableObservation();
    observation.effective_configuration = EffectiveConfigurationValidity::unverified;
    observation.reference_suitability = ReferenceSuitability::unverifiable;
    observation.reference_conditions_met = false;

    const auto result = evaluateObservation(observation);
    CHECK(hasCode(result, ObservationRejectionCode::effective_configuration_unverified));
    CHECK(hasCode(result, ObservationRejectionCode::reference_unverifiable));
    CHECK(hasCode(result, ObservationRejectionCode::reference_conditions_not_met));

    observation.effective_configuration = EffectiveConfigurationValidity::mismatch;
    observation.reference_suitability = ReferenceSuitability::unsuitable;
    const auto mismatch = evaluateObservation(observation);
    CHECK(hasCode(mismatch, ObservationRejectionCode::effective_configuration_mismatch));
    CHECK(hasCode(mismatch, ObservationRejectionCode::reference_unsuitable));

    observation.effective_configuration = EffectiveConfigurationValidity::invalid;
    observation.reference_suitability = static_cast<ReferenceSuitability>(99);
    const auto invalid = evaluateObservation(observation);
    CHECK(hasCode(invalid, ObservationRejectionCode::effective_configuration_invalid));
    CHECK(hasCode(invalid, ObservationRejectionCode::reference_unverifiable));
}

void testRejectsEstimatorFailureAndInvalidMetrics() {
    auto observation = acceptableObservation();
    observation.estimate.status = CarrierEstimateStatus::ambiguous_signal;
    observation.estimate.reason = "ambiguous";
    observation.estimate.drift_hz_per_second.reset();
    observation.duration_seconds = std::numeric_limits<double>::quiet_NaN();
    observation.clipped_samples = 20'000;

    const auto result = evaluateObservation(observation);
    CHECK(hasCode(result, ObservationRejectionCode::estimator_rejected));
    CHECK(hasCode(result, ObservationRejectionCode::invalid_metric));
    CHECK(result.rejections.size() >= 3);
}

void testRejectsIncompleteSuccessfulEstimate() {
    auto observation = acceptableObservation();
    observation.estimate.frequency_hz.reset();
    observation.estimate.reference_time_seconds = std::numeric_limits<double>::quiet_NaN();
    const auto result = evaluateObservation(observation);
    CHECK(result.status == ObservationAcceptanceStatus::rejected);
    CHECK(hasCode(result, ObservationRejectionCode::invalid_metric));
}

void testRejectsInvalidPolicy() {
    ObservationAcceptancePolicy policy;
    policy.maximum_clipped_fraction = 1.1;
    const auto result = evaluateObservation(acceptableObservation(), policy);
    CHECK(result.status == ObservationAcceptanceStatus::invalid_policy);
    CHECK(result.rejections.size() == 1);
    CHECK(result.rejections.front().code == ObservationRejectionCode::invalid_policy);
    CHECK(!result.rejections.front().reason.empty());

    policy.version = "site-policy-v1";
    policy.maximum_clipped_fraction = 0.01;
    const auto custom = evaluateObservation(acceptableObservation(), policy);
    CHECK(custom.accepted());
    CHECK(custom.policy_version == "site-policy-v1");
}

} // namespace

int main() {
    try {
        testAcceptsBoundaryValues();
        testCollectsQualityFailures();
        testFailsClosedOnProvenance();
        testRejectsEstimatorFailureAndInvalidMetrics();
        testRejectsIncompleteSuccessfulEstimate();
        testRejectsInvalidPolicy();
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
    std::cout << "observation acceptance tests passed\n";
    return 0;
}
