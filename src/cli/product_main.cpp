#include "cli/ProductionCli.h"
#ifdef SDRCAL_PRODUCT_LIVE_ENABLED
#include "cli/LiveCliSupport.h"
#endif

#include <csignal>
#include <iostream>

namespace {
volatile std::sig_atomic_t cancelled = 0;
extern "C" void cancel(int) {
    cancelled = 1;
}
} // namespace

int main(int argc, char* argv[]) {
    std::vector<std::string> arguments;
    for (int index = 1; index < argc; ++index)
        arguments.emplace_back(argv[index]);
    const auto parsed = sdrcal::cli::parseProductArguments(arguments);
    if (parsed.action == sdrcal::cli::ProductAction::help && parsed.ok()) {
        std::cout << sdrcal::cli::productUsage();
        return 0;
    }
    if (parsed.action == sdrcal::cli::ProductAction::version && parsed.ok()) {
        std::cout << "sdrcal 0.1.0\n";
        return 0;
    }
    std::signal(SIGINT, cancel);
    sdrcal::cli::LiveBoundaryFactory liveFactory;
#ifdef SDRCAL_PRODUCT_LIVE_ENABLED
    liveFactory = sdrcal::cli::productionLiveBoundaryFactory();
#endif
    return static_cast<int>(sdrcal::cli::runProductCommand(
        parsed, std::cout, std::cerr, [] { return cancelled != 0; }, std::move(liveFactory)));
}
