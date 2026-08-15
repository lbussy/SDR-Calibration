#pragma once

#include <string>
#include <vector>

namespace sdrcal::core {

enum class FrequencyModelType { local_constant, linear };
enum class ModelFitStatus { success, invalid_input, insufficient_span };
enum class ModelEvaluationStatus { success, invalid_model, outside_validity_domain };

struct ModelObservation {
    double indicated_frequency_hz = 0.0;
    double estimated_true_frequency_hz = 0.0;
    bool accepted = false;
    std::string acceptance_policy_version;
    std::string independence_id;
};

struct FrequencyErrorModel {
    std::string version = "frequency-error-model-v1";
    FrequencyModelType type = FrequencyModelType::local_constant;
    double reference_frequency_hz = 0.0;
    double intercept_error_hz = 0.0;
    double slope_ppm = 0.0;
    double minimum_frequency_hz = 0.0;
    double maximum_frequency_hz = 0.0;
    double residual_standard_uncertainty_hz = 0.0;
    std::size_t residual_degrees_of_freedom = 0;
    bool residual_standard_uncertainty_available = false;
    std::size_t observation_count = 0;
};

struct ModelFitResult {
    ModelFitStatus status = ModelFitStatus::invalid_input;
    std::string reason;
    FrequencyErrorModel model;

    [[nodiscard]] bool succeeded() const noexcept {
        return status == ModelFitStatus::success;
    }
};

struct ModelEvaluationResult {
    ModelEvaluationStatus status = ModelEvaluationStatus::invalid_model;
    std::string reason;
    double indicated_error_hz = 0.0;
    double estimated_true_frequency_hz = 0.0;

    [[nodiscard]] bool succeeded() const noexcept {
        return status == ModelEvaluationStatus::success;
    }
};

[[nodiscard]] ModelFitResult fitLocalConstantModel(const ModelObservation& observation);
[[nodiscard]] ModelFitResult fitLinearModel(const std::vector<ModelObservation>& observations);
[[nodiscard]] ModelEvaluationResult evaluateModel(const FrequencyErrorModel& model,
                                                  double indicated_frequency_hz);

} // namespace sdrcal::core
