#include "cli/CaptureCliWorkflow.h"

#include "capture/CapturePlan.h"
#include "capture/CaptureRecorder.h"
#include "soapy/SoapyCaptureSession.h"

#include <iomanip>
#include <ostream>
#include <utility>

namespace sdrcal::cli {
namespace {

void printOptional(std::ostream& output, const std::optional<double>& value) {
    if (value.has_value()) {
        output << std::setprecision(17) << *value;
    } else {
        output << "not_requested_or_unavailable";
    }
}

void printSetting(
    std::ostream& output,
    const char* name,
    const capture::EffectiveSetting& setting) {
    output << "  " << name << ": requested=";
    printOptional(output, setting.requested);
    output << " effective=";
    printOptional(output, setting.effective);
    output << " state=" << capture::toString(setting.state) << '\n';
}

} // namespace

ExitStatus runCaptureCommand(
    const ParseResult& parsed,
    soapy::SoapyApi& api,
    std::ostream& output,
    std::ostream& error,
    CancellationCheck cancelled) {
    if (!parsed.ok() || parsed.action != CommandAction::capture || !parsed.request.has_value()) {
        for (const auto& message : parsed.errors) {
            error << "error: " << message << '\n';
        }
        return ExitStatus::usage;
    }

    const capture::CaptureRequest& request = *parsed.request;
    output << "Requested settings:\n"
           << "  channel: " << request.rx_channel << '\n'
           << "  center_frequency_hz: " << std::setprecision(17)
           << request.center_frequency_hz << '\n'
           << "  sample_rate_sps: " << request.sample_rate_sps << '\n'
           << "  bandwidth_hz: ";
    printOptional(output, request.bandwidth_hz);
    output << "\n  gain_db: ";
    printOptional(output, request.gain_db);
    output << "\n  setting_policy: " << capture::toString(request.setting_policy) << '\n';

    soapy::SoapyCaptureSession session(api);
    const auto prepared = session.prepare(request);
    if (!prepared.ready || !prepared.plan.has_value()) {
        error << "preparation failed at " << soapy::toString(prepared.error.stage) << ": "
              << prepared.error.message << '\n';
        return ExitStatus::preparation;
    }

    output << "Effective settings:\n";
    printSetting(output, "center_frequency_hz", prepared.effective.center_frequency_hz);
    printSetting(output, "sample_rate_sps", prepared.effective.sample_rate_sps);
    printSetting(output, "bandwidth_hz", prepared.effective.bandwidth_hz);
    printSetting(output, "gain_db", prepared.effective.gain_db);
    output << "  target_samples: " << prepared.plan->target_samples << '\n'
           << "  target_bytes: " << prepared.plan->target_bytes << '\n';

    capture::CaptureRecorder recorder;
    const auto result = recorder.record(
        *prepared.plan, prepared.device, session, std::move(cancelled));
    output << "Capture outcome:\n"
           << "  status: " << capture::toString(result.status) << '\n'
           << "  written_samples: " << result.stream.written_samples << '\n'
           << "  written_bytes: " << result.stream.written_bytes << '\n'
           << "  raw_path: " << result.raw_path.string() << '\n'
           << "  manifest_path: " << result.manifest_path.string() << '\n'
           << "  final_state: " << capture::toString(result.final_state) << '\n';
    if (result.status != capture::CaptureStatus::complete) {
        error << "capture did not complete: " << capture::toString(result.error.category)
              << ": " << result.error.message << '\n';
        return ExitStatus::capture;
    }
    return ExitStatus::success;
}

} // namespace sdrcal::cli
