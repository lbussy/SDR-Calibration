#include "cli/CaptureCliArguments.h"
#include "cli/CaptureCliWorkflow.h"
#include "soapy/RealSoapyApi.h"
#include "sdrcal/Version.h"

#include <csignal>
#include <iostream>
#include <string>
#include <vector>

namespace {

volatile std::sig_atomic_t cancellationRequested = 0;

extern "C" void requestCancellation(int) { cancellationRequested = 1; }

} // namespace

int main(int argc, char* argv[]) {
    std::vector<std::string> arguments;
    arguments.reserve(static_cast<std::size_t>(argc > 0 ? argc - 1 : 0));
    for (int index = 1; index < argc; ++index) {
        arguments.emplace_back(argv[index]);
    }

    const auto parsed = sdrcal::cli::parseCaptureArguments(arguments);
    if (parsed.action == sdrcal::cli::CommandAction::help && parsed.ok()) {
        std::cout << sdrcal::cli::captureUsage();
        return 0;
    }
    if (parsed.action == sdrcal::cli::CommandAction::version && parsed.ok()) {
        std::cout << "sdrcal-capture " << sdrcal::kVersion << '\n';
        return 0;
    }
    if (!parsed.ok()) {
        for (const auto& error : parsed.errors) {
            std::cerr << "error: " << error << '\n';
        }
        std::cerr << "Use --help for usage.\n";
        return static_cast<int>(sdrcal::cli::ExitStatus::usage);
    }

    std::signal(SIGINT, requestCancellation);
    sdrcal::soapy::RealSoapyApi api;
    return static_cast<int>(sdrcal::cli::runCaptureCommand(
        parsed,
        api,
        std::cout,
        std::cerr,
        []() { return cancellationRequested != 0; }));
}
