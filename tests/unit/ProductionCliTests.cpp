#include "cli/ProductionCli.h"
#include "profile/CanonicalJson.h"
#include "profile/Sha256.h"
#include "reference/ReferenceRegistry.h"

#include <algorithm>
#include <bit>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace {
int failures = 0;
#define CHECK(condition)                                                                           \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            std::cerr << __FILE__ << ':' << __LINE__ << ": check failed: " #condition "\n";        \
            ++failures;                                                                            \
        }                                                                                          \
    } while (false)

using sdrcal::profile::JsonArray;
using sdrcal::profile::JsonObject;

std::string samples(double offsetHz) {
    std::string bytes;
    for (std::size_t index = 0; index < 1024U; ++index) {
        const double phase =
            2.0 * 3.14159265358979323846 * offsetHz * static_cast<double>(index) / 1024.0;
        for (const float value :
             {static_cast<float>(std::cos(phase)), static_cast<float>(std::sin(phase))}) {
            const auto bits = std::bit_cast<std::uint32_t>(value);
            for (unsigned int byte = 0; byte < 4U; ++byte)
                bytes.push_back(static_cast<char>((bits >> (8U * byte)) & 0xffU));
        }
    }
    return bytes;
}

void write(const std::filesystem::path& path, std::string_view value) {
    std::ofstream stream(path, std::ios::binary);
    stream.write(value.data(), static_cast<std::streamsize>(value.size()));
}

JsonObject config() {
    return {{"clock_source", "internal"},
            {"sample_rate_hz", 1024},
            {"bandwidth_hz", 1000},
            {"frequency_correction_ppm", 0.0}};
}

