#include "core/Uncertainty.h"

#include <algorithm>
#include <cmath>
#include <set>
#include <utility>

namespace sdrcal::core {

UncertaintyResult calculateUncertainty(const UncertaintyBudget& budget) {
    UncertaintyResult result;
    result.budget_version = budget.version;
    result.coverage_factor = budget.coverage_factor;
    result.material_components_complete = budget.excluded_or_unknown_material_components.empty();
    if (budget.version != "uncertainty-budget-v1" || budget.components.empty() ||
        !std::isfinite(budget.coverage_factor) || budget.coverage_factor <= 0.0) {
        result.reason = "budget version, components, and positive coverage factor are required";
        return result;
    }

    double variance = 0.0;
    std::set<std::string> component_names;
    for (const auto& component : budget.components) {
        if (component.name.empty() || !std::isfinite(component.standard_uncertainty_hz) ||
            component.standard_uncertainty_hz < 0.0 ||
            !std::isfinite(component.sensitivity_coefficient) ||
            !component_names.emplace(component.name).second) {
            result.reason =
                "uncertainty components require unique names and finite nonnegative values";
            return result;
        }
        const double contribution =
            component.standard_uncertainty_hz * component.sensitivity_coefficient;
        variance += contribution * contribution;
    }
    std::set<std::string> excluded_names;
    for (const auto& name : budget.excluded_or_unknown_material_components) {
        if (name.empty() || component_names.contains(name) ||
            !excluded_names.emplace(name).second) {
            result.reason =
                "excluded or unknown material components require unique unbudgeted names";
            return result;
        }
    }

    std::vector<std::vector<double>> correlation_matrix(
        budget.components.size(), std::vector<double>(budget.components.size(), 0.0));
    for (std::size_t index = 0; index < budget.components.size(); ++index) {
        correlation_matrix[index][index] = 1.0;
    }
    std::set<std::pair<std::size_t, std::size_t>> seen;
    for (const auto& correlation : budget.correlations) {
        const auto low = std::min(correlation.first_component, correlation.second_component);
        const auto high = std::max(correlation.first_component, correlation.second_component);
        if (high >= budget.components.size() || low == high ||
            !std::isfinite(correlation.coefficient) || std::abs(correlation.coefficient) > 1.0 ||
            !seen.emplace(low, high).second) {
            result.status = UncertaintyStatus::invalid_correlation;
            result.reason =
                "correlations require unique valid component pairs and coefficients in [-1, 1]";
            return result;
        }
        const auto& first = budget.components[low];
        const auto& second = budget.components[high];
        correlation_matrix[low][high] = correlation.coefficient;
        correlation_matrix[high][low] = correlation.coefficient;
        variance += 2.0 * correlation.coefficient * first.standard_uncertainty_hz *
                    first.sensitivity_coefficient * second.standard_uncertainty_hz *
                    second.sensitivity_coefficient;
    }
    // A valid correlation matrix must be positive semidefinite. This tolerant
    // Cholesky decomposition accepts exact zero pivots only when the remaining
    // column is also zero.
    std::vector<std::vector<double>> lower(budget.components.size(),
                                           std::vector<double>(budget.components.size(), 0.0));
    constexpr double tolerance = 1e-12;
    for (std::size_t row = 0; row < budget.components.size(); ++row) {
        for (std::size_t column = 0; column <= row; ++column) {
            double value = correlation_matrix[row][column];
            for (std::size_t prior = 0; prior < column; ++prior) {
                value -= lower[row][prior] * lower[column][prior];
            }
            if (row == column) {
                if (value < -tolerance) {
                    result.status = UncertaintyStatus::invalid_correlation;
                    result.reason = "declared coefficients do not form a valid correlation matrix";
                    return result;
                }
                lower[row][column] = std::sqrt(std::max(0.0, value));
            } else if (lower[column][column] > tolerance) {
                lower[row][column] = value / lower[column][column];
            } else if (std::abs(value) > tolerance) {
                result.status = UncertaintyStatus::invalid_correlation;
                result.reason = "declared coefficients do not form a valid correlation matrix";
                return result;
            }
        }
    }
    if (!std::isfinite(variance) || variance < 0.0) {
        result.status = UncertaintyStatus::invalid_correlation;
        result.reason = "declared correlations produce an invalid combined variance";
        return result;
    }

    result.combined_standard_uncertainty_hz = std::sqrt(variance);
    result.expanded_uncertainty_hz =
        result.combined_standard_uncertainty_hz * budget.coverage_factor;
    if (!std::isfinite(result.expanded_uncertainty_hz)) {
        result.reason = "expanded uncertainty is not finite";
        return result;
    }
    result.status = UncertaintyStatus::success;
    result.reason = "uncertainty budget combined";
    return result;
}

} // namespace sdrcal::core
