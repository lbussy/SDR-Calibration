#include "evidence/EvidenceBundle.h"
#include "profile/Sha256.h"
#include "reference/ReferenceRegistry.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>

namespace {
int failures = 0;
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            std::cerr << __FILE__ << ':' << __LINE__ << " CHECK(" #x ") failed\n";                 \
            ++failures;                                                                            \
        }                                                                                          \
    } while (false)

sdrcal::reference::ReferenceEntry entry(std::string id = "wwv-10") {
    using namespace sdrcal;
    return {std::move(id),
            core::ReferenceClass::authority_confirmed,
            100,
            10000000.0,
            0.001,
            reference::OperatingStatus::active,
            "radio reception",
            {"night path reviewed"},
            {"propagation uncertainty applies"},
            {{"nist-page", "current NIST schedule", "2026-08-01T00:00:00Z", "2026-09-01T00:00:00Z",
              std::string(64, 'a')}}};
}
sdrcal::reference::ReferenceRegistry registry() {
    sdrcal::reference::ReferenceRegistry value;
    value.registry_id = "global";
    value.registry_version = "2026.08";
    value.generated_at = "2026-08-01T00:00:00Z";
    value.expires_at = "2026-09-01T00:00:00Z";
    value.provenance = "signed project registry";
    value.references = {entry()};
    CHECK(sdrcal::reference::refreshRegistryIntegrity(value));
    return value;
}

void registryTests() {
    using namespace sdrcal;
    auto value = registry();
    CHECK(reference::validateRegistry(value).valid());
    value.references[0].assurance_ceiling = 99;
    CHECK(!reference::validateRegistry(value, false).valid());
    value = registry();
    value.references[0].nominal_frequency_hz += 1.0;
    CHECK(!reference::validateRegistry(value).valid());
    value = registry();
    CHECK(!reference::verifyRegistryIntegrity(value, {}, true));
    value.integrity.signature = profile::SignatureRecord{"ed25519", "test-key", "signature"};
    CHECK(reference::verifyRegistryIntegrity(
        value,
        [](auto payload, auto key, auto signature) {
            return !payload.empty() && key == "test-key" && signature == "signature";
        },
        true));
    auto local = entry("local-gpsdo");
    local.reference_class = core::ReferenceClass::locally_characterized;
    local.assurance_ceiling = 75;
    auto resolved =
        reference::resolveReference(value, {local}, "local-gpsdo", "2026-08-15T00:00:00Z");
    CHECK(resolved.usable());
    CHECK(resolved.local_overlay_selected);
    CHECK(resolved.effective_assurance_ceiling == 75);
    local.reference_id = "wwv-10";
    resolved = reference::resolveReference(value, {local}, "wwv-10", "2026-08-15T00:00:00Z");
    CHECK(resolved.shadows_global);
    resolved = reference::resolveReference(value, {local, local}, "wwv-10", "2026-08-15T00:00:00Z");
    CHECK(resolved.status == reference::ResolutionStatus::conflict);
    CHECK(reference::resolveReference(value, {}, "missing", "2026-08-15T00:00:00Z").status ==
          reference::ResolutionStatus::missing);
    CHECK(reference::resolveReference(value, {}, "wwv-10", "2026-10-01T00:00:00Z").status ==
          reference::ResolutionStatus::expired);
    const auto dir = std::filesystem::temp_directory_path() / "sdrcal-reference-tests";
    std::filesystem::create_directories(dir);
    const auto target = dir / "registry.json";
    {
        std::ofstream out(target);
        out << "known-good";
    }
    auto invalid = registry();
    invalid.integrity.sha256 = std::string(64, '0');
    CHECK(!reference::activateRegistryAtomically(target, invalid).succeeded());
    std::ifstream in(target);
    std::string retained;
    in >> retained;
    CHECK(retained == "known-good");
    auto signed_value = registry();
    signed_value.integrity.signature = profile::SignatureRecord{"ed25519", "test-key", "signature"};
    const auto verifier = [](auto, auto key, auto signature) {
        return key == "test-key" && signature == "signature";
    };
    CHECK(reference::activateRegistryAtomically(target, signed_value, verifier).succeeded());
    std::filesystem::remove_all(dir);
}

sdrcal::evidence::BundleRequest bundle() {
    sdrcal::evidence::BundleRequest value;
    value.bundle_id = "run-1";
    value.created_at = "2026-08-15T00:00:00Z";
    value.source_identity = "observation-1";
    value.monotonic_duration_ms = 1000;
    value.maximum_duration_ms = 2000;
    value.maximum_bundle_bytes = 4096;
    value.atomic_write_completed = true;
    value.artifacts = {{"summary", sdrcal::evidence::ArtifactClass::reduced_evidence,
                        "application/json", std::string(64, 'b'), 100, 1000, false}};
    value.metadata = {{"estimator", "v1"}};
    return value;
}
void evidenceTests() {
    using namespace sdrcal;
    auto value = bundle();
    CHECK(evidence::buildEvidenceManifest(value).valid());
    value.metadata.push_back({"api_token", "oops"});
    CHECK(!evidence::buildEvidenceManifest(value).valid());
    value = bundle();
    value.metadata.push_back({"artifact_path", "../private/key"});
    CHECK(!evidence::buildEvidenceManifest(value).valid());
    value = bundle();
    value.artifacts.push_back({"iq", evidence::ArtifactClass::raw_iq, "application/x-cf32le",
                               std::string(64, 'c'), 1000, 1000, false});
    CHECK(!evidence::buildEvidenceManifest(value).valid());
    value.allow_raw_iq = true;
    value.maximum_raw_iq_bytes = 1000;
    value.maximum_raw_iq_duration_ms = 1000;
    CHECK(evidence::buildEvidenceManifest(value).valid());
    value.maximum_raw_iq_bytes = 999;
    CHECK(!evidence::buildEvidenceManifest(value).valid());
    value = bundle();
    value.artifacts.push_back({"debug",
                               evidence::ArtifactClass::expanded_diagnostic,
                               "text/plain",
                               std::string(64, 'd'),
                               10,
                               {},
                               false});
    CHECK(!evidence::buildEvidenceManifest(value).valid());
    value.allow_expanded_diagnostics = true;
    CHECK(evidence::buildEvidenceManifest(value).valid());
    value = bundle();
    value.artifacts[0].byte_count = 5000;
    CHECK(!evidence::buildEvidenceManifest(value).valid());
    value = bundle();
    value.maximum_bundle_bytes = std::numeric_limits<std::uint64_t>::max();
    CHECK(!evidence::buildEvidenceManifest(value).valid());
}
} // namespace
int main() {
    registryTests();
    evidenceTests();
    if (failures)
        std::cerr << failures << " failure(s)\n";
    return failures == 0 ? 0 : 1;
}
