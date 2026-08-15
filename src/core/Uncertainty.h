#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace sdrcal::core {

enum class UncertaintyStatus { success, invalid_input, invalid_correlation };

struct StandardUncertaintyComponent {
    std::string name;
    double standard_uncertainty_hz = 0.0;
    double sensitivity_coefficient = 1.0;
};

struct UncertaintyCorrelation {
    std::size_t first_component = 0;
    std::size_t second_component = 0;
    double coefficient = 0.0;
};

struct UncertaintyBudget {
    std::string version = "uncertainty-budget-v1";
    std::vector<StandardUncertaintyComponent> components;
    std::vector<UncertaintyCorrelation> correlations;
    std::vector<std::string> excluded_or_unknown_material_components;
    double coverage_factor = 2.0;
};

struct UncertaintyResult {
    UncertaintyStatus status = UncertaintyStatus::invalid_input;
    std::string reason;
    std::string budget_version;
    double combined_standard_uncertainty_hz = 0.0;
    double expanded_uncertainty_hz = 0.0;
    double coverage_factor = 0.0;
    bool material_components_complete = false;

    [[nodiscard]] bool succeeded() const noexcept {
        return status == UncertaintyStatus::success;
    }
};

[[nodiscard]] UncertaintyResult calculateUncertainty(const UncertaintyBudget& budget);

} // namespace sdrcal::core
