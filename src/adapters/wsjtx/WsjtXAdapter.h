#pragma once

#include "profile/ProfileTypes.h"

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace sdrcal::wsjtx {

inline constexpr std::size_t maximum_input_bytes = 4U * 1024U * 1024U;
inline constexpr std::size_t maximum_fmt_lines = 100000U;

enum class Status {
    success,
    unsupported_version,
    malformed,
    ambiguous,
    resource_limit,
    unrepresentable,
    invalid_profile,
    unknown_format
};

struct LossReport {
    bool lossy = true;
    std::vector<std::string> omitted_capabilities;
};

struct SettingsProjection {
    double intercept_hz = 0.0;
    double slope_ppm = 0.0;
    std::string application_version;
    std::string configuration_name;
    std::string created_at;
    std::string artifact_sha256;
    std::string mapping_test_id;
    bool informational_only = true;
    LossReport loss;
};

struct SettingsResult {
    Status status = Status::malformed;
    std::string reason;
    std::optional<SettingsProjection> projection;
    std::string text;
    std::optional<profile::AdapterRecord> adapter_record;
    [[nodiscard]] bool succeeded() const noexcept {
        return status == Status::success;
    }
};

[[nodiscard]] SettingsResult importSettings(std::string_view text, std::string application_version,
                                            std::string configuration_name,
                                            std::string imported_at);
[[nodiscard]] SettingsResult exportSettings(const profile::CalibrationProfile& profile,
                                            std::string application_version,
                                            std::string configuration_name,
                                            std::string exported_at);

struct FmtObservation {
    std::string original_line;
    std::size_t line_number = 0;
    bool parsed = false;
    bool rejected_by_source = false;
    bool manual_review_required = false;
    std::string time;
    double frequency_khz = 0.0;
    int mode_flag = 0;
    double expected_audio_hz = 0.0;
    double measured_audio_hz = 0.0;
    double delta_hz = 0.0;
    double snr_db = 0.0;
    double metric = 0.0;
    std::string reason;
};

struct FmtResult {
    Status status = Status::malformed;
    std::string reason;
    std::string source_sha256;
    std::vector<FmtObservation> lines;
    std::optional<profile::AdapterRecord> adapter_record;
    [[nodiscard]] bool succeeded() const noexcept {
        return status == Status::success;
    }
};

[[nodiscard]] FmtResult importFmtAll(std::string_view text, std::string application_version,
                                     std::string imported_at);

} // namespace sdrcal::wsjtx
