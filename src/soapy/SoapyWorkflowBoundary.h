#pragma once

#include "application/CalibrationWorkflow.h"
#include "capture/MemoryAcquisition.h"
#include "core/SignalQualityAnalyzer.h"
#include "soapy/SoapyCaptureSession.h"

#include <functional>

namespace sdrcal::soapy {

struct ReferenceConditionEvidence {
    bool established = false;
    std::string evidence_id;
};

using ReferenceConditionResolver =
    std::function<ReferenceConditionEvidence(const application::ObservationRequest&)>;

struct SoapyWorkflowOptions {
    capture::CaptureRequest capture_request;
    application::DeviceCandidate expected_device;
    capture::SettingTolerances setting_tolerances;
    capture::ResourceLimits resource_limits;
    capture::MemoryAcquisitionLimits memory_limits;
    core::CarrierEstimatorOptions estimator_options;
    core::SignalQualityOptions signal_quality_options;
    ReferenceConditionResolver reference_conditions;
};

class SoapyWorkflowBoundary final : public application::DeviceWorkflowBoundary {
  public:
    SoapyWorkflowBoundary(SoapyApi& api, SoapyWorkflowOptions options);

    [[nodiscard]] std::vector<application::DeviceCandidate> discover() override;
    [[nodiscard]] application::AcquisitionResult
    acquire(const application::DeviceCandidate& device,
            const profile::DeviceConfiguration& requested,
            const application::ObservationRequest& request,
            const std::function<bool()>& cancelled) override;

  private:
    SoapyApi& api_;
    SoapyWorkflowOptions options_;
};

} // namespace sdrcal::soapy
