#pragma once

#include "profile/ProfileTypes.h"

#include <string>
#include <string_view>

namespace sdrcal::profile {

struct CanonicalJsonResult {
    bool success = false;
    std::string value;
    std::string error;
};

struct JsonParseResult {
    bool success = false;
    JsonValue value;
    std::string error;
};

[[nodiscard]] CanonicalJsonResult canonicalizeJson(const JsonValue& value);
[[nodiscard]] JsonParseResult parseJson(std::string_view input);
[[nodiscard]] std::string serializeProfile(const CalibrationProfile& profile,
                                           bool include_integrity = true);
[[nodiscard]] JsonObject profileJsonObject(const CalibrationProfile& profile,
                                           bool include_integrity = true);

} // namespace sdrcal::profile
