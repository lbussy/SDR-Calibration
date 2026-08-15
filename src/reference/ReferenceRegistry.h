#pragma once

#include "core/ReliabilityQuotient.h"
#include "profile/ProfileStorage.h"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace sdrcal::reference {

enum class OperatingStatus { active, intermittent, inactive, unknown };

struct EvidenceSource {
    std::string source_id;
    std::string description;
    std::string retrieved_at;
    std::optional<std::string> expires_at;
    std::string sha256;
};

struct ReferenceEntry {
    std::string reference_id;
    core::ReferenceClass reference_class = core::ReferenceClass::unknown;
    int assurance_ceiling = 0;
    double nominal_frequency_hz = 0.0;
    std::optional<double> frequency_uncertainty_hz;
    OperatingStatus operating_status = OperatingStatus::unknown;
    std::string location_or_connection;
    std::vector<std::string> conditions;
    std::vector<std::string> limitations;
    std::vector<EvidenceSource> evidence;
};

struct RegistryIntegrity {
    std::string canonicalization = "RFC8785";
    std::string sha256;
    std::optional<profile::SignatureRecord> signature;
};

struct ReferenceRegistry {
    std::string schema_name = "sdr-reference-registry";
    std::string schema_version = "1.0.0";
    std::string registry_id;
    std::string registry_version;
    std::string generated_at;
    std::optional<std::string> expires_at;
    std::string provenance;
    std::vector<ReferenceEntry> references;
    RegistryIntegrity integrity;
};

struct RegistryIssue {
    std::string path;
    std::string message;
};
struct RegistryValidation {
    std::vector<RegistryIssue> issues;
    [[nodiscard]] bool valid() const noexcept {
        return issues.empty();
    }
};

[[nodiscard]] RegistryValidation validateRegistry(const ReferenceRegistry& registry,
                                                  bool verify_digest = true);
[[nodiscard]] std::string serializeRegistry(const ReferenceRegistry& registry,
                                            bool include_integrity = true);
[[nodiscard]] bool refreshRegistryIntegrity(ReferenceRegistry& registry,
                                            std::string* error = nullptr);
[[nodiscard]] bool verifyRegistryIntegrity(const ReferenceRegistry& registry,
                                           const profile::VerifyHook& verifier,
                                           bool require_signature, std::string* error = nullptr);
[[nodiscard]] profile::ReplacementResult
activateRegistryAtomically(const std::filesystem::path& destination,
                           const ReferenceRegistry& registry,
                           const profile::VerifyHook& verifier = {}, bool require_signature = true);

enum class ResolutionStatus { usable, reduced_assurance, expired, missing, conflict, rejected };
struct ResolutionResult {
    ResolutionStatus status = ResolutionStatus::missing;
    std::string reason;
    std::optional<ReferenceEntry> reference;
    bool local_overlay_selected = false;
    bool shadows_global = false;
    int effective_assurance_ceiling = 0;
    [[nodiscard]] bool usable() const noexcept {
        return status == ResolutionStatus::usable || status == ResolutionStatus::reduced_assurance;
    }
};
[[nodiscard]] ResolutionResult resolveReference(const ReferenceRegistry& global_registry,
                                                const std::vector<ReferenceEntry>& local_overlay,
                                                std::string_view reference_id,
                                                std::string_view evaluated_at);

} // namespace sdrcal::reference
