#include "application/CalibrationWorkflow.h"
#include "profile/CanonicalJson.h"
#include "profile/ProfileEngine.h"
#include "reference/ReferenceRegistry.h"

#include <iostream>

namespace {
int failures = 0;
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            std::cerr << __FILE__ << ':' << __LINE__ << " CHECK(" #x ") failed\n";                 \
            ++failures;                                                                            \
        }                                                                                          \
    } while (false)

class FakeDevices final : public sdrcal::application::DeviceWorkflowBoundary {
  public:
    std::vector<sdrcal::application::DeviceCandidate> candidates;
    bool fail = false;
    bool unsafe = false;
    bool mismatch = false;
    bool reject_metrics = false;
    int calls = 0;

    std::vector<sdrcal::application::DeviceCandidate> discover() override {
        return candidates;
    }

    sdrcal::application::AcquisitionResult
    acquire(const sdrcal::application::DeviceCandidate& device,
            const sdrcal::profile::DeviceConfiguration& requested,
            const sdrcal::application::ObservationRequest& request,
            const std::function<bool()>& cancelled) override {
        ++calls;
        sdrcal::application::AcquisitionResult value;
        value.identity = device.identity;
        value.configuration = device.configuration;
        if (requested.sample_rate_hz <= 0) {
            value.reason = "invalid requested configuration";
            return value;
        }
        if (cancelled && cancelled()) {
            value.status = sdrcal::application::AcquisitionStatus::cancelled;
            return value;
        }
        if (fail) {
            value.reason = "fake acquisition failure";
            return value;
        }
        if (mismatch)
            value.configuration.sample_rate_hz += 1;
        value.status = sdrcal::application::AcquisitionStatus::success;
        value.samples.assign(1024, {1.0F, 0.0F});
        value.effective_indicated_center_frequency_hz = request.indicated_center_frequency_hz;
        value.effective_indicated_center_verified = true;
        value.sample_rate_sps = 1024.0;
        value.duration_seconds = 1.0;
        value.signal_to_noise_db = reject_metrics ? 0.0 : 30.0;
        value.frequency_instability_hz = 0.1;
        value.interference_to_carrier_db = -20.0;
        value.effective_configuration = sdrcal::core::EffectiveConfigurationValidity::verified;
        value.reference_conditions_met = true;
        value.final_device_state_safe = !unsafe;
        return value;
    }
};

sdrcal::application::DeviceCandidate device() {
    sdrcal::application::DeviceCandidate value;
    value.identity = {"fake", "Test", "Fake SDR", "device-1",
                      sdrcal::profile::IdentityStrength::hardware_serial};
    value.configuration.clock_source = "fake-clock";
    value.configuration.sample_rate_hz = 1024;
    value.configuration.bandwidth_hz = 1000;
    return value;
}

sdrcal::reference::ReferenceRegistry
registry(sdrcal::core::ReferenceClass kind = sdrcal::core::ReferenceClass::authority_confirmed) {
    using namespace sdrcal;
    reference::ReferenceRegistry value;
    value.registry_id = "test-registry";
    value.registry_version = "2026.08";
    value.generated_at = "2026-08-01T00:00:00Z";
    value.expires_at = "2026-09-01T00:00:00Z";
    value.provenance = "deterministic fixture";
    const int ceiling = core::referenceAssuranceCeiling(kind);
    for (const auto& [id, frequency] : std::vector<std::pair<std::string, double>>{
             {"ref-10", 10'000'000.0}, {"ref-20", 20'000'000.0}}) {
        value.references.push_back(
            {id,
             kind,
             ceiling,
             frequency,
             0.1,
             reference::OperatingStatus::active,
             "fixture",
             {"fixture condition"},
             {"fixture only"},
             {{id + "-evidence", "fixture evidence", "2026-08-01T00:00:00Z", "2026-09-01T00:00:00Z",
               std::string(64, id == "ref-10" ? 'a' : 'b')}}});
    }
    CHECK(reference::refreshRegistryIntegrity(value));
    value.integrity.signature =
        profile::SignatureRecord{"ed25519", "fixture-key", "fixture-signature"};
    return value;
}

std::vector<sdrcal::profile::AssuranceComponentRecord> assurance(int score = 95) {
    return {{"reference_provenance", score, "fixture"},
            {"received_signal_suitability", score, "fixture"},
            {"device_binding", score, "fixture"},
            {"environmental_validity", score, "fixture"},
            {"observation_quality", score, "fixture"},
            {"model_quality", score, "fixture"},
            {"artifact_integrity", score, "fixture"},
            {"evidence_completeness", score, "fixture"}};
}

sdrcal::application::WorkflowRequest request() {
    using namespace sdrcal;
    application::WorkflowRequest value;
    value.profile_id = "profile-phase-11";
    value.calibration_run_id = "run-phase-11";
    value.created_at = "2026-08-15T13:00:00Z";
    value.not_valid_after = "2027-08-15T13:00:00Z";
    value.selected_device_identifier = "device-1";
    value.requested_configuration = device().configuration;
    value.temperature = {20.0, 10.0, 30.0, "fixture enclosure"};
    value.global_registry = registry();
    value.registry_verifier = [](auto payload, auto key, auto signature) {
        return !payload.empty() && key == "fixture-key" && signature == "fixture-signature";
    };
    value.observations = {{"obs-10", "independent-10", "ref-10", 10'000'011.0},
                          {"obs-20", "independent-20", "ref-20", 20'000'021.0}};
    value.uncertainty_budget.components = {{"reference", 0.2, 1.0}, {"estimator", 0.3, 1.0}};
    value.assurance_components = assurance();
    value.software_version = "0.1.0-test";
    value.evidence.bundle_id = "bundle-phase-11";
    value.evidence.created_at = value.created_at;
    value.evidence.source_identity = value.calibration_run_id;
    value.evidence.monotonic_duration_ms = 2000;
    value.evidence.maximum_duration_ms = 10000;
    value.evidence.maximum_bundle_bytes = 100000;
    value.interoperability = application::InteroperabilityRequest{"2.7.0", "Test Rig"};
    return value;
}

