#include "cli/LiveCliSupport.h"

#include "soapy/RealSoapyApi.h"
#include "soapy/SoapyWorkflowBoundary.h"

#include <stdexcept>
#include <utility>

namespace sdrcal::cli {
namespace {

class ProductionLiveBoundary final : public application::DeviceWorkflowBoundary {
  public:
    explicit ProductionLiveBoundary(const ProductRequest& request)
        : boundary_(api_, options(request)) {}

    [[nodiscard]] std::vector<application::DeviceCandidate> discover() override {
        return boundary_.discover();
    }

    [[nodiscard]] application::AcquisitionResult
    acquire(const application::DeviceCandidate& device,
            const profile::DeviceConfiguration& requested,
            const application::ObservationRequest& observation,
            const std::function<bool()>& cancelled) override {
        return boundary_.acquire(device, requested, observation, cancelled);
    }

  private:
    static soapy::SoapyWorkflowOptions options(const ProductRequest& request) {
        if (request.input_mode != ProductInputMode::live || !request.live_acquisition ||
            request.workflow.observations.empty())
            throw std::invalid_argument("live request is incomplete");
        const auto& live = *request.live_acquisition;
        soapy::SoapyWorkflowOptions result;
        result.capture_request.device_arguments = live.device_arguments;
        result.capture_request.rx_channel = live.rx_channel;
        result.capture_request.center_frequency_hz =
            request.workflow.observations.front().indicated_center_frequency_hz;
        result.capture_request.sample_rate_sps =
            static_cast<double>(request.workflow.requested_configuration.sample_rate_hz);
        if (request.workflow.requested_configuration.bandwidth_hz)
            result.capture_request.bandwidth_hz =
                static_cast<double>(*request.workflow.requested_configuration.bandwidth_hz);
        result.capture_request.gain_db = live.gain_db;
        result.capture_request.duration_seconds = live.duration_seconds;
        result.capture_request.sample_count = live.sample_count;
        result.capture_request.read_timeout = live.read_timeout;
        result.capture_request.setting_policy = capture::SettingPolicy::strict;
        result.capture_request.purpose = "production-live-calibration";
        result.expected_device = request.device;
        result.memory_limits.maximum_bytes = live.maximum_memory_bytes;
        result.estimator_options = request.workflow.estimator_options;

        std::map<std::string, bool> conditionFree;
        for (const auto& reference : request.workflow.global_registry.references)
            conditionFree.emplace(reference.reference_id,
                                  reference.conditions.size() == 1U &&
                                      reference.conditions.front() == "none");
        const std::string evidencePrefix =
            "authenticated-registry:" + request.workflow.global_registry.registry_id + ":" +
            request.workflow.global_registry.registry_version + ":";
        result.reference_conditions = [conditionFree = std::move(conditionFree),
                                       evidencePrefix](const auto& observation) {
            const auto found = conditionFree.find(observation.reference_id);
            const bool established = found != conditionFree.end() && found->second;
            return soapy::ReferenceConditionEvidence{established,
                                                     evidencePrefix + observation.reference_id};
        };
        return result;
    }

    soapy::RealSoapyApi api_;
    soapy::SoapyWorkflowBoundary boundary_;
};

} // namespace

LiveBoundaryFactory productionLiveBoundaryFactory() {
    return [](const ProductRequest& request) {
        return std::make_unique<ProductionLiveBoundary>(request);
    };
}

} // namespace sdrcal::cli
