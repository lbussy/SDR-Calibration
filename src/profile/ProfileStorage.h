#pragma once

#include "profile/ProfileEngine.h"

#include <filesystem>

namespace sdrcal::profile {

enum class ReplacementStatus {
    success,
    validation_failed,
    write_failed,
    sync_failed,
    replace_failed
};
struct ReplacementResult {
    ReplacementStatus status = ReplacementStatus::validation_failed;
    std::string reason;
    [[nodiscard]] bool succeeded() const noexcept {
        return status == ReplacementStatus::success;
    }
};

[[nodiscard]] ReplacementResult replaceProfileAtomically(const std::filesystem::path& destination,
                                                         const CalibrationProfile& profile,
                                                         const VerifyHook& verifier = {},
                                                         bool require_signature = false);

} // namespace sdrcal::profile
