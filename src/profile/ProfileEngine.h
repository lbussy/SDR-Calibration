#pragma once

#include "profile/ProfileTypes.h"

#include <functional>

namespace sdrcal::profile {

enum class CompatibilityStatus { supported, unsupported_schema, unsupported_required_capability };
struct CompatibilityResult {
    CompatibilityStatus status;
    std::string reason;
};

enum class ValidationCode {
    empty_value,
    invalid_value,
    duplicate_value,
    unsupported_schema,
    unsupported_capability,
    overlapping_segments,
    assurance_mismatch,
    invalid_integrity,
    invalid_adapter,
    invalid_revocation
};
struct ValidationIssue {
    ValidationCode code;
    std::string path;
    std::string message;
};
struct ValidationResult {
    std::vector<ValidationIssue> issues;
    [[nodiscard]] bool valid() const noexcept {
        return issues.empty();
    }
};

using SignHook =
    std::function<std::optional<std::string>(std::string_view payload, std::string_view key_id)>;
using VerifyHook = std::function<bool(std::string_view payload, std::string_view key_id,
                                      std::string_view signature)>;

[[nodiscard]] CompatibilityResult checkCompatibility(const CalibrationProfile& profile);
[[nodiscard]] ValidationResult validateProfile(const CalibrationProfile& profile,
                                               bool verify_digest = true);
[[nodiscard]] bool refreshIntegrity(CalibrationProfile& profile, std::string* error = nullptr);
[[nodiscard]] bool signProfile(CalibrationProfile& profile, std::string key_id,
                               const SignHook& signer, std::string* error = nullptr);
[[nodiscard]] bool verifyIntegrity(const CalibrationProfile& profile, const VerifyHook& verifier,
                                   bool require_signature, std::string* error = nullptr);

enum class EvaluationStatus {
    qualification_capable,
    calibrated,
    informational_only,
    profile_expired,
    identity_mismatch,
    configuration_mismatch,
    outside_frequency_domain,
    outside_temperature_domain,
    insufficient_warmup,
    unsupported_schema,
    unsupported_model,
    integrity_failure,
    revoked,
    assurance_requirement_not_met,
    uncertainty_requirement_not_met,
    invalid_request
};

struct EvaluationRequest {
    double indicated_frequency_hz = 0.0;
    std::optional<double> target_frequency_hz;
    DeviceIdentity device;
    DeviceConfiguration configuration;
    double temperature_c = 0.0;
    std::int64_t warmup_seconds = 0;
    std::string evaluated_at;
    int required_reliability_quotient = 90;
    std::optional<double> maximum_expanded_uncertainty_hz;
    bool require_verified_signature = false;
};

struct EvaluationResult {
    EvaluationStatus status = EvaluationStatus::invalid_request;
    std::string reason;
    std::string profile_id;
    std::string segment_id;
    double indicated_error_hz = 0.0;
    double estimated_true_frequency_hz = 0.0;
    double expanded_uncertainty_hz = 0.0;
    std::optional<double> target_offset_hz;
    [[nodiscard]] bool usable() const noexcept {
        return status == EvaluationStatus::qualification_capable ||
               status == EvaluationStatus::calibrated ||
               status == EvaluationStatus::informational_only;
    }
};

[[nodiscard]] EvaluationResult evaluateProfile(const CalibrationProfile& profile,
                                               const EvaluationRequest& request,
                                               const VerifyHook& verifier = {});

} // namespace sdrcal::profile
