#include "core/CalibrationModel.h"
#include "core/ReliabilityQuotient.h"
#include "core/Uncertainty.h"

#include <cmath>
#include <iostream>
#include <limits>
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

bool near(double actual, double expected, double tolerance = 1e-9) {
    return std::abs(actual - expected) <= tolerance;
}

void testLocalModelSignAndExactDomain() {
    const auto fit = fitLocalConstantModel(
        {10'000'003.0, 10'000'000.0, true, "observation-acceptance-v1", "capture-1"});
    CHECK(fit.succeeded());
    CHECK(near(fit.model.intercept_error_hz, 3.0));
    CHECK(fit.model.minimum_frequency_hz == fit.model.maximum_frequency_hz);

    const auto evaluated = evaluateModel(fit.model, 10'000'003.0);
    CHECK(evaluated.succeeded());
    CHECK(near(evaluated.estimated_true_frequency_hz, 10'000'000.0));
    CHECK(evaluateModel(fit.model, 10'000'004.0).status ==
          ModelEvaluationStatus::outside_validity_domain);
}

void testLinearGoldenFitAndBoundaries() {
    const std::vector<ModelObservation> observations = {
        {9'000'000.0, 8'999'996.0, true, "observation-acceptance-v1", "capture-1"},
        {10'000'000.0, 9'999'994.0, true, "observation-acceptance-v1", "capture-2"},
        {11'000'000.0, 10'999'992.0, true, "observation-acceptance-v1", "capture-3"},
    };
    const auto fit = fitLinearModel(observations);
    CHECK(fit.succeeded());
    CHECK(fit.model.version == "frequency-error-model-v1");
    CHECK(near(fit.model.reference_frequency_hz, 10'000'000.0));
    CHECK(near(fit.model.intercept_error_hz, 6.0));
    CHECK(near(fit.model.slope_ppm, 2.0));
    CHECK(near(fit.model.residual_standard_uncertainty_hz, 0.0));
    CHECK(fit.model.residual_degrees_of_freedom == 1);
    CHECK(fit.model.residual_standard_uncertainty_available);

    const auto low = evaluateModel(fit.model, 9'000'000.0);
    const auto middle = evaluateModel(fit.model, 10'500'000.0);
    const auto high = evaluateModel(fit.model, 11'000'000.0);
    CHECK(low.succeeded() && middle.succeeded() && high.succeeded());
    CHECK(near(low.indicated_error_hz, 4.0));
    CHECK(near(middle.indicated_error_hz, 7.0));
    CHECK(near(high.indicated_error_hz, 8.0));
    CHECK(evaluateModel(fit.model, 8'999'999.0).status ==
          ModelEvaluationStatus::outside_validity_domain);
    CHECK(evaluateModel(fit.model, 11'000'001.0).status ==
          ModelEvaluationStatus::outside_validity_domain);

    const auto two_point = fitLinearModel({observations[0], observations[2]});
    CHECK(two_point.succeeded());
    CHECK(two_point.model.residual_degrees_of_freedom == 0);
    CHECK(!two_point.model.residual_standard_uncertainty_available);
}

void testModelFitFailsClosed() {
    CHECK(fitLinearModel({}).status == ModelFitStatus::invalid_input);
    CHECK(fitLinearModel({{10.0, 9.0, true, "v1", "a"}, {10.0, 8.0, true, "v1", "b"}}).status ==
          ModelFitStatus::insufficient_span);
    CHECK(fitLinearModel({{10.0, 9.0, true, "v1", "same"}, {20.0, 18.0, true, "v1", "same"}})
              .status == ModelFitStatus::invalid_input);
    ModelObservation non_finite;
    non_finite.indicated_frequency_hz = std::numeric_limits<double>::quiet_NaN();
    non_finite.estimated_true_frequency_hz = 1.0;
    CHECK(fitLocalConstantModel(non_finite).status == ModelFitStatus::invalid_input);

    FrequencyErrorModel invalid;
    invalid.observation_count = 1;
    CHECK(evaluateModel(invalid, 1.0).status == ModelEvaluationStatus::invalid_model);
}

void testIndependentAndCorrelatedUncertainty() {
    UncertaintyBudget budget;
    budget.components = {{"reference", 3.0, 1.0}, {"estimator", 4.0, 1.0}};
    auto result = calculateUncertainty(budget);
    CHECK(result.succeeded());
    CHECK(result.budget_version == "uncertainty-budget-v1");
    CHECK(near(result.combined_standard_uncertainty_hz, 5.0));
    CHECK(near(result.expanded_uncertainty_hz, 10.0));
    CHECK(near(result.coverage_factor, 2.0));

    budget.correlations = {{0, 1, 1.0}};
    result = calculateUncertainty(budget);
    CHECK(result.succeeded());
    CHECK(near(result.combined_standard_uncertainty_hz, 7.0));
    CHECK(near(result.expanded_uncertainty_hz, 14.0));
}

