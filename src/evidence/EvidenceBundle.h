#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace sdrcal::evidence {

enum class ArtifactClass { reduced_evidence, raw_iq, expanded_diagnostic };
struct ArtifactRecord {
    std::string artifact_id;
    ArtifactClass artifact_class = ArtifactClass::reduced_evidence;
    std::string media_type;
    std::string sha256;
    std::uint64_t byte_count = 0;
    std::optional<std::int64_t> duration_ms;
    bool truncated = false;
};
struct MetadataField {
    std::string name;
    std::string value;
};
struct BundleRequest {
    std::string bundle_id;
    std::string created_at;
    std::string source_identity;
    std::int64_t monotonic_duration_ms = 0;
    std::uint64_t maximum_bundle_bytes = 0;
    std::int64_t maximum_duration_ms = 0;
    bool allow_raw_iq = false;
    std::uint64_t maximum_raw_iq_bytes = 0;
    std::int64_t maximum_raw_iq_duration_ms = 0;
    bool allow_expanded_diagnostics = false;
    std::vector<MetadataField> metadata;
    std::vector<ArtifactRecord> artifacts;
    std::vector<std::string> partial_failures;
    bool atomic_write_completed = false;
};
struct BundleIssue {
    std::string path;
    std::string message;
};
struct BundleResult {
    std::vector<BundleIssue> issues;
    std::string manifest_json;
    [[nodiscard]] bool valid() const noexcept {
        return issues.empty();
    }
};
[[nodiscard]] BundleResult buildEvidenceManifest(const BundleRequest& request);

} // namespace sdrcal::evidence
