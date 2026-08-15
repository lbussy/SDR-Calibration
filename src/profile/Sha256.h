#pragma once

#include <string>
#include <string_view>

namespace sdrcal::profile {

[[nodiscard]] std::string sha256Hex(std::string_view input);
[[nodiscard]] bool isSha256Hex(std::string_view input) noexcept;

} // namespace sdrcal::profile
