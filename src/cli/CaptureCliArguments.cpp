#include "cli/CaptureCliArguments.h"

#include "capture/CapturePlan.h"

#include <algorithm>
#include <charconv>
#include <cctype>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <set>
#include <sstream>
#include <string_view>
#include <utility>

namespace sdrcal::cli {
namespace {

template <typename Integer>
bool parseInteger(std::string_view text, Integer& result) {
    if (text.empty() || text.front() == '-' || text.front() == '+') {
        return false;
    }
    const auto parsed = std::from_chars(text.data(), text.data() + text.size(), result);
    return parsed.ec == std::errc{} && parsed.ptr == text.data() + text.size();
}

bool parseDouble(std::string_view text, double& result) {
    if (text.empty()) {
        return false;
    }
    std::string owned(text);
    char* end = nullptr;
    errno = 0;
    result = std::strtod(owned.c_str(), &end);
    return errno != ERANGE && end == owned.c_str() + owned.size() && std::isfinite(result);
}

void addError(ParseResult& result, std::string message) {
    result.errors.push_back(std::move(message));
}

} // namespace

ParseResult parseCaptureArguments(const std::vector<std::string>& arguments) {
    ParseResult result;
    capture::CaptureRequest request;
    std::set<std::string> seen;

    for (std::size_t index = 0; index < arguments.size(); ++index) {
        const std::string& option = arguments[index];
        if (option == "--help") {
            if (arguments.size() != 1) {
                addError(result, "--help must be used alone");
            }
            result.action = CommandAction::help;
            continue;
        }
        if (option == "--version") {
            if (arguments.size() != 1) {
                addError(result, "--version must be used alone");
            }
            result.action = CommandAction::version;
            continue;
        }
        if (!option.starts_with("--")) {
            addError(result, "unexpected positional argument: " + option);
            continue;
        }
        if (index + 1 >= arguments.size()) {
            addError(result, "missing value for " + option);
            continue;
        }
        const std::string value = arguments[++index];
        if (value.starts_with("--")) {
            addError(result, "missing value for " + option);
            --index;
            continue;
        }

        const bool repeatable = option == "--device";
        if (!repeatable && !seen.insert(option).second) {
            addError(result, "duplicate option: " + option);
            continue;
        }

        if (option == "--device") {
            const auto separator = value.find('=');
            if (separator == std::string::npos || separator == 0 || separator + 1 >= value.size()) {
                addError(result, "--device requires a non-empty key=value");
                continue;
            }
            const std::string key = value.substr(0, separator);
            const std::string deviceValue = value.substr(separator + 1);
            const bool invalidKey = std::any_of(key.begin(), key.end(), [](const char character) {
                const auto byte = static_cast<unsigned char>(character);
                return std::isspace(byte) != 0 || std::iscntrl(byte) != 0 || character == '=';
            });
            const bool invalidValue =
                std::any_of(deviceValue.begin(), deviceValue.end(), [](const char character) {
                    return std::iscntrl(static_cast<unsigned char>(character)) != 0;
            });
            if (invalidKey || invalidValue) {
                addError(
                    result,
                    "--device key=value must not contain whitespace in the key or control "
                    "characters");
            } else if (!request.device_arguments.emplace(key, deviceValue).second) {
                addError(result, "duplicate device key: " + key);
            }
        } else if (option == "--index") {
            std::size_t parsed = 0;
            if (!parseInteger(value, parsed)) {
                addError(result, "--index requires a non-negative integer");
            } else {
                request.enumeration_index = parsed;
            }
        } else if (option == "--channel") {
            if (!parseInteger(value, request.rx_channel)) {
                addError(result, "--channel requires a non-negative integer");
            }
        } else if (option == "--frequency-hz") {
            if (!parseDouble(value, request.center_frequency_hz)) {
                addError(result, "--frequency-hz requires a finite number");
            }
        } else if (option == "--sample-rate") {
            if (!parseDouble(value, request.sample_rate_sps)) {
                addError(result, "--sample-rate requires a finite number");
            }
        } else if (option == "--bandwidth") {
            double parsed = 0.0;
            if (!parseDouble(value, parsed)) {
                addError(result, "--bandwidth requires a finite number");
            } else {
                request.bandwidth_hz = parsed;
            }
        } else if (option == "--gain") {
            double parsed = 0.0;
            if (!parseDouble(value, parsed)) {
                addError(result, "--gain requires a finite number");
            } else {
                request.gain_db = parsed;
            }
        } else if (option == "--duration") {
            double parsed = 0.0;
            if (!parseDouble(value, parsed)) {
                addError(result, "--duration requires a finite number");
            } else {
                request.duration_seconds = parsed;
            }
        } else if (option == "--samples") {
            std::uint64_t parsed = 0;
            if (!parseInteger(value, parsed)) {
                addError(result, "--samples requires a positive integer");
            } else {
                request.sample_count = parsed;
            }
        } else if (option == "--output") {
            request.output_path = value;
        } else if (option == "--timeout-ms") {
            std::uint64_t parsed = 0;
            if (!parseInteger(value, parsed) ||
                parsed > static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max())) {
                addError(result, "--timeout-ms requires a bounded positive integer");
            } else {
                request.read_timeout = std::chrono::milliseconds(parsed);
            }
        } else if (option == "--policy") {
            if (value == "strict") {
                request.setting_policy = capture::SettingPolicy::strict;
            } else if (value == "permissive") {
                request.setting_policy = capture::SettingPolicy::permissive;
            } else {
                addError(result, "--policy must be strict or permissive");
            }
        } else if (option == "--purpose") {
            if (value.empty()) {
                addError(result, "--purpose must not be empty");
            } else {
                request.purpose = value;
            }
        } else {
            addError(result, "unknown option: " + option);
        }
    }

    if (result.action != CommandAction::capture) {
        return result;
    }
    if (request.device_arguments.empty()) {
        addError(result, "at least one --device key=value selector is required");
    }
    for (const auto& error : capture::validateCaptureRequestBeforeDevice(request)) {
        addError(result, error.message);
    }
    if (result.errors.empty()) {
        result.request = std::move(request);
    }
    return result;
}

std::string captureUsage() {
    return R"(Usage: sdrcal-capture [options]

Required:
  --device KEY=VALUE       Explicit SoapySDR selection argument; repeatable
  --frequency-hz HZ        Requested RF center frequency
  --sample-rate SPS        Requested complex sample rate
  --duration SECONDS       Capture duration bound (choose one bound)
  --samples COUNT          Exact sample-count bound (choose one bound)
  --output PATH            Output basename for .cf32 and .capture.json

Optional:
  --index N                Select one match from this invocation (default: unique only)
  --channel N              RX channel (default: 0)
  --bandwidth HZ           Requested hardware bandwidth
  --gain DB                Requested aggregate manual RX gain
  --timeout-ms MS          Per-read timeout, 1 through 5000 (default: 100)
  --policy POLICY          strict or permissive (default: strict)
  --purpose TEXT           Non-semantic operator note
  --help                   Show this help without accessing a device
  --version                Show version without accessing a device

Existing complete artifacts are never overwritten. SIGINT requests cancellation.
)";
}

} // namespace sdrcal::cli
