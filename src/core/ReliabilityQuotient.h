#pragma once

#include <string>
#include <vector>

namespace sdrcal::core {

enum class ReferenceClass {
    authority_confirmed,
    derived_traceable,
    locally_characterized,
    ad_hoc,
    unknown,
};
enum class ReliabilityStatus { success, invalid_input };

struct AssuranceComponent {
    std::string name;
    int score = 0;
};

struct ReliabilityResult {
    ReliabilityStatus status = ReliabilityStatus::invalid_input;
    std::string reason;
    std::string scoring_version;
    int reference_ceiling = 0;
    int quotient = 0;

    [[nodiscard]] bool succeeded() const noexcept {
        return status == ReliabilityStatus::success;
    }
};

[[nodiscard]] int referenceAssuranceCeiling(ReferenceClass reference_class) noexcept;
[[nodiscard]] ReliabilityResult
calculateReliabilityQuotient(ReferenceClass reference_class,
                             const std::vector<AssuranceComponent>& components,
                             std::string scoring_version = "reliability-quotient-v1");

} // namespace sdrcal::core
