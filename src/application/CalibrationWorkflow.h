#pragma once

#include "adapters/wsjtx/WsjtXAdapter.h"
#include "core/CalibrationModel.h"
#include "core/CarrierEstimator.h"
#include "core/ObservationAcceptance.h"
#include "core/Uncertainty.h"
#include "evidence/EvidenceBundle.h"
#include "profile/ProfileTypes.h"
#include "reference/ReferenceRegistry.h"

#include <complex>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace sdrcal::application {

enum class AcquisitionStatus { success, cancelled, failed };

struct DeviceCandidate {
    profile::DeviceIdentity identity;
    profile::DeviceConfiguration configuration;
};

struct ObservationRequest {
    std::string observation_id;
    std::string independence_id;
    std::string reference_id;
    double indicated_center_frequency_hz = 0.0;
};

struct AcquisitionResult {
    AcquisitionStatus status = AcquisitionStatus::failed;
    std::string reason;
    profile::DeviceIdentity identity;
    profile::DeviceConfiguration configuration;
    std::vector<std::complex<float>> samples;
    double effective_indicated_center_frequency_hz = 0.0;
    bool effective_indicated_center_verified = false;
    double sample_rate_sps = 0.0;
    double duration_seconds = 0.0;
    double signal_to_noise_db = 0.0;
    std::uint64_t clipped_samples = 0;
    std::uint64_t missing_samples = 0;
    std::uint64_t discontinuities = 0;
    double frequency_instability_hz = 0.0;
    double interference_to_carrier_db = 0.0;
    core::EffectiveConfigurationValidity effective_configuration =
        core::EffectiveConfigurationValidity::unverified;
    bool reference_conditions_met = false;
    bool final_device_state_safe = false;
};

class DeviceWorkflowBoundary {
  public:
    virtual ~DeviceWorkflowBoundary() = default;
    [[nodiscard]] virtual std::vector<DeviceCandidate> discover() = 0;
    [[nodiscard]] virtual AcquisitionResult acquire(const DeviceCandidate& device,
                                                    const profile::DeviceConfiguration& requested,
                                                    const ObservationRequest& request,
                                                    const std::function<bool()>& cancelled) = 0;
};

struct InteroperabilityRequest {
    std::string wsjtx_version;
    std::string configuration_name;
};

struct WorkflowRequest {
    std::string profile_id;
    std::string calibration_run_id;
    std::string created_at;
    std::string not_valid_after;
    std::string selected_device_identifier;
    profile::DeviceConfiguration requested_configuration;
    std::int64_t minimum_warmup_seconds = 0;
    profile::TemperatureValidity temperature;
    reference::ReferenceRegistry global_registry;
    profile::VerifyHook registry_verifier;
    std::vector<reference::ReferenceEntry> local_references;
    std::vector<ObservationRequest> observations;
    core::FrequencyModelType model_type = core::FrequencyModelType::linear;
    core::CarrierEstimatorOptions estimator_options;
    core::ObservationAcceptancePolicy acceptance_policy;
    core::UncertaintyBudget uncertainty_budget;
    std::vector<profile::AssuranceComponentRecord> assurance_components;
    int qualification_threshold = 90;
    std::string software_name = "SDR Calibration";
    std::string software_version;
    evidence::BundleRequest evidence;
    std::optional<InteroperabilityRequest> interoperability;
};

enum class WorkflowStatus { success, cancelled, failed };
enum class WorkflowStage {
    validate_request,
    discover_device,
    resolve_references,
    acquire_observations,
    estimate_and_accept,
    fit_model,
    calculate_uncertainty,
    calculate_assurance,
    build_evidence,
    build_profile,
    export_interoperability,
    complete
};

struct StageRecord {
    WorkflowStage stage = WorkflowStage::validate_request;
    bool succeeded = false;
    std::string detail;
};

struct WorkflowResult {
    WorkflowStatus status = WorkflowStatus::failed;
    WorkflowStage terminal_stage = WorkflowStage::validate_request;
    std::string reason;
    std::vector<StageRecord> stages;
    std::optional<profile::CalibrationProfile> profile;
    std::string evidence_manifest;
    std::string evidence_summary;
    std::optional<wsjtx::SettingsResult> interoperability;
    std::vector<std::string> warnings;
    [[nodiscard]] bool succeeded() const noexcept {
        return status == WorkflowStatus::success;
    }
};

using CancellationCheck = std::function<bool()>;

class CalibrationWorkflow {
  public:
    [[nodiscard]] WorkflowResult run(const WorkflowRequest& request,
                                     DeviceWorkflowBoundary& devices,
                                     CancellationCheck cancelled = {}) const;
};

} // namespace sdrcal::application
