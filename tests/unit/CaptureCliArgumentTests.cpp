#include "cli/CaptureCliArguments.h"

#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace sdrcal::cli;

namespace {
class TestFailure : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};
#define CHECK(condition)                                                                       \
    do {                                                                                       \
        if (!(condition)) {                                                                    \
            throw TestFailure(std::string("check failed: ") + #condition);                    \
        }                                                                                      \
    } while (false)

std::vector<std::string> validArguments() {
    return {"--device", "driver=fake", "--device", "serial=123", "--channel", "1",
            "--frequency-hz", "10000000", "--sample-rate", "2000000", "--bandwidth",
            "1800000", "--gain", "20", "--samples", "8", "--output", "capture",
            "--timeout-ms", "250", "--policy", "permissive", "--purpose", "diagnostic"};
}

void testCompleteRequest() {
    const auto result = parseCaptureArguments(validArguments());
    CHECK(result.ok());
    CHECK(result.request.has_value());
    CHECK(result.request->device_arguments.size() == 2);
    CHECK(result.request->rx_channel == 1);
    CHECK(result.request->sample_count == 8);
    CHECK(result.request->read_timeout.count() == 250);
    CHECK(result.request->setting_policy == sdrcal::capture::SettingPolicy::permissive);
}

void testHelpAndVersionAreDeviceFree() {
    CHECK(parseCaptureArguments({"--help"}).ok());
    CHECK(parseCaptureArguments({"--help"}).action == CommandAction::help);
    CHECK(parseCaptureArguments({"--version"}).action == CommandAction::version);
    CHECK(!parseCaptureArguments({"--help", "--device", "driver=fake"}).ok());
}

void testMissingAndAmbiguousValuesFail() {
    CHECK(!parseCaptureArguments({}).ok());
    auto both = validArguments();
    both.insert(both.end(), {"--duration", "1"});
    CHECK(!parseCaptureArguments(both).ok());
    auto duplicate = validArguments();
    duplicate.insert(duplicate.end(), {"--gain", "21"});
    CHECK(!parseCaptureArguments(duplicate).ok());
    auto duplicateDevice = validArguments();
    duplicateDevice.insert(duplicateDevice.end(), {"--device", "serial=456"});
    CHECK(!parseCaptureArguments(duplicateDevice).ok());
}

void testInvalidNumericAndBoundsFail() {
    for (const auto& replacement : {"nan", "inf", "-1", "0"}) {
        auto arguments = validArguments();
        arguments[7] = replacement;
        CHECK(!parseCaptureArguments(arguments).ok());
    }
    auto excessive = validArguments();
    excessive[15] = "268435457";
    CHECK(!parseCaptureArguments(excessive).ok());
    auto timeout = validArguments();
    timeout[19] = "5001";
    CHECK(!parseCaptureArguments(timeout).ok());
}

void testMalformedSyntaxFails() {
    auto malformedDevice = validArguments();
    malformedDevice[1] = "driver";
    CHECK(!parseCaptureArguments(malformedDevice).ok());
    auto whitespaceKey = validArguments();
    whitespaceKey[1] = "driver key=fake";
    CHECK(!parseCaptureArguments(whitespaceKey).ok());
    auto unknown = validArguments();
    unknown.insert(unknown.end(), {"--mystery", "value"});
    CHECK(!parseCaptureArguments(unknown).ok());
    auto positional = validArguments();
    positional.push_back("stray");
    CHECK(!parseCaptureArguments(positional).ok());
}
} // namespace

int main() {
    const std::vector<std::pair<std::string, std::function<void()>>> tests{
        {"complete request", testCompleteRequest},
        {"help and version", testHelpAndVersionAreDeviceFree},
        {"missing and ambiguous", testMissingAndAmbiguousValuesFail},
        {"numeric and bounds", testInvalidNumericAndBoundsFail},
        {"malformed syntax", testMalformedSyntaxFails},
    };
    std::size_t failures = 0;
    for (const auto& [name, test] : tests) {
        try {
            test();
            std::cout << "PASS: " << name << '\n';
        } catch (const std::exception& exception) {
            ++failures;
            std::cerr << "FAIL: " << name << ": " << exception.what() << '\n';
        }
    }
    return failures == 0 ? 0 : 1;
}