void testUncertaintyRejectsInvalidBudgets() {
    UncertaintyBudget budget;
    CHECK(calculateUncertainty(budget).status == UncertaintyStatus::invalid_input);

    budget.components = {{"one", 1.0, 1.0}, {"two", 1.0, 1.0}};
    budget.correlations = {{0, 2, 0.5}};
    CHECK(calculateUncertainty(budget).status == UncertaintyStatus::invalid_correlation);
    budget.correlations = {{0, 1, 1.1}};
    CHECK(calculateUncertainty(budget).status == UncertaintyStatus::invalid_correlation);
    budget.correlations = {{0, 1, 0.5}, {1, 0, 0.5}};
    CHECK(calculateUncertainty(budget).status == UncertaintyStatus::invalid_correlation);
    budget.correlations = {{0, 1, -1.0}};
    CHECK(calculateUncertainty(budget).succeeded());

    budget.components.push_back({"three", 1.0, 1.0});
    budget.correlations = {{0, 1, -0.9}, {0, 2, -0.9}, {1, 2, -0.9}};
    CHECK(calculateUncertainty(budget).status == UncertaintyStatus::invalid_correlation);

    budget.correlations.clear();
    budget.excluded_or_unknown_material_components = {"temperature"};
    const auto incomplete = calculateUncertainty(budget);
    CHECK(incomplete.succeeded());
    CHECK(!incomplete.material_components_complete);

    budget.excluded_or_unknown_material_components.clear();
    budget.components = {{"duplicate", 1.0, 1.0}, {"duplicate", 2.0, 1.0}};
    CHECK(calculateUncertainty(budget).status == UncertaintyStatus::invalid_input);
}

std::vector<AssuranceComponent> assuranceScores(int model_score = 94) {
    return {{"reference_provenance", 99}, {"received_signal_suitability", 97},
            {"device_binding", 96},       {"environmental_validity", 95},
            {"observation_quality", 93},  {"model_quality", model_score},
            {"artifact_integrity", 98},   {"evidence_completeness", 92}};
}

void testReliabilityWeakestComponentAndCeilings() {
    auto result =
        calculateReliabilityQuotient(ReferenceClass::authority_confirmed, assuranceScores());
    CHECK(result.succeeded());
    CHECK(result.scoring_version == "reliability-quotient-v1");
    CHECK(result.reference_ceiling == 100);
    CHECK(result.quotient == 92);

    result = calculateReliabilityQuotient(ReferenceClass::locally_characterized, assuranceScores());
    CHECK(result.succeeded());
    CHECK(result.reference_ceiling == 75);
    CHECK(result.quotient == 75);

    result = calculateReliabilityQuotient(ReferenceClass::unknown, assuranceScores());
    CHECK(result.succeeded());
    CHECK(result.quotient == 0);

    result = calculateReliabilityQuotient(ReferenceClass::authority_confirmed, assuranceScores(40));
    CHECK(result.quotient == 40);
}

void testReliabilityRejectsInvalidComponents() {
    CHECK(calculateReliabilityQuotient(ReferenceClass::authority_confirmed, {}).status ==
          ReliabilityStatus::invalid_input);
    CHECK(calculateReliabilityQuotient(ReferenceClass::authority_confirmed, {{"model", 101}})
              .status == ReliabilityStatus::invalid_input);
    CHECK(calculateReliabilityQuotient(ReferenceClass::authority_confirmed,
                                       {{"model", 90}, {"model", 80}})
              .status == ReliabilityStatus::invalid_input);
    CHECK(calculateReliabilityQuotient(ReferenceClass::authority_confirmed, assuranceScores(),
                                       "future-v2")
              .status == ReliabilityStatus::invalid_input);
    CHECK(calculateReliabilityQuotient(static_cast<ReferenceClass>(99), assuranceScores()).status ==
          ReliabilityStatus::invalid_input);
}

} // namespace

int main() {
    try {
        testLocalModelSignAndExactDomain();
        testLinearGoldenFitAndBoundaries();
        testModelFitFailsClosed();
        testIndependentAndCorrelatedUncertainty();
        testUncertaintyRejectsInvalidBudgets();
        testReliabilityWeakestComponentAndCeilings();
        testReliabilityRejectsInvalidComponents();
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
    std::cout << "model and uncertainty tests passed\n";
    return 0;
}
