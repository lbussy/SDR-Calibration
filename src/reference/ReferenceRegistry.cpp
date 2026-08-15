#include "reference/ReferenceRegistry.h"

#include "profile/CanonicalJson.h"
#include "profile/Sha256.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <fstream>
#include <set>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

namespace sdrcal::reference {
namespace {
using profile::JsonArray;
using profile::JsonObject;
using profile::JsonValue;

bool timestamp(const std::string& value) {
    return value.size() >= 20U && value[4] == '-' && value[7] == '-' && value[10] == 'T' &&
           value.back() == 'Z';
}
const char* className(core::ReferenceClass value) {
    switch (value) {
    case core::ReferenceClass::authority_confirmed:
        return "authority_confirmed";
    case core::ReferenceClass::derived_traceable:
        return "derived_traceable";
    case core::ReferenceClass::locally_characterized:
        return "locally_characterized";
    case core::ReferenceClass::ad_hoc:
        return "ad_hoc";
    case core::ReferenceClass::unknown:
        return "unknown";
    }
    return "unknown";
}
const char* statusName(OperatingStatus value) {
    switch (value) {
    case OperatingStatus::active:
        return "active";
    case OperatingStatus::intermittent:
        return "intermittent";
    case OperatingStatus::inactive:
        return "inactive";
    case OperatingStatus::unknown:
        return "unknown";
    }
    return "unknown";
}
JsonArray strings(const std::vector<std::string>& values) {
    JsonArray result;
    for (const auto& value : values)
        result.emplace_back(value);
    return result;
}
JsonObject object(const ReferenceRegistry& registry, bool include_integrity) {
    JsonArray references;
    for (const auto& item : registry.references) {
        JsonArray evidence;
        for (const auto& source : item.evidence) {
            JsonObject e{{"description", source.description},
                         {"retrieved_at", source.retrieved_at},
                         {"sha256", source.sha256},
                         {"source_id", source.source_id}};
            if (source.expires_at)
                e["expires_at"] = *source.expires_at;
            evidence.emplace_back(e);
        }
        JsonObject entry{{"assurance_ceiling", item.assurance_ceiling},
                         {"conditions", strings(item.conditions)},
                         {"evidence", evidence},
                         {"limitations", strings(item.limitations)},
                         {"location_or_connection", item.location_or_connection},
                         {"nominal_frequency_hz", item.nominal_frequency_hz},
                         {"operating_status", statusName(item.operating_status)},
                         {"reference_class", className(item.reference_class)},
                         {"reference_id", item.reference_id}};
        if (item.frequency_uncertainty_hz)
            entry["frequency_uncertainty_hz"] = *item.frequency_uncertainty_hz;
        references.emplace_back(entry);
    }
    JsonObject root{{"generated_at", registry.generated_at},
                    {"provenance", registry.provenance},
                    {"references", references},
                    {"registry_id", registry.registry_id},
                    {"registry_version", registry.registry_version},
                    {"schema_name", registry.schema_name},
                    {"schema_version", registry.schema_version}};
    if (registry.expires_at)
        root["expires_at"] = *registry.expires_at;
    if (include_integrity) {
        JsonObject integrity{{"canonicalization", registry.integrity.canonicalization},
                             {"sha256", registry.integrity.sha256}};
        if (registry.integrity.signature)
            integrity["signature"] =
                JsonObject{{"algorithm", registry.integrity.signature->algorithm},
                           {"key_id", registry.integrity.signature->key_id},
                           {"value", registry.integrity.signature->value}};
        root["integrity"] = integrity;
    }
    return root;
}
void issue(RegistryValidation& result, std::string path, std::string message) {
    result.issues.push_back({std::move(path), std::move(message)});
}
bool syncFile(const std::filesystem::path& path) {
#ifdef _WIN32
    const int descriptor = _open(path.string().c_str(), _O_WRONLY | _O_BINARY);
    if (descriptor < 0)
        return false;
    const bool ok = _commit(descriptor) == 0;
    _close(descriptor);
    return ok;
#else
    const int descriptor = ::open(path.c_str(), O_RDONLY);
    if (descriptor < 0)
        return false;
    const bool ok = ::fsync(descriptor) == 0;
    ::close(descriptor);
    return ok;
#endif
}
} // namespace

std::string serializeRegistry(const ReferenceRegistry& registry, bool include_integrity) {
    const auto result = profile::canonicalizeJson(object(registry, include_integrity));
    return result.success ? result.value : std::string{};
}

RegistryValidation validateRegistry(const ReferenceRegistry& registry, bool verify_digest) {
    RegistryValidation result;
    if (registry.schema_name != "sdr-reference-registry" || registry.schema_version != "1.0.0")
        issue(result, "schema", "unsupported registry schema");
    if (registry.registry_id.empty() || registry.registry_version.empty())
        issue(result, "registry", "registry identity and version are required");
    if (!timestamp(registry.generated_at) || !registry.expires_at ||
        !timestamp(*registry.expires_at))
        issue(result, "validity",
              "generated and expiry UTC timestamps are required and must be valid");
    if (registry.expires_at && *registry.expires_at <= registry.generated_at)
        issue(result, "expires_at", "expiry must follow generation");
    if (registry.provenance.empty())
        issue(result, "provenance", "provenance is required");
    if (registry.references.empty())
        issue(result, "references", "at least one reference is required");
    std::set<std::string> ids;
    for (std::size_t i = 0; i < registry.references.size(); ++i) {
        const auto& item = registry.references[i];
        const auto base = "references[" + std::to_string(i) + "]";
        if (item.reference_id.empty() || !ids.insert(item.reference_id).second)
            issue(result, base + ".reference_id", "reference ID is empty or duplicated");
        if (!std::isfinite(item.nominal_frequency_hz) || item.nominal_frequency_hz <= 0.0)
            issue(result, base + ".nominal_frequency_hz", "frequency must be finite and positive");
        if (item.frequency_uncertainty_hz && (!std::isfinite(*item.frequency_uncertainty_hz) ||
                                              *item.frequency_uncertainty_hz < 0.0))
            issue(result, base + ".frequency_uncertainty_hz",
                  "uncertainty must be finite and nonnegative");
        if (item.assurance_ceiling != core::referenceAssuranceCeiling(item.reference_class))
            issue(result, base + ".assurance_ceiling", "class-to-ceiling mapping mismatch");
        if (item.location_or_connection.empty() || item.conditions.empty() ||
            item.limitations.empty() || item.evidence.empty())
            issue(result, base, "location, conditions, limitations, and evidence are required");
        std::set<std::string> evidence_ids;
        for (const auto& source : item.evidence) {
            if (source.source_id.empty() || !evidence_ids.insert(source.source_id).second ||
                source.description.empty() || !timestamp(source.retrieved_at) ||
                !profile::isSha256Hex(source.sha256))
                issue(result, base + ".evidence",
                      "evidence identity, description, retrieval time, or digest is invalid");
            if (!source.expires_at || !timestamp(*source.expires_at) ||
                *source.expires_at <= source.retrieved_at)
                issue(result, base + ".evidence.expires_at",
                      "evidence expiry is required and must follow retrieval");
        }
    }
    if (registry.integrity.canonicalization != "RFC8785" ||
        !profile::isSha256Hex(registry.integrity.sha256))
        issue(result, "integrity", "integrity metadata is invalid");
    if (verify_digest && profile::isSha256Hex(registry.integrity.sha256) &&
        profile::sha256Hex(serializeRegistry(registry, false)) != registry.integrity.sha256)
        issue(result, "integrity.sha256", "registry digest mismatch");
    return result;
}

bool refreshRegistryIntegrity(ReferenceRegistry& registry, std::string* error) {
    registry.integrity.signature.reset();
    const auto payload = serializeRegistry(registry, false);
    if (payload.empty()) {
        if (error)
            *error = "registry serialization failed";
        return false;
    }
    registry.integrity.sha256 = profile::sha256Hex(payload);
    return true;
}

bool verifyRegistryIntegrity(const ReferenceRegistry& registry, const profile::VerifyHook& verifier,
                             bool require_signature, std::string* error) {
    if (!validateRegistry(registry).valid()) {
        if (error)
            *error = "registry semantic or digest validation failed";
        return false;
    }
    if (!registry.integrity.signature) {
        if (require_signature) {
            if (error)
                *error = "registry signature required";
            return false;
        }
        return true;
    }
    const auto& signature = *registry.integrity.signature;
    if (signature.algorithm != "ed25519" || signature.key_id.empty() || signature.value.empty() ||
        !verifier ||
        !verifier(serializeRegistry(registry, false), signature.key_id, signature.value)) {
        if (error)
            *error = "registry signature verification failed";
        return false;
    }
    return true;
}

profile::ReplacementResult activateRegistryAtomically(const std::filesystem::path& destination,
                                                      const ReferenceRegistry& registry,
                                                      const profile::VerifyHook& verifier,
                                                      bool require_signature) {
    std::string error;
    if (destination.empty() ||
        !verifyRegistryIntegrity(registry, verifier, require_signature, &error))
        return {profile::ReplacementStatus::validation_failed, error};
    const auto payload = serializeRegistry(registry, true);
    static std::atomic<unsigned long long> sequence{0};
    const auto temporary =
        destination.parent_path() / (destination.filename().string() + ".tmp-registry-" +
                                     std::to_string(sequence.fetch_add(1)));
    std::error_code ec;
    if (std::filesystem::exists(temporary, ec))
        return {profile::ReplacementStatus::write_failed, "temporary registry path exists"};
    {
        std::ofstream out(temporary, std::ios::binary | std::ios::trunc);
        if (!out || !(out << payload << '\n') || !out.flush()) {
            out.close();
            std::filesystem::remove(temporary, ec);
            return {profile::ReplacementStatus::write_failed, "temporary registry write failed"};
        }
    }
    if (!syncFile(temporary)) {
        std::filesystem::remove(temporary, ec);
        return {profile::ReplacementStatus::sync_failed,
                "temporary registry synchronization failed"};
    }
#ifdef _WIN32
    const bool replaced =
        std::filesystem::exists(destination)
            ? ReplaceFileW(destination.c_str(), temporary.c_str(), nullptr,
                           REPLACEFILE_WRITE_THROUGH, nullptr, nullptr) != 0
            : MoveFileExW(temporary.c_str(), destination.c_str(), MOVEFILE_WRITE_THROUGH) != 0;
    if (!replaced) {
#else
    std::filesystem::rename(temporary, destination, ec);
    if (ec) {
#endif
        std::filesystem::remove(temporary, ec);
        return {profile::ReplacementStatus::replace_failed, "atomic registry rename failed"};
    }
    return {profile::ReplacementStatus::success, "registry atomically activated"};
}

ResolutionResult resolveReference(const ReferenceRegistry& global_registry,
                                  const std::vector<ReferenceEntry>& local_overlay,
                                  std::string_view reference_id, std::string_view evaluated_at) {
    const auto failure = [](ResolutionStatus status, std::string reason) {
        ResolutionResult result;
        result.status = status;
        result.reason = std::move(reason);
        return result;
    };
    if (!validateRegistry(global_registry).valid() || !timestamp(std::string(evaluated_at)))
        return failure(ResolutionStatus::rejected, "registry or evaluation time is invalid");
    if (global_registry.expires_at && evaluated_at > *global_registry.expires_at)
        return failure(ResolutionStatus::expired, "cached registry is expired");
    std::set<std::string> local_ids;
    for (const auto& item : local_overlay)
        if (item.reference_id.empty() || !local_ids.insert(item.reference_id).second)
            return failure(ResolutionStatus::conflict,
                           "local overlay contains duplicate or empty IDs");
    const auto global =
        std::find_if(global_registry.references.begin(), global_registry.references.end(),
                     [&](const auto& item) { return item.reference_id == reference_id; });
    const auto local =
        std::find_if(local_overlay.begin(), local_overlay.end(),
                     [&](const auto& item) { return item.reference_id == reference_id; });
    const ReferenceEntry* selected =
        local != local_overlay.end()
            ? &*local
            : (global != global_registry.references.end() ? &*global : nullptr);
    if (!selected)
        return failure(ResolutionStatus::missing, "reference is absent");
    ReferenceRegistry probe = global_registry;
    probe.references = {*selected};
    if (!refreshRegistryIntegrity(probe) || !validateRegistry(probe).valid() ||
        selected->operating_status == OperatingStatus::inactive ||
        selected->operating_status == OperatingStatus::unknown)
        return failure(ResolutionStatus::rejected,
                       "reference identity, evidence, or operating status is unsuitable");
    for (const auto& evidence : selected->evidence)
        if (evidence.expires_at && evaluated_at > *evidence.expires_at)
            return failure(ResolutionStatus::expired, "required reference evidence is expired");
    const bool reduced = selected->reference_class != core::ReferenceClass::authority_confirmed ||
                         selected->operating_status != OperatingStatus::active;
    return {reduced ? ResolutionStatus::reduced_assurance : ResolutionStatus::usable,
            reduced ? "reference usable within its assurance ceiling" : "reference usable",
            *selected,
            local != local_overlay.end(),
            local != local_overlay.end() && global != global_registry.references.end(),
            selected->assurance_ceiling};
}
} // namespace sdrcal::reference