std::string requestJson(const std::string& first, const std::string& second) {
    using namespace sdrcal;
    reference::ReferenceRegistry registry;
    registry.registry_id = "fixture-registry";
    registry.registry_version = "1";
    registry.generated_at = "2026-08-01T00:00:00Z";
    registry.expires_at = "2026-09-01T00:00:00Z";
    registry.provenance = "test";
    for (const auto& [id, frequency] : std::vector<std::pair<std::string, double>>{
             {"ref-10", 10'000'000.0}, {"ref-20", 20'000'000.0}}) {
        registry.references.push_back({id,
                                       core::ReferenceClass::authority_confirmed,
                                       100,
                                       frequency,
                                       0.1,
                                       reference::OperatingStatus::active,
                                       "fixture",
                                       {"test"},
                                       {"test only"},
                                       {{id + "-e", "fixture", "2026-08-01T00:00:00Z",
                                         "2026-09-01T00:00:00Z", std::string(64, 'a')}}});
    }
    CHECK(reference::refreshRegistryIntegrity(registry));
    registry.integrity.signature =
        profile::SignatureRecord{"ed25519", "fixture-key", "pinned-signature"};
    const auto parsedRegistry = profile::parseJson(reference::serializeRegistry(registry));
    CHECK(parsedRegistry.success);

    auto observation = [](std::string id, std::string independent, std::string reference,
                          double center, std::string path, std::string digest) {
        return JsonObject{{"observation_id", std::move(id)},
                          {"independence_id", std::move(independent)},
                          {"reference_id", std::move(reference)},
                          {"indicated_center_frequency_hz", center},
                          {"effective_indicated_center_frequency_hz", center},
                          {"input_path", std::move(path)},
                          {"sha256", std::move(digest)},
                          {"maximum_bytes", 10000},
                          {"duration_seconds", 1.0},
                          {"sample_count", 1024},
                          {"signal_to_noise_db", 30.0},
                          {"clipped_samples", 0},
                          {"missing_samples", 0},
                          {"discontinuities", 0},
                          {"frequency_instability_hz", 0.1},
                          {"interference_to_carrier_db", -20.0},
                          {"reference_conditions_met", true}};
    };
    JsonArray assurance;
    for (const auto* name : {"reference_provenance", "received_signal_suitability",
                             "device_binding", "environmental_validity", "observation_quality",
                             "model_quality", "artifact_integrity", "evidence_completeness"})
        assurance.emplace_back(JsonObject{{"name", name}, {"score", 95}, {"basis", "fixture"}});
    JsonObject root{
        {"schema_name", "sdrcal-recorded-calibration-request"},
        {"schema_version", "1.0.0"},
        {"profile_id", "profile-cli"},
        {"calibration_run_id", "run-cli"},
        {"created_at", "2026-08-15T13:00:00Z"},
        {"not_valid_after", "2027-08-15T13:00:00Z"},
        {"software_version", "0.1.0-test"},
        {"minimum_warmup_seconds", 0},
        {"qualification_threshold", 90},
        {"device", JsonObject{{"driver", "recorded"},
                              {"manufacturer", "Test"},
                              {"model", "Fixture"},
                              {"identifier", "fixture-1"},
                              {"identity_strength", "hardware_serial"},
                              {"effective_configuration", config()}}},
        {"requested_configuration", config()},
        {"temperature", JsonObject{{"reference_c", 20.0},
                                   {"minimum_c", 10.0},
                                   {"maximum_c", 30.0},
                                   {"measurement_location", "fixture"}}},
        {"registry", parsedRegistry.value},
        {"observations", JsonArray{observation("obs-10", "independent-10", "ref-10", 10'000'000.0,
                                               "first.cf32", profile::sha256Hex(first)),
                                   observation("obs-20", "independent-20", "ref-20", 20'000'000.0,
                                               "second.cf32", profile::sha256Hex(second))}},
        {"uncertainty",
         JsonObject{{"version", "uncertainty-budget-v1"},
                    {"coverage_factor", 2.0},
                    {"components", JsonArray{JsonObject{{"name", "reference"},
                                                        {"standard_uncertainty_hz", 0.2},
                                                        {"sensitivity_coefficient", 1.0}},
                                             JsonObject{{"name", "estimator"},
                                                        {"standard_uncertainty_hz", 0.3},
                                                        {"sensitivity_coefficient", 1.0}}}},
                    {"correlations", JsonArray{}},
                    {"excluded_or_unknown_material_components", JsonArray{}}}},
        {"assurance_components", assurance},
        {"estimator_policy", JsonObject{{"minimum_samples", 64},
                                        {"minimum_mean_power", 1.0e-12},
                                        {"minimum_model_coherence", 0.75},
                                        {"maximum_absolute_frequency_fraction", 0.45},
                                        {"robust_iterations", 4},
                                        {"huber_threshold_radians", 0.35}}},
        {"acceptance_policy", JsonObject{{"version", "observation-acceptance-v1"},
                                         {"minimum_duration_seconds", 1.0},
                                         {"minimum_signal_to_noise_db", 10.0},
                                         {"maximum_clipped_fraction", 0.0},
                                         {"maximum_missing_samples", 0},
                                         {"maximum_discontinuities", 0},
                                         {"maximum_absolute_drift_hz_per_second", 1.0},
                                         {"maximum_frequency_instability_hz", 0.5},
                                         {"maximum_interference_to_carrier_db", -10.0}}},
        {"evidence", JsonObject{{"bundle_id", "bundle-cli"},
                                {"monotonic_duration_ms", 2000},
                                {"maximum_bundle_bytes", 100000},
                                {"maximum_duration_ms", 10000}}}};
    return profile::canonicalizeJson(root).value;
}

void argumentTests() {
    using namespace sdrcal::cli;
    CHECK(parseProductArguments({"--help"}).action == ProductAction::help);
    CHECK(parseProductArguments({"--version"}).action == ProductAction::version);
    CHECK(parseProductArguments(
              {"calibrate", "--request", "a", "--trust-file", "t", "--output-dir", "b"})
              .ok());
    CHECK(!parseProductArguments({"calibrate", "--request", "a"}).ok());
    CHECK(!parseProductArguments(
               {"calibrate", "--request", "a", "--request", "b", "--output-dir", "c"})
               .ok());
}

