#include "core/ReliabilityQuotient.h"

#include <algorithm>
#include <set>

namespace sdrcal::core {

int referenceAssuranceCeiling(ReferenceClass reference_class) noexcept {
    switch (reference_class) {
    case ReferenceClass::authority_confirmed:
        return 100;
    case ReferenceClass::derived_traceable:
        return 90;
    case ReferenceClass::locally_characterized:
        return 75;
    case ReferenceClass::ad_hoc:
        return 50;
    case ReferenceClass::unknown:
        return 0;
    }
    return 0;
}

ReliabilityResult calculateReliabilityQuotient(ReferenceClass reference_class,
                                               const std::vector<AssuranceComponent>& components,
                                               std::string scoring_version) {
    ReliabilityResult result;
    result.scoring_version = std::move(scoring_version);
    result.reference_ceiling = referenceAssuranceCeiling(reference_class);
    const bool valid_reference_class = reference_class == ReferenceClass::authority_confirmed ||
                                       reference_class == ReferenceClass::derived_traceable ||
                                       reference_class == ReferenceClass::locally_characterized ||
                                       reference_class == ReferenceClass::ad_hoc ||
                                       reference_class == ReferenceClass::unknown;
    if (!valid_reference_class || result.scoring_version != "reliability-quotient-v1" ||
        components.empty()) {
        result.reason = "the supported scoring version and all assurance components are required";
        return result;
    }

    const std::set<std::string> required_names = {
        "reference_provenance",   "received_signal_suitability", "device_binding",
        "environmental_validity", "observation_quality",         "model_quality",
        "artifact_integrity",     "evidence_completeness",
    };
    std::set<std::string> names;
    int quotient = result.reference_ceiling;
    for (const auto& component : components) {
        if (component.name.empty() || component.score < 0 || component.score > 100 ||
            !names.emplace(component.name).second) {
            result.reason =
                "assurance components require unique names and scores from 0 through 100";
            return result;
        }
        quotient = std::min(quotient, component.score);
    }
    if (names != required_names) {
        result.reason = "reliability-quotient-v1 requires exactly its eight named components";
        return result;
    }
    result.status = ReliabilityStatus::success;
    result.reason = "reliability quotient calculated as the weakest applicable assurance score";
    result.quotient = quotient;
    return result;
}

} // namespace sdrcal::core
