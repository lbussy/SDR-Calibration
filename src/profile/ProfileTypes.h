#pragma once

#include "core/ReliabilityQuotient.h"

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace sdrcal::profile {

struct JsonValue;
using JsonArray = std::vector<JsonValue>;
using JsonObject = std::map<std::string, JsonValue>;

struct JsonValue {
    using Storage = std::variant<std::nullptr_t, bool, std::int64_t, double, std::string, JsonArray,
                                 JsonObject>;
    Storage value = nullptr;

    JsonValue() = default;
    JsonValue(std::nullptr_t) : value(nullptr) {}
    JsonValue(bool input) : value(input) {}
    JsonValue(std::int64_t input) : value(input) {}
    JsonValue(int input) : value(static_cast<std::int64_t>(input)) {}
    JsonValue(double input) : value(input) {}
    JsonValue(std::string input) : value(std::move(input)) {}
    JsonValue(const char* input) : value(std::string(input)) {}
    JsonValue(JsonArray input) : value(std::move(input)) {}
    JsonValue(JsonObject input) : value(std::move(input)) {}
};

enum class ProfileStatus { qualification_capable, calibrated, informational_only, revoked };
enum class IdentityStrength { hardware_serial, driver_serial, operator_assigned, insufficient };
enum class SegmentModelType { constant_ppm, linear };
enum class UncertaintyKind { standard, expanded };

struct DeviceIdentity {
    std::string driver;
    std::string manufacturer;
    std::string model;
    std::string identifier;
    IdentityStrength strength = IdentityStrength::insufficient;
};

struct DeviceConfiguration {
    std::string clock_source;
    std::int64_t sample_rate_hz = 0;
    std::optional<std::int64_t> bandwidth_hz;
    double frequency_correction_ppm = 0.0;
    std::optional<std::string> driver_version;
    std::optional<std::string> firmware_version;
    std::optional<std::string> antenna_port;
    std::optional<std::string> tuner_path;
    JsonObject binding_extension;
};

struct SegmentUncertainty {
    UncertaintyKind kind = UncertaintyKind::expanded;
    double coverage_factor = 0.0;
    std::optional<double> confidence_level_approximate;
    double base_hz = 0.0;
    double ppm_component = 0.0;
    std::vector<std::string> included_components;
    std::vector<std::string> excluded_components;
};

struct FrequencySegment {
    std::string segment_id;
    double minimum_frequency_hz = 0.0;
    double maximum_frequency_hz = 0.0;
    SegmentModelType model_type = SegmentModelType::linear;
    double error_ppm = 0.0;
    double reference_frequency_hz = 0.0;
    double intercept_error_hz = 0.0;
    double slope_ppm = 0.0;
    SegmentUncertainty uncertainty;
};

struct AssuranceComponentRecord {
    std::string name;
    int score = 0;
    std::string basis;
};

struct ProfileAssurance {
    std::string scoring_policy_version = "reliability-quotient-v1";
    int reliability_quotient = 0;
    core::ReferenceClass reference_class = core::ReferenceClass::unknown;
    int reference_score_ceiling = 0;
    std::vector<std::string> limiting_components;
    std::vector<AssuranceComponentRecord> components;
};

struct TemperatureValidity {
    double reference_c = 0.0;
    double minimum_c = 0.0;
    double maximum_c = 0.0;
    std::string measurement_location;
};

struct ProfileValidity {
    std::string calibrated_at;
    std::string not_valid_after;
    std::int64_t minimum_warmup_seconds = 0;
    TemperatureValidity temperature;
};

struct ReferenceRecord {
    std::string reference_id;
    core::ReferenceClass kind = core::ReferenceClass::unknown;
    double nominal_frequency_hz = 0.0;
    int assurance_score_ceiling = 0;
    std::optional<double> frequency_uncertainty_hz;
    std::optional<std::string> description;
    std::string evidence_sha256;
};

struct SoftwareRecord {
    std::string name;
    std::string version;
};
struct RegistryRecord {
    std::int64_t schema_version = 0;
    std::string registry_version;
    std::string sha256;
};
struct EvidenceBundleRecord {
    std::string media_type;
    std::string sha256;
    std::optional<std::string> location;
};

struct ProfileProvenance {
    std::string calibration_run_id;
    SoftwareRecord software;
    std::optional<RegistryRecord> reference_registry;
    std::vector<ReferenceRecord> reference_set;
    std::optional<EvidenceBundleRecord> evidence_bundle;
    std::vector<std::string> observation_ids;
};

struct SignatureRecord {
    std::string algorithm = "ed25519";
    std::string key_id;
    std::string value;
};
struct IntegrityRecord {
    std::string canonicalization = "RFC8785";
    std::string sha256;
    std::optional<SignatureRecord> signature;
};

struct AdapterCompatibility {
    std::string version_range;
    std::string mapping_test_id;
    bool mapping_verified = false;
};
struct AdapterRecord {
    std::string adapter_id;
    std::string direction;
    std::string application = "WSJT-X";
    std::string application_version;
    std::string created_at;
    std::string artifact_sha256;
    bool lossy = false;
    std::vector<std::string> omitted_capabilities;
    std::optional<AdapterCompatibility> compatibility;
    JsonObject metadata;
};

struct CalibrationProfile {
    std::string schema_name = "sdr-calibration-profile";
    std::string schema_version = "1.0.0";
    std::string profile_id;
    std::optional<std::string> supersedes_profile_id;
    ProfileStatus profile_status = ProfileStatus::informational_only;
    std::string created_at;
    DeviceIdentity device;
    DeviceConfiguration configuration;
    std::vector<FrequencySegment> segments;
    ProfileAssurance assurance;
    ProfileValidity validity;
    ProfileProvenance provenance;
    IntegrityRecord integrity;
    std::vector<AdapterRecord> adapters;
    JsonObject optional_extensions;
    std::vector<std::string> required_capabilities;
};

} // namespace sdrcal::profile
