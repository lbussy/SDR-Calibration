#include "evidence/EvidenceBundle.h"

#include "profile/CanonicalJson.h"
#include "profile/Sha256.h"

#include <algorithm>
#include <cctype>
#include <limits>
#include <set>

namespace sdrcal::evidence {
namespace {
bool timestamp(const std::string& value) {
    return value.size() >= 20U && value[4] == '-' && value[10] == 'T' && value.back() == 'Z';
}
std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}
bool forbidden(const std::string& name) {
    const auto value = lower(name);
    for (const auto* term : {"password", "passwd", "credential", "secret", "token", "private_key",
                             "private-key", "api_key", "apikey"})
        if (value.find(term) != std::string::npos)
            return true;
    return false;
}
bool unsafePath(const std::string& name, const std::string& value) {
    const auto key = lower(name);
    const bool path_like =
        key.find("path") != std::string::npos || key.find("location") != std::string::npos;
    if (!path_like)
        return false;
    return value.starts_with('/') || value.starts_with('\\') ||
           (value.size() > 2U && std::isalpha(static_cast<unsigned char>(value[0])) &&
            value[1] == ':') ||
           value.find("../") != std::string::npos || value.find("..\\") != std::string::npos;
}
const char* className(ArtifactClass value) {
    switch (value) {
    case ArtifactClass::reduced_evidence:
        return "reduced_evidence";
    case ArtifactClass::raw_iq:
        return "raw_iq";
    case ArtifactClass::expanded_diagnostic:
        return "expanded_diagnostic";
    }
    return "reduced_evidence";
}
} // namespace

BundleResult buildEvidenceManifest(const BundleRequest& request) {
    BundleResult result;
    auto issue = [&](std::string path, std::string message) {
        result.issues.push_back({std::move(path), std::move(message)});
    };
    if (request.bundle_id.empty() || request.source_identity.empty())
        issue("identity", "bundle and source identity are required");
    if (!timestamp(request.created_at))
        issue("created_at", "UTC creation timestamp is invalid");
    if (request.monotonic_duration_ms < 0 || request.maximum_duration_ms <= 0 ||
        request.monotonic_duration_ms > request.maximum_duration_ms)
        issue("monotonic_duration_ms", "duration is negative, unbounded, or exceeds its bound");
    if (request.maximum_bundle_bytes == 0)
        issue("maximum_bundle_bytes", "bundle byte bound is required");
    if (request.maximum_bundle_bytes >
        static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()))
        issue("maximum_bundle_bytes", "bundle byte bound exceeds JSON integer range");
    std::uint64_t total = 0;
    std::set<std::string> ids;
    profile::JsonArray artifacts;
    for (std::size_t i = 0; i < request.artifacts.size(); ++i) {
        const auto& item = request.artifacts[i];
        const auto base = "artifacts[" + std::to_string(i) + "]";
        if (item.artifact_id.empty() || !ids.insert(item.artifact_id).second)
            issue(base + ".artifact_id", "artifact ID is empty or duplicated");
        if (item.media_type.empty() || !profile::isSha256Hex(item.sha256))
            issue(base, "media type or digest is invalid");
        if (item.byte_count > static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()))
            issue(base + ".byte_count", "artifact byte count exceeds JSON integer range");
        if (item.byte_count >
            request.maximum_bundle_bytes - std::min(total, request.maximum_bundle_bytes))
            issue(base + ".byte_count", "bundle byte bound exceeded");
        else
            total += item.byte_count;
        if (item.duration_ms && *item.duration_ms < 0)
            issue(base + ".duration_ms", "artifact duration is negative");
        if (item.artifact_class == ArtifactClass::raw_iq &&
            (!request.allow_raw_iq || request.maximum_raw_iq_bytes == 0 ||
             request.maximum_raw_iq_duration_ms <= 0 ||
             item.byte_count > request.maximum_raw_iq_bytes || !item.duration_ms ||
             *item.duration_ms > request.maximum_raw_iq_duration_ms))
            issue(base, "raw IQ is not explicitly allowed and bounded");
        if (item.artifact_class == ArtifactClass::expanded_diagnostic &&
            !request.allow_expanded_diagnostics)
            issue(base, "expanded diagnostics are not opted in");
        profile::JsonObject object{{"artifact_class", className(item.artifact_class)},
                                   {"artifact_id", item.artifact_id},
                                   {"byte_count", static_cast<std::int64_t>(item.byte_count)},
                                   {"media_type", item.media_type},
                                   {"sha256", item.sha256},
                                   {"truncated", item.truncated}};
        if (item.duration_ms)
            object["duration_ms"] = *item.duration_ms;
        artifacts.emplace_back(object);
    }
    profile::JsonObject metadata;
    for (const auto& item : request.metadata) {
        if (item.name.empty() || forbidden(item.name) || forbidden(item.value))
            issue("metadata." + item.name, "metadata may contain secret material");
        else if (unsafePath(item.name, item.value))
            issue("metadata." + item.name, "shareable metadata contains an unsafe path");
        else if (!metadata.emplace(item.name, item.value).second)
            issue("metadata." + item.name, "metadata name is duplicated");
    }
    profile::JsonArray failures;
    for (const auto& failure : request.partial_failures)
        failures.emplace_back(failure);
    if (!result.valid())
        return result;
    profile::JsonObject root{
        {"artifact_count", static_cast<std::int64_t>(request.artifacts.size())},
        {"artifacts", artifacts},
        {"atomic_write_completed", request.atomic_write_completed},
        {"bundle_id", request.bundle_id},
        {"created_at", request.created_at},
        {"maximum_bundle_bytes", static_cast<std::int64_t>(request.maximum_bundle_bytes)},
        {"maximum_duration_ms", request.maximum_duration_ms},
        {"metadata", metadata},
        {"monotonic_duration_ms", request.monotonic_duration_ms},
        {"partial_failures", failures},
        {"schema_name", "sdr-evidence-bundle"},
        {"schema_version", "1.0.0"},
        {"source_identity", request.source_identity},
        {"total_bytes", static_cast<std::int64_t>(total)}};
    const auto canonical = profile::canonicalizeJson(root);
    if (!canonical.success)
        issue("manifest", canonical.error);
    else
        result.manifest_json = canonical.value;
    return result;
}
} // namespace sdrcal::evidence