void successAndRepeatability() {
    using namespace sdrcal;
    FakeDevices fake;
    fake.candidates = {device()};
    application::CalibrationWorkflow workflow;
    const auto first = workflow.run(request(), fake);
    CHECK(first.succeeded());
    CHECK(first.profile.has_value());
    CHECK(profile::validateProfile(*first.profile).valid());
    CHECK(first.profile->profile_status == profile::ProfileStatus::qualification_capable);
    CHECK(first.profile->adapters.size() == 1U);
    CHECK(first.interoperability && first.interoperability->succeeded());
    CHECK(!first.evidence_manifest.empty());
    CHECK(first.evidence_summary.find("requested_center_frequency_hz") != std::string::npos);
    CHECK(first.evidence_summary.find("effective_center_frequency_hz") != std::string::npos);
    CHECK(first.evidence_manifest.find("\"atomic_write_completed\":false") != std::string::npos);
    CHECK(first.stages.back().stage == application::WorkflowStage::complete);
    FakeDevices second_fake;
    second_fake.candidates = {device()};
    const auto second = workflow.run(request(), second_fake);
    CHECK(second.succeeded());
    CHECK(profile::serializeProfile(*first.profile) == profile::serializeProfile(*second.profile));
    CHECK(first.evidence_manifest == second.evidence_manifest);
}

void failClosedPaths() {
    using namespace sdrcal;
    application::CalibrationWorkflow workflow;
    FakeDevices fake;
    fake.candidates = {device(), device()};
    auto result = workflow.run(request(), fake);
    CHECK(!result.succeeded() && !result.profile);
    CHECK(result.terminal_stage == application::WorkflowStage::discover_device);

    fake.candidates = {device()};
    fake.fail = true;
    result = workflow.run(request(), fake);
    CHECK(result.terminal_stage == application::WorkflowStage::acquire_observations);
    fake.fail = false;

    fake.unsafe = true;
    result = workflow.run(request(), fake);
    CHECK(result.terminal_stage == application::WorkflowStage::acquire_observations);
    CHECK(!result.profile);

    fake.unsafe = false;
    fake.reject_metrics = true;
    result = workflow.run(request(), fake);
    CHECK(result.terminal_stage == application::WorkflowStage::estimate_and_accept);
    CHECK(!result.profile);

    fake.reject_metrics = false;
    auto invalid_reference = request();
    invalid_reference.observations[0].reference_id = "missing";
    result = workflow.run(invalid_reference, fake);
    CHECK(result.terminal_stage == application::WorkflowStage::resolve_references);

    auto unsigned_registry = request();
    unsigned_registry.global_registry.integrity.signature.reset();
    result = workflow.run(unsigned_registry, fake);
    CHECK(result.terminal_stage == application::WorkflowStage::resolve_references);
    CHECK(result.reason.find("authentication") != std::string::npos);

    auto invalid_uncertainty = request();
    invalid_uncertainty.uncertainty_budget.components.clear();
    result = workflow.run(invalid_uncertainty, fake);
    CHECK(result.terminal_stage == application::WorkflowStage::calculate_uncertainty);

    auto no_span = request();
    no_span.observations[1].indicated_center_frequency_hz =
        no_span.observations[0].indicated_center_frequency_hz;
    result = workflow.run(no_span, fake);
    CHECK(result.terminal_stage == application::WorkflowStage::fit_model);

    auto private_evidence = request();
    private_evidence.evidence.metadata.push_back({"api_token", "secret"});
    result = workflow.run(private_evidence, fake);
    CHECK(result.terminal_stage == application::WorkflowStage::build_evidence);

    result = workflow.run(request(), fake, [] { return true; });
    CHECK(result.status == application::WorkflowStatus::cancelled);
    CHECK(!result.profile);

    int cancellation_checks = 0;
    result = workflow.run(request(), fake, [&] { return ++cancellation_checks >= 7; });
    CHECK(result.status == application::WorkflowStatus::cancelled);
    CHECK(result.terminal_stage == application::WorkflowStage::fit_model);
}

void reducedAssuranceAndInteropWarning() {
    using namespace sdrcal;
    application::CalibrationWorkflow workflow;
    FakeDevices fake;
    fake.candidates = {device()};
    auto reduced = request();
    reduced.global_registry = registry(core::ReferenceClass::locally_characterized);
    reduced.interoperability->wsjtx_version = "9.0.0";
    const auto result = workflow.run(reduced, fake);
    CHECK(result.succeeded());
    CHECK(result.profile->profile_status == profile::ProfileStatus::informational_only);
    CHECK(result.profile->assurance.reliability_quotient == 75);
    CHECK(!result.warnings.empty());
    CHECK(result.profile->adapters.empty());
}
} // namespace

int main() {
    successAndRepeatability();
    failClosedPaths();
    reducedAssuranceAndInteropWarning();
    if (failures)
        std::cerr << failures << " failure(s)\n";
    return failures == 0 ? 0 : 1;
}
