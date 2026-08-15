#pragma once

#include "capture/CaptureTypes.h"

#include <optional>
#include <string>
#include <vector>

namespace sdrcal::cli {

enum class CommandAction { capture, help, version };

struct ParseResult {
    CommandAction action = CommandAction::capture;
    std::optional<capture::CaptureRequest> request;
    std::vector<std::string> errors;

    [[nodiscard]] bool ok() const noexcept { return errors.empty(); }
};

[[nodiscard]] ParseResult parseCaptureArguments(const std::vector<std::string>& arguments);
[[nodiscard]] std::string captureUsage();

} // namespace sdrcal::cli
