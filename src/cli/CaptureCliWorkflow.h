#pragma once

#include "cli/CaptureCliArguments.h"
#include "soapy/SoapyApi.h"

#include <functional>
#include <iosfwd>

namespace sdrcal::cli {

enum class ExitStatus : int { success = 0, usage = 2, preparation = 3, capture = 4 };

using CancellationCheck = std::function<bool()>;

[[nodiscard]] ExitStatus runCaptureCommand(
    const ParseResult& parsed,
    soapy::SoapyApi& api,
    std::ostream& output,
    std::ostream& error,
    CancellationCheck cancelled = {});

} // namespace sdrcal::cli
