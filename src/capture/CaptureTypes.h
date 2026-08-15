#pragma once

#include <chrono>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace sdrcal::capture {

inline constexpr std::uint64_t kBytesPerComplexSample = 8;

enum class SettingPolicy { strict, permissive };
enum class SettingState {
    applied_verified,
    applied_changed,
    applied_unverified,
    unsupported,
    failed
};
enum class CaptureStatus { complete, partial, cancelled, failed };
enum class ErrorCategory {
    none,
    validation,
    effective_setting,
    timeout_limit,
    overflow,
    discontinuity,
    end_of_input,
    source_error,
    cancellation,
    raw_open,
    raw_write,
    raw_finalize,
    manifest_write,
    publication,
    cleanup
};
enum class FinalState { known_safe, known_open, unknown };
enum class ReadStatus { samples, timeout, overflow, discontinuity, end_of_input, cancelled, error };

struct ResourceLimits {
    double maximum_duration_seconds = 120.0;
    std::uint64_t maximum_sample_count = 268'435'456;
    std::uint64_t maximum_raw_bytes = 2'147'483'648;
    std::chrono::milliseconds default_read_timeout{100};
    std::chrono::milliseconds maximum_read_timeout{5'000};
    std::uint32_t maximum_consecutive_timeouts = 10;
};

struct NumericTolerance {
    double absolute = 1.0;
    double relative = 1.0e-6;
};

struct SettingTolerances {
    NumericTolerance center_frequency_hz{};
    NumericTolerance sample_rate_sps{};
    NumericTolerance bandwidth_hz{};
    NumericTolerance gain_db{0.1, 0.0};
};

struct CaptureRequest {
    std::map<std::string, std::string> device_arguments;
    std::optional<std::size_t> enumeration_index;
    std::size_t rx_channel = 0;
    double center_frequency_hz = 0.0;
    double sample_rate_sps = 0.0;
    std::optional<double> bandwidth_hz;
    std::optional<double> gain_db;
    std::optional<double> duration_seconds;
    std::optional<std::uint64_t> sample_count;
    std::filesystem::path output_path;
    std::chrono::milliseconds read_timeout{100};
    SettingPolicy setting_policy = SettingPolicy::strict;
    std::optional<std::string> purpose;
};

struct EffectiveSetting {
    std::optional<double> requested;
    std::optional<double> effective;
    SettingState state = SettingState::applied_unverified;
};

struct EffectiveSettings {
    EffectiveSetting center_frequency_hz;
    EffectiveSetting sample_rate_sps;
    EffectiveSetting bandwidth_hz;
    EffectiveSetting gain_db;
    std::optional<bool> automatic_gain;
    std::optional<double> frequency_correction_ppm;
};

struct DeviceMetadata {
    std::map<std::string, std::string> resolved_arguments;
    std::optional<std::string> driver_key;
    std::optional<std::string> hardware_key;
    std::optional<std::string> serial;
    std::map<std::string, std::string> hardware_info;
    std::optional<std::string> library_version;
    std::optional<std::string> api_version;
    std::optional<std::string> abi_version;
    std::optional<std::string> driver_version;
    std::optional<std::string> firmware_version;
    std::optional<std::string> antenna;
    std::optional<std::string> clock_source;
    std::optional<std::string> tuner_path;
};

struct CapturePlan {
    CaptureRequest request;
    EffectiveSettings effective;
    ResourceLimits limits;
    std::uint64_t target_samples = 0;
    std::uint64_t target_bytes = 0;
    std::string rounding_rule;
    std::string stream_format = "CF32";
    std::optional<std::size_t> stream_mtu;
};

struct CaptureError {
    ErrorCategory category = ErrorCategory::none;
    std::string message;
};

struct CleanupResult {
    bool attempted = false;
    bool succeeded = false;
    std::string detail;
    FinalState final_state = FinalState::unknown;
};

struct StreamStatistics {
    std::uint64_t target_samples = 0;
    std::uint64_t written_samples = 0;
    std::uint64_t written_bytes = 0;
    std::uint64_t read_calls = 0;
    std::uint64_t short_reads = 0;
    std::uint64_t timeouts = 0;
    std::uint64_t overflows = 0;
    std::uint64_t discontinuities = 0;
    bool timestamps_available = false;
    std::optional<std::int64_t> first_timestamp_ns;
    std::optional<std::int64_t> last_timestamp_ns;
};

struct CaptureResult {
    CaptureStatus status = CaptureStatus::failed;
    CaptureError error;
    StreamStatistics stream;
    CleanupResult source_cleanup;
    CleanupResult raw_cleanup;
    CleanupResult manifest_cleanup;
    FinalState final_state = FinalState::unknown;
    std::filesystem::path raw_path;
    std::filesystem::path manifest_path;
    bool atomic_raw_publication = false;
    bool atomic_manifest_publication = false;
    std::string utc_start;
    std::string utc_end;
    double monotonic_elapsed_seconds = 0.0;
};

struct ReadResult {
    ReadStatus status = ReadStatus::error;
    std::vector<std::complex<float>> samples;
    std::optional<std::int64_t> timestamp_ns;
    std::string detail;
};

struct ValidationResult {
    std::optional<CapturePlan> plan;
    std::vector<CaptureError> errors;

    [[nodiscard]] bool ok() const { return plan.has_value() && errors.empty(); }
};

} // namespace sdrcal::capture