void requestValidationTests() {
    const auto first = samples(10.0);
    const auto second = samples(20.0);
    const std::map<std::string, std::string> trust{{"fixture-key", "pinned-signature"}};
    CHECK(sdrcal::cli::parseProductRequest(requestJson(first, second), "/tmp", trust).ok());
    CHECK(!sdrcal::cli::parseProductRequest("{", "/tmp", trust).ok());
    auto unknown = requestJson(first, second);
    unknown.insert(unknown.size() - 1U, ",\"unknown_member\":true");
    CHECK(!sdrcal::cli::parseProductRequest(unknown, "/tmp", trust).ok());
    CHECK(!sdrcal::cli::parseProductRequest(requestJson(first, second), "/tmp", {}).ok());
    auto traversal = requestJson(first, second);
    const auto position = traversal.find("first.cf32");
    traversal.replace(position, std::string("first.cf32").size(), "../first.cf32");
    CHECK(!sdrcal::cli::parseProductRequest(traversal, "/tmp", trust).ok());

    auto excessive = requestJson(first, second);
    const std::string configuredBound = "\"maximum_bytes\":10000";
    const std::string excessiveBound =
        "\"maximum_bytes\":" +
        std::to_string(sdrcal::cli::recordedInputMaximumBytes() + 1U);
    std::size_t boundPosition = 0;
    std::size_t replacedBounds = 0;
    while ((boundPosition = excessive.find(configuredBound, boundPosition)) != std::string::npos) {
        excessive.replace(boundPosition, configuredBound.size(), excessiveBound);
        boundPosition += excessiveBound.size();
        ++replacedBounds;
    }
    CHECK(replacedBounds == 2U);
    const auto excessiveResult =
        sdrcal::cli::parseProductRequest(excessive, "/tmp", trust);
    CHECK(!excessiveResult.ok());
    CHECK(std::any_of(excessiveResult.errors.begin(), excessiveResult.errors.end(),
                      [](const auto& error) {
        return error.find("recorded-input resource policy") != std::string::npos;
    }));
}

void commandTests() {
    using namespace sdrcal::cli;
    const auto root = std::filesystem::temp_directory_path() / "sdrcal-production-cli-tests";
    std::error_code ignored;
    std::filesystem::remove_all(root, ignored);
    std::filesystem::create_directories(root);
    const auto first = samples(10.0);
    const auto second = samples(20.0);
    write(root / "first.cf32", first);
    write(root / "second.cf32", second);
    write(root / "request.json", requestJson(first, second));
    write(root / "trust.json", "{\"fixture-key\":\"pinned-signature\"}");
    ProductArguments arguments{
        ProductAction::calibrate, root / "request.json", root / "trust.json", root / "output", {}};
    std::ostringstream output, diagnostics;
    const auto status = runProductCommand(arguments, output, diagnostics);
    if (status != ProductExit::success)
        std::cerr << "output=" << output.str() << " diagnostics=" << diagnostics.str();
    CHECK(status == ProductExit::success);
    CHECK(output.str().find("\"status\":\"success\"") != std::string::npos);
    CHECK(diagnostics.str().find("progress:") != std::string::npos);
    CHECK(std::filesystem::exists(root / "output/profile.json"));
    CHECK(std::filesystem::exists(root / "output/evidence.json"));
    CHECK(std::filesystem::exists(root / "output/summary.json"));
    std::ifstream evidenceStream(root / "output/evidence.json", std::ios::binary);
    std::stringstream evidenceText;
    evidenceText << evidenceStream.rdbuf();
    CHECK(evidenceText.str().find("\"atomic_write_completed\":true") != std::string::npos);
    std::ifstream profileStream(root / "output/profile.json", std::ios::binary);
    std::stringstream profileText;
    profileText << profileStream.rdbuf();
    CHECK(profileText.str().find(sdrcal::profile::sha256Hex(evidenceText.str())) !=
          std::string::npos);
    std::ostringstream repeatedOutput, repeatedDiagnostics;
    CHECK(runProductCommand(arguments, repeatedOutput, repeatedDiagnostics) == ProductExit::output);
    CHECK(repeatedOutput.str().find("output_error") != std::string::npos);

    auto cancelledArguments = arguments;
    cancelledArguments.output_directory = root / "cancelled";
    std::ostringstream cancelledOutput, cancelledDiagnostics;
    CHECK(runProductCommand(cancelledArguments, cancelledOutput, cancelledDiagnostics,
                            [] { return true; }) == ProductExit::cancelled);
    CHECK(!std::filesystem::exists(root / "cancelled"));

    auto badText = requestJson(first, second);
    const auto digest = sdrcal::profile::sha256Hex(first);
    badText.replace(badText.find(digest), digest.size(), std::string(64, '0'));
    write(root / "bad.json", badText);
    ProductArguments bad{
        ProductAction::calibrate, root / "bad.json", root / "trust.json", root / "bad-output", {}};
    std::ostringstream badOutput, badDiagnostics;
    CHECK(runProductCommand(bad, badOutput, badDiagnostics) == ProductExit::input);
    CHECK(!std::filesystem::exists(root / "bad-output"));
    CHECK(!std::filesystem::exists(root / "bad-output.staging"));
    std::filesystem::remove_all(root, ignored);
}
} // namespace

int main() {
    argumentTests();
    requestValidationTests();
    commandTests();
    if (failures)
        std::cerr << failures << " failure(s)\n";
    return failures == 0 ? 0 : 1;
}
