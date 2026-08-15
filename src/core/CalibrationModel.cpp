#include "core/CalibrationModel.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>

namespace sdrcal::core {
namespace {

bool validObservation(const ModelObservation& observation) {
    return std::isfinite(observation.indicated_frequency_hz) &&
           observation.indicated_frequency_hz > 0.0 &&
           std::isfinite(observation.estimated_true_frequency_hz) &&
           observation.estimated_true_frequency_hz > 0.0 && observation.accepted &&
           !observation.acceptance_policy_version.empty() && !observation.independence_id.empty();
}

bool validModel(const FrequencyErrorModel& model) {
    const bool type_invariants =
        (model.type == FrequencyModelType::local_constant && model.slope_ppm == 0.0 &&
         model.observation_count == 1 && model.minimum_frequency_hz == model.maximum_frequency_hz &&
         model.reference_frequency_hz == model.minimum_frequency_hz &&
         model.residual_degrees_of_freedom == 0 && !model.residual_standard_uncertainty_available &&
         model.residual_standard_uncertainty_hz == 0.0) ||
        (model.type == FrequencyModelType::linear && model.observation_count >= 2 &&
         model.minimum_frequency_hz < model.maximum_frequency_hz &&
         model.residual_degrees_of_freedom == model.observation_count - 2 &&
         model.residual_standard_uncertainty_available == (model.observation_count > 2) &&
         (model.residual_standard_uncertainty_available ||
          model.residual_standard_uncertainty_hz == 0.0));
    return model.version == "frequency-error-model-v1" &&
           std::isfinite(model.reference_frequency_hz) && model.reference_frequency_hz > 0.0 &&
           std::isfinite(model.intercept_error_hz) && std::isfinite(model.slope_ppm) &&
           std::isfinite(model.minimum_frequency_hz) && std::isfinite(model.maximum_frequency_hz) &&
           model.minimum_frequency_hz > 0.0 &&
           model.minimum_frequency_hz <= model.maximum_frequency_hz &&
           std::isfinite(model.residual_standard_uncertainty_hz) &&
           model.residual_standard_uncertainty_hz >= 0.0 && type_invariants;
}

} // namespace

ModelFitResult fitLocalConstantModel(const ModelObservation& observation) {
    ModelFitResult result;
    if (!validObservation(observation)) {
        result.reason = "observation frequencies must be finite and positive";
        return result;
    }

    result.status = ModelFitStatus::success;
    result.reason = "local constant model fitted";
    result.model.type = FrequencyModelType::local_constant;
    result.model.reference_frequency_hz = observation.indicated_frequency_hz;
    result.model.intercept_error_hz =
        observation.indicated_frequency_hz - observation.estimated_true_frequency_hz;
    result.model.minimum_frequency_hz = observation.indicated_frequency_hz;
    result.model.maximum_frequency_hz = observation.indicated_frequency_hz;
    result.model.observation_count = 1;
    return result;
}

ModelFitResult fitLinearModel(const std::vector<ModelObservation>& observations) {
    ModelFitResult result;
    if (observations.size() < 2 || !std::ranges::all_of(observations, [](const auto& value) {
            return validObservation(value);
        })) {
        result.reason = "a linear model requires at least two valid observations";
        return result;
    }
    std::vector<std::string> independence_ids;
    independence_ids.reserve(observations.size());
    for (const auto& observation : observations) {
        independence_ids.push_back(observation.independence_id);
    }
    std::ranges::sort(independence_ids);
    if (std::ranges::adjacent_find(independence_ids) != independence_ids.end()) {
        result.reason = "a linear model requires independently identified observations";
        return result;
    }

    const auto [minimum, maximum] =
        std::ranges::minmax_element(observations, {}, &ModelObservation::indicated_frequency_hz);
    if (minimum->indicated_frequency_hz == maximum->indicated_frequency_hz) {
        result.status = ModelFitStatus::insufficient_span;
        result.reason = "a linear model requires nonzero indicated-frequency span";
        return result;
    }

    const double reference =
        std::midpoint(minimum->indicated_frequency_hz, maximum->indicated_frequency_hz);
    double sum_x = 0.0;
    double sum_y = 0.0;
    for (const auto& observation : observations) {
        sum_x += (observation.indicated_frequency_hz - reference) / 1'000'000.0;
        sum_y += observation.indicated_frequency_hz - observation.estimated_true_frequency_hz;
    }
    const double count = static_cast<double>(observations.size());
    const double mean_x = sum_x / count;
    const double mean_y = sum_y / count;
    double sum_xx = 0.0;
    double sum_xy = 0.0;
    for (const auto& observation : observations) {
        const double x = (observation.indicated_frequency_hz - reference) / 1'000'000.0;
        const double y =
            observation.indicated_frequency_hz - observation.estimated_true_frequency_hz;
        sum_xx += (x - mean_x) * (x - mean_x);
        sum_xy += (x - mean_x) * (y - mean_y);
    }
    if (!(sum_xx > 0.0) || !std::isfinite(sum_xx) || !std::isfinite(sum_xy)) {
        result.status = ModelFitStatus::insufficient_span;
        result.reason = "indicated-frequency span is not numerically resolvable";
        return result;
    }

    const double slope = sum_xy / sum_xx;
    const double intercept = mean_y - slope * mean_x;
    double squared_residuals = 0.0;
    for (const auto& observation : observations) {
        const double x = (observation.indicated_frequency_hz - reference) / 1'000'000.0;
        const double residual =
            (observation.indicated_frequency_hz - observation.estimated_true_frequency_hz) -
            (intercept + slope * x);
        squared_residuals += residual * residual;
    }
    const double residual_uncertainty =
        observations.size() > 2 ? std::sqrt(squared_residuals / (count - 2.0)) : 0.0;
    if (!std::isfinite(slope) || !std::isfinite(intercept) ||
        !std::isfinite(residual_uncertainty)) {
        result.reason = "linear model calculation produced a non-finite result";
        return result;
    }

    result.status = ModelFitStatus::success;
    result.reason = "linear model fitted";
    result.model.type = FrequencyModelType::linear;
    result.model.reference_frequency_hz = reference;
    result.model.intercept_error_hz = intercept;
    result.model.slope_ppm = slope;
    result.model.minimum_frequency_hz = minimum->indicated_frequency_hz;
    result.model.maximum_frequency_hz = maximum->indicated_frequency_hz;
    result.model.residual_standard_uncertainty_hz = residual_uncertainty;
    result.model.residual_degrees_of_freedom = observations.size() - 2;
    result.model.residual_standard_uncertainty_available = observations.size() > 2;
    result.model.observation_count = observations.size();
    return result;
}

ModelEvaluationResult evaluateModel(const FrequencyErrorModel& model,
                                    double indicated_frequency_hz) {
    ModelEvaluationResult result;
    if (!validModel(model) || !std::isfinite(indicated_frequency_hz) ||
        indicated_frequency_hz <= 0.0) {
        result.reason = "model and evaluation frequency must be valid";
        return result;
    }
    if (indicated_frequency_hz < model.minimum_frequency_hz ||
        indicated_frequency_hz > model.maximum_frequency_hz) {
        result.status = ModelEvaluationStatus::outside_validity_domain;
        result.reason = "extrapolation outside the observation-bounded domain is prohibited";
        return result;
    }

    const double delta_mhz = (indicated_frequency_hz - model.reference_frequency_hz) / 1'000'000.0;
    result.indicated_error_hz = model.intercept_error_hz + model.slope_ppm * delta_mhz;
    result.estimated_true_frequency_hz = indicated_frequency_hz - result.indicated_error_hz;
    if (!std::isfinite(result.indicated_error_hz) ||
        !std::isfinite(result.estimated_true_frequency_hz)) {
        result.reason = "model evaluation produced a non-finite result";
        return result;
    }
    result.status = ModelEvaluationStatus::success;
    result.reason = "model evaluated within its validity domain";
    return result;
}

} // namespace sdrcal::core
