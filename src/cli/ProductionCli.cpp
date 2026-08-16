#include "cli/ProductionCli.h"

#include "capture/MemoryAcquisition.h"
#include "profile/CanonicalJson.h"
#include "profile/Sha256.h"

#include <algorithm>
#include <bit>
#include <cmath>
#include <fstream>
#include <limits>
#include <set>
#include <sstream>
#include <system_error>
#ifndef _WIN32
#include <fcntl.h>
#include <unistd.h>
#endif

namespace sdrcal::cli {
namespace {
#ifndef SDRCAL_RECORDED_INPUT_MAX_BYTES
#define SDRCAL_RECORDED_INPUT_MAX_BYTES 2147483648
#endif
constexpr std::uint64_t kRecordedInputMaximumBytes = SDRCAL_RECORDED_INPUT_MAX_BYTES;
static_assert(kRecordedInputMaximumBytes > 0U && kRecordedInputMaximumBytes <= 2'147'483'648ULL);

using profile::JsonArray;
using profile::JsonObject;
using profile::JsonValue;

struct Reader {
    std::vector<std::string> errors;

    const JsonObject* object(const JsonValue& value, const std::string& path) {
        if (const auto* item = std::get_if<JsonObject>(&value.value))
            return item;
        errors.push_back(path + " must be an object");
        return nullptr;
    }
    const JsonArray* array(const JsonValue& value, const std::string& path) {
        if (const auto* item = std::get_if<JsonArray>(&value.value))
            return item;
        errors.push_back(path + " must be an array");
        return nullptr;
    }
    const JsonValue* member(const JsonObject& value, const std::string& name,
                            const std::string& path) {
        const auto found = value.find(name);
        if (found != value.end())
            return &found->second;
        errors.push_back(path + "." + name + " is required");
        return nullptr;
    }
    std::string string(const JsonObject& value, const std::string& name, const std::string& path) {
        const auto* item = member(value, name, path);
        if (item)
            if (const auto* text = std::get_if<std::string>(&item->value)) {
                if (!text->empty())
                    return *text;
            }
        errors.push_back(path + "." + name + " must be a non-empty string");
        return {};
    }
    double number(const JsonObject& value, const std::string& name, const std::string& path) {
        const auto* item = member(value, name, path);
        double result = std::numeric_limits<double>::quiet_NaN();
        if (item) {
            if (const auto* real = std::get_if<double>(&item->value))
                result = *real;
            if (const auto* integer = std::get_if<std::int64_t>(&item->value))
                result = static_cast<double>(*integer);
        }
        if (!std::isfinite(result))
            errors.push_back(path + "." + name + " must be finite");
        return result;
    }
    std::int64_t integer(const JsonObject& value, const std::string& name,
                         const std::string& path) {
        const auto* item = member(value, name, path);
        if (item)
            if (const auto* result = std::get_if<std::int64_t>(&item->value))
                return *result;
        errors.push_back(path + "." + name + " must be an integer");
        return 0;
    }
    bool boolean(const JsonObject& value, const std::string& name, const std::string& path) {
        const auto* item = member(value, name, path);
        if (item)
            if (const auto* result = std::get_if<bool>(&item->value))
                return *result;
        errors.push_back(path + "." + name + " must be a boolean");
        return false;
    }
    void members(const JsonObject& value, std::initializer_list<std::string_view> allowed,
                 const std::string& path) {
        for (const auto& [name, ignored] : value) {
            (void)ignored;
            if (std::ranges::none_of(allowed, [&](auto item) { return item == name; }))
                errors.push_back(path + "." + name + " is unknown");
        }
    }
};

core::ReferenceClass referenceClass(std::string_view value, Reader& reader,
                                    const std::string& path) {
    if (value == "authority_confirmed")
        return core::ReferenceClass::authority_confirmed;
    if (value == "derived_traceable")
        return core::ReferenceClass::derived_traceable;
    if (value == "locally_characterized")
        return core::ReferenceClass::locally_characterized;
    if (value == "ad_hoc")
        return core::ReferenceClass::ad_hoc;
    reader.errors.push_back(path + " has an unknown reference class");
    return core::ReferenceClass::unknown;
}

profile::DeviceConfiguration configuration(const JsonObject& object, Reader& reader,
                                           const std::string& path) {
    reader.members(object,
                   {"clock_source", "sample_rate_hz", "bandwidth_hz", "frequency_correction_ppm",
                    "driver_version", "firmware_version", "antenna_port", "tuner_path",
                    "binding_extension"},
                   path);
    profile::DeviceConfiguration result;
    result.clock_source = reader.string(object, "clock_source", path);
    result.sample_rate_hz = reader.integer(object, "sample_rate_hz", path);
    result.frequency_correction_ppm = reader.number(object, "frequency_correction_ppm", path);
    auto optionalInteger = [&](const char* name, std::optional<std::int64_t>& target) {
        if (object.contains(name))
            target = reader.integer(object, name, path);
    };
    optionalInteger("bandwidth_hz", result.bandwidth_hz);
    auto optionalString = [&](const char* name, std::optional<std::string>& target) {
        if (object.contains(name))
            target = reader.string(object, name, path);
    };
    optionalString("driver_version", result.driver_version);
    optionalString("firmware_version", result.firmware_version);
    optionalString("antenna_port", result.antenna_port);
    optionalString("tuner_path", result.tuner_path);
    if (object.contains("binding_extension")) {
        const auto* value = reader.member(object, "binding_extension", path);
        const auto* extension =
            value ? reader.object(*value, path + ".binding_extension") : nullptr;
        if (extension)
            result.binding_extension = *extension;
    }
    return result;
}

std::map<std::string, std::string> stringMap(const JsonObject& object, Reader& reader,
                                             const std::string& path) {
    std::map<std::string, std::string> result;
    for (const auto& [key, value] : object) {
        const auto* text = std::get_if<std::string>(&value.value);
        if (key.empty() || !text || text->empty())
            reader.errors.push_back(path + " must contain non-empty string keys and values");
        else
            result.emplace(key, *text);
    }
    return result;
}

std::vector<std::string> strings(const JsonObject& object, const char* name, Reader& reader,
                                 const std::string& path) {
    std::vector<std::string> result;
    const auto* value = reader.member(object, name, path);
    const auto* array = value ? reader.array(*value, path + "." + name) : nullptr;
    if (!array)
        return result;
    for (std::size_t i = 0; i < array->size(); ++i) {
        if (const auto* item = std::get_if<std::string>(&(*array)[i].value); item && !item->empty())
            result.push_back(*item);
        else
            reader.errors.push_back(path + "." + name + " entries must be non-empty strings");
    }
    return result;
}

reference::ReferenceEntry referenceEntry(const JsonObject& object, Reader& reader,
                                         const std::string& path) {
    reader.members(object,
                   {"reference_id", "reference_class", "assurance_ceiling", "nominal_frequency_hz",
                    "frequency_uncertainty_hz", "operating_status", "location_or_connection",
                    "conditions", "limitations", "evidence"},
                   path);
    reference::ReferenceEntry result;
    result.reference_id = reader.string(object, "reference_id", path);
    result.reference_class = referenceClass(reader.string(object, "reference_class", path), reader,
                                            path + ".reference_class");
    result.assurance_ceiling = static_cast<int>(reader.integer(object, "assurance_ceiling", path));
    result.nominal_frequency_hz = reader.number(object, "nominal_frequency_hz", path);
    if (object.contains("frequency_uncertainty_hz"))
        result.frequency_uncertainty_hz = reader.number(object, "frequency_uncertainty_hz", path);
    const auto status = reader.string(object, "operating_status", path);
    if (status == "active")
        result.operating_status = reference::OperatingStatus::active;
    else if (status == "intermittent")
        result.operating_status = reference::OperatingStatus::intermittent;
    else if (status == "inactive")
        result.operating_status = reference::OperatingStatus::inactive;
    else
        reader.errors.push_back(path + ".operating_status is unknown");
    result.location_or_connection = reader.string(object, "location_or_connection", path);
    result.conditions = strings(object, "conditions", reader, path);
    result.limitations = strings(object, "limitations", reader, path);
    const auto* evidenceValue = reader.member(object, "evidence", path);
    const auto* evidenceArray =
        evidenceValue ? reader.array(*evidenceValue, path + ".evidence") : nullptr;
    if (evidenceArray)
        for (std::size_t i = 0; i < evidenceArray->size(); ++i) {
            const std::string itemPath = path + ".evidence[" + std::to_string(i) + "]";
            const auto* item = reader.object((*evidenceArray)[i], itemPath);
            if (!item)
                continue;
            reader.members(*item,
                           {"source_id", "description", "retrieved_at", "expires_at", "sha256"},
                           itemPath);
            reference::EvidenceSource source;
            source.source_id = reader.string(*item, "source_id", itemPath);
            source.description = reader.string(*item, "description", itemPath);
            source.retrieved_at = reader.string(*item, "retrieved_at", itemPath);
            if (item->contains("expires_at"))
                source.expires_at = reader.string(*item, "expires_at", itemPath);
            source.sha256 = reader.string(*item, "sha256", itemPath);
            result.evidence.push_back(std::move(source));
        }
    return result;
}

std::string readText(const std::filesystem::path& path, std::string& error,
                     std::uint64_t limit = 8U * 1024U * 1024U) {
    std::error_code ec;
    const auto size = std::filesystem::file_size(path, ec);
    if (ec || size > limit) {
        error = "request file is unavailable or exceeds the byte limit";
        return {};
    }
    std::ifstream stream(path, std::ios::binary);
    std::string result(static_cast<std::size_t>(size), '\0');
    if (!stream.read(result.data(), static_cast<std::streamsize>(result.size()))) {
        error = "request file could not be read";
        return {};
    }
    return result;
}

class RecordedBoundary final : public application::DeviceWorkflowBoundary {
  public:
    RecordedBoundary(const ProductRequest& request, ProductCancellationCheck cancel,
                     std::string& inputError)
        : product_(request), cancel_(std::move(cancel)), input_error_(inputError) {}
    std::vector<application::DeviceCandidate> discover() override {
        return {product_.device};
    }
    application::AcquisitionResult acquire(const application::DeviceCandidate& device,
                                           const profile::DeviceConfiguration&,
                                           const application::ObservationRequest& request,
                                           const std::function<bool()>&) override {
        application::AcquisitionResult result;
        result.identity = device.identity;
        result.configuration = device.configuration;
        const auto found = std::ranges::find_if(product_.observations, [&](const auto& value) {
            return value.request.observation_id == request.observation_id;
        });
        if (found == product_.observations.end()) {
            result.reason = "recorded observation is missing";
            return result;
        }
        if (cancel_ && cancel_()) {
            result.status = application::AcquisitionStatus::cancelled;
            return result;
        }
        std::error_code ec;
        const auto size = std::filesystem::file_size(found->input_path, ec);
        if (ec || size == 0 || size > found->maximum_bytes || size % 8U != 0U ||
            size / 8U != found->sample_count) {
            input_error_ = "recorded input is unavailable, empty, oversized, or not CF32LE aligned";
            result.reason = input_error_;
            return result;
        }
        std::ifstream stream(found->input_path, std::ios::binary);
        std::string bytes(static_cast<std::size_t>(size), '\0');
        if (!stream.read(bytes.data(), static_cast<std::streamsize>(bytes.size())) ||
            profile::sha256Hex(bytes) != found->sha256) {
            input_error_ = "recorded input read or SHA-256 verification failed";
            result.reason = input_error_;
            return result;
        }
        result.samples.reserve(bytes.size() / 8U);
        for (std::size_t offset = 0; offset < bytes.size(); offset += 8U) {
            std::uint32_t realBits = 0, imaginaryBits = 0;
            for (unsigned int index = 0; index < 4U; ++index) {
                realBits |=
                    static_cast<std::uint32_t>(static_cast<unsigned char>(bytes[offset + index]))
                    << (8U * index);
                imaginaryBits |= static_cast<std::uint32_t>(
                                     static_cast<unsigned char>(bytes[offset + 4U + index]))
                                 << (8U * index);
            }
            const float real = std::bit_cast<float>(realBits);
            const float imaginary = std::bit_cast<float>(imaginaryBits);
            if (!std::isfinite(real) || !std::isfinite(imaginary)) {
                input_error_ = "recorded input contains a non-finite sample";
                result.reason = input_error_;
                return result;
            }
            result.samples.emplace_back(real, imaginary);
        }
        result.carrier_estimate = core::estimateCarrier(
            result.samples, static_cast<double>(device.configuration.sample_rate_hz),
            product_.workflow.estimator_options);
        result.status = application::AcquisitionStatus::success;
        result.effective_indicated_center_frequency_hz =
            found->effective_indicated_center_frequency_hz;
        result.effective_indicated_center_verified = true;
        result.sample_rate_sps = static_cast<double>(device.configuration.sample_rate_hz);
        result.duration_seconds = found->duration_seconds;
        result.signal_to_noise_db = found->signal_to_noise_db;
        result.clipped_samples = found->clipped_samples;
        result.missing_samples = found->missing_samples;
        result.discontinuities = found->discontinuities;
        result.frequency_instability_hz = found->frequency_instability_hz;
        result.interference_to_carrier_db = found->interference_to_carrier_db;
        result.effective_configuration = core::EffectiveConfigurationValidity::verified;
        result.reference_conditions_met = found->reference_conditions_met;
        result.final_device_state_safe = true;
        return result;
    }

  private:
    const ProductRequest& product_;
    ProductCancellationCheck cancel_;
    std::string& input_error_;
};

std::string terminalJson(std::string status, int exitCode, std::string reason = {},
                         const std::vector<std::string>& artifacts = {}) {
    JsonArray files;
    for (const auto& file : artifacts)
        files.emplace_back(file);
    const auto encoded = profile::canonicalizeJson(JsonObject{{"artifacts", files},
                                                              {"exit_status", exitCode},
                                                              {"reason", reason},
                                                              {"schema_name", "sdrcal-cli-result"},
                                                              {"schema_version", "1.0.0"},
                                                              {"status", std::move(status)}});
    return encoded.success ? encoded.value + "\n" : "{}\n";
}

bool writeFile(const std::filesystem::path& path, std::string_view contents) {
    std::ofstream stream(path, std::ios::binary | std::ios::trunc);
    if (!stream.write(contents.data(), static_cast<std::streamsize>(contents.size())).good() ||
        !stream.flush().good())
        return false;
    stream.close();
#ifndef _WIN32
    const int descriptor = ::open(path.c_str(), O_RDONLY);
    if (descriptor < 0)
        return false;
    const bool synced = ::fsync(descriptor) == 0;
    (void)::close(descriptor);
    return synced;
#else
    return true;
#endif
}
} // namespace

ProductArguments parseProductArguments(const std::vector<std::string>& arguments) {
    ProductArguments result;
    if (arguments.size() == 1U && arguments[0] == "--help") {
        result.action = ProductAction::help;
        return result;
    }
    if (arguments.size() == 1U && arguments[0] == "--version") {
        result.action = ProductAction::version;
        return result;
    }
    if (arguments.empty() || arguments[0] != "calibrate") {
        result.errors.push_back("expected calibrate, --help, or --version");
        return result;
    }
    std::set<std::string> seen;
    for (std::size_t index = 1; index < arguments.size(); ++index) {
        const auto option = arguments[index];
        if ((option != "--request" && option != "--trust-file" && option != "--output-dir") ||
            index + 1 >= arguments.size()) {
            result.errors.push_back("unknown option or missing value: " + option);
            continue;
        }
        if (!seen.insert(option).second) {
            result.errors.push_back("duplicate option: " + option);
            ++index;
            continue;
        }
        const auto value = std::filesystem::path(arguments[++index]);
        if (value.empty())
            result.errors.push_back("empty path for " + option);
        else if (option == "--request")
            result.request_path = value;
        else if (option == "--trust-file")
            result.trust_path = value;
        else
            result.output_directory = value;
    }
    if (result.request_path.empty())
        result.errors.push_back("--request is required");
    if (result.trust_path.empty())
        result.errors.push_back("--trust-file is required");
    if (result.output_directory.empty())
        result.errors.push_back("--output-dir is required");
    return result;
}

ProductRequestResult parseProductRequest(std::string_view json,
                                         const std::filesystem::path& baseDirectory,
                                         std::map<std::string, std::string> trustedSignatures) {
    ProductRequestResult result;
    const auto parsed = profile::parseJson(json);
    if (!parsed.success) {
        result.errors.push_back("invalid JSON: " + parsed.error);
        return result;
    }
    Reader reader;
    const auto* root = reader.object(parsed.value, "$ ");
    if (!root) {
        result.errors = std::move(reader.errors);
        return result;
    }
    const auto schemaName = reader.string(*root, "schema_name", "$");
    const bool liveMode = schemaName == "sdrcal-live-calibration-request";
    const bool recordedMode = schemaName == "sdrcal-recorded-calibration-request";
    if (liveMode) {
        reader.members(*root,
                       {"schema_name",
                        "schema_version",
                        "profile_id",
                        "calibration_run_id",
                        "created_at",
                        "not_valid_after",
                        "software_version",
                        "device",
                        "requested_configuration",
                        "live_acquisition",
                        "minimum_warmup_seconds",
                        "temperature",
                        "registry",
                        "observations",
                        "uncertainty",
                        "assurance_components",
                        "qualification_threshold",
                        "estimator_policy",
                        "acceptance_policy",
                        "evidence",
                        "interoperability"},
                       "$");
    } else {
        reader.members(*root,
                       {"schema_name",
                        "schema_version",
                        "profile_id",
                        "calibration_run_id",
                        "created_at",
                        "not_valid_after",
                        "software_version",
                        "device",
                        "requested_configuration",
                        "minimum_warmup_seconds",
                        "temperature",
                        "registry",
                        "observations",
                        "uncertainty",
                        "assurance_components",
                        "qualification_threshold",
                        "estimator_policy",
                        "acceptance_policy",
                        "evidence",
                        "interoperability"},
                       "$");
    }
    if (!liveMode && !recordedMode)
        reader.errors.push_back("$.schema_name is unsupported");
    if (reader.string(*root, "schema_version", "$") != "1.0.0")
        reader.errors.push_back("$.schema_version is unsupported");
    ProductRequest product;
    product.input_mode = liveMode ? ProductInputMode::live : ProductInputMode::recorded;
    auto& workflow = product.workflow;
    workflow.profile_id = reader.string(*root, "profile_id", "$");
    workflow.calibration_run_id = reader.string(*root, "calibration_run_id", "$");
    workflow.created_at = reader.string(*root, "created_at", "$");
    workflow.not_valid_after = reader.string(*root, "not_valid_after", "$");
    workflow.software_version = reader.string(*root, "software_version", "$");
    workflow.minimum_warmup_seconds = reader.integer(*root, "minimum_warmup_seconds", "$");
    workflow.qualification_threshold =
        static_cast<int>(reader.integer(*root, "qualification_threshold", "$"));
    const auto* deviceValue = reader.member(*root, "device", "$");
    const auto* device = deviceValue ? reader.object(*deviceValue, "$.device") : nullptr;
    if (device) {
        reader.members(*device,
                       {"driver", "manufacturer", "model", "identifier", "identity_strength",
                        "effective_configuration"},
                       "$.device");
        product.device.identity.driver = reader.string(*device, "driver", "$.device");
        product.device.identity.manufacturer = reader.string(*device, "manufacturer", "$.device");
        product.device.identity.model = reader.string(*device, "model", "$.device");
        product.device.identity.identifier = reader.string(*device, "identifier", "$.device");
        const auto strength = reader.string(*device, "identity_strength", "$.device");
        if (strength == "hardware_serial")
            product.device.identity.strength = profile::IdentityStrength::hardware_serial;
        else if (strength == "driver_serial")
            product.device.identity.strength = profile::IdentityStrength::driver_serial;
        else if (strength == "operator_assigned")
            product.device.identity.strength = profile::IdentityStrength::operator_assigned;
        else
            reader.errors.push_back("$.device.identity_strength is unsupported");
        const auto* configValue = reader.member(*device, "effective_configuration", "$.device");
        const auto* config =
            configValue ? reader.object(*configValue, "$.device.effective_configuration") : nullptr;
        if (config)
            product.device.configuration =
                configuration(*config, reader, "$.device.effective_configuration");
        workflow.selected_device_identifier = product.device.identity.identifier;
    }
    const auto* requestedValue = reader.member(*root, "requested_configuration", "$");
    const auto* requested =
        requestedValue ? reader.object(*requestedValue, "$.requested_configuration") : nullptr;
    if (requested)
        workflow.requested_configuration =
            configuration(*requested, reader, "$.requested_configuration");
    if (liveMode) {
        LiveAcquisitionRequest live;
        const auto* acquisitionValue = reader.member(*root, "live_acquisition", "$");
        const auto* acquisition =
            acquisitionValue ? reader.object(*acquisitionValue, "$.live_acquisition") : nullptr;
        if (acquisition) {
            reader.members(*acquisition,
                           {"device_arguments", "rx_channel", "gain_db", "duration_seconds",
                            "sample_count", "read_timeout_ms", "maximum_memory_bytes"},
                           "$.live_acquisition");
            const auto* argumentsValue =
                reader.member(*acquisition, "device_arguments", "$.live_acquisition");
            const auto* arguments =
                argumentsValue
                    ? reader.object(*argumentsValue, "$.live_acquisition.device_arguments")
                    : nullptr;
            if (arguments)
                live.device_arguments =
                    stringMap(*arguments, reader, "$.live_acquisition.device_arguments");
            if (live.device_arguments.empty())
                reader.errors.push_back(
                    "$.live_acquisition.device_arguments must select one stable device");
            const auto channel = reader.integer(*acquisition, "rx_channel", "$.live_acquisition");
            if (channel < 0 ||
                static_cast<std::uint64_t>(channel) >
                    static_cast<std::uint64_t>(std::numeric_limits<std::size_t>::max()))
                reader.errors.push_back(
                    "$.live_acquisition.rx_channel must be non-negative and representable");
            else
                live.rx_channel = static_cast<std::size_t>(channel);
            if (acquisition->contains("gain_db"))
                live.gain_db = reader.number(*acquisition, "gain_db", "$.live_acquisition");
            const bool hasDuration = acquisition->contains("duration_seconds");
            const bool hasSamples = acquisition->contains("sample_count");
            if (hasDuration == hasSamples) {
                reader.errors.push_back(
                    "$.live_acquisition requires exactly one duration_seconds or sample_count");
            } else if (hasDuration) {
                const double duration =
                    reader.number(*acquisition, "duration_seconds", "$.live_acquisition");
                if (duration <= 0.0)
                    reader.errors.push_back("$.live_acquisition.duration_seconds must be positive");
                else
                    live.duration_seconds = duration;
            } else {
                const auto samples =
                    reader.integer(*acquisition, "sample_count", "$.live_acquisition");
                if (samples <= 0)
                    reader.errors.push_back("$.live_acquisition.sample_count must be positive");
                else
                    live.sample_count = static_cast<std::uint64_t>(samples);
            }
            const auto timeout =
                reader.integer(*acquisition, "read_timeout_ms", "$.live_acquisition");
            if (timeout <= 0 || timeout > std::chrono::milliseconds::max().count())
                reader.errors.push_back(
                    "$.live_acquisition.read_timeout_ms must be positive and representable");
            else
                live.read_timeout = std::chrono::milliseconds(timeout);
            const auto memory =
                reader.integer(*acquisition, "maximum_memory_bytes", "$.live_acquisition");
            if (memory <= 0)
                reader.errors.push_back("$.live_acquisition.maximum_memory_bytes must be positive");
            else
                live.maximum_memory_bytes = static_cast<std::uint64_t>(memory);
        }
        product.live_acquisition = std::move(live);
        workflow.evidence.metadata.push_back({"input_mode", "live"});
    } else {
        workflow.evidence.metadata.push_back({"input_mode", "recorded"});
    }
    const auto* temperatureValue = reader.member(*root, "temperature", "$");
    const auto* temperature =
        temperatureValue ? reader.object(*temperatureValue, "$.temperature") : nullptr;
    if (temperature) {
        reader.members(*temperature,
                       {"reference_c", "minimum_c", "maximum_c", "measurement_location"},
                       "$.temperature");
        workflow.temperature.reference_c =
            reader.number(*temperature, "reference_c", "$.temperature");
        workflow.temperature.minimum_c = reader.number(*temperature, "minimum_c", "$.temperature");
        workflow.temperature.maximum_c = reader.number(*temperature, "maximum_c", "$.temperature");
        workflow.temperature.measurement_location =
            reader.string(*temperature, "measurement_location", "$.temperature");
    }
    const auto* registryValue = reader.member(*root, "registry", "$");
    const auto* registry = registryValue ? reader.object(*registryValue, "$.registry") : nullptr;
    if (registry) {
        reader.members(*registry,
                       {"schema_name", "schema_version", "registry_id", "registry_version",
                        "generated_at", "expires_at", "provenance", "references", "integrity"},
                       "$.registry");
        workflow.global_registry.schema_name =
            reader.string(*registry, "schema_name", "$.registry");
        workflow.global_registry.schema_version =
            reader.string(*registry, "schema_version", "$.registry");
        workflow.global_registry.registry_id =
            reader.string(*registry, "registry_id", "$.registry");
        workflow.global_registry.registry_version =
            reader.string(*registry, "registry_version", "$.registry");
        workflow.global_registry.generated_at =
            reader.string(*registry, "generated_at", "$.registry");
        if (registry->contains("expires_at"))
            workflow.global_registry.expires_at =
                reader.string(*registry, "expires_at", "$.registry");
        workflow.global_registry.provenance = reader.string(*registry, "provenance", "$.registry");
        const auto* referencesValue = reader.member(*registry, "references", "$.registry");
        const auto* references =
            referencesValue ? reader.array(*referencesValue, "$.registry.references") : nullptr;
        if (references)
            for (std::size_t i = 0; i < references->size(); ++i) {
                const auto path = "$.registry.references[" + std::to_string(i) + "]";
                if (const auto* item = reader.object((*references)[i], path))
                    workflow.global_registry.references.push_back(
                        referenceEntry(*item, reader, path));
            }
        const auto* integrityValue = reader.member(*registry, "integrity", "$.registry");
        const auto* integrity =
            integrityValue ? reader.object(*integrityValue, "$.registry.integrity") : nullptr;
        if (integrity) {
            reader.members(*integrity, {"canonicalization", "sha256", "signature"},
                           "$.registry.integrity");
            workflow.global_registry.integrity.canonicalization =
                reader.string(*integrity, "canonicalization", "$.registry.integrity");
            workflow.global_registry.integrity.sha256 =
                reader.string(*integrity, "sha256", "$.registry.integrity");
            const auto* signatureValue =
                reader.member(*integrity, "signature", "$.registry.integrity");
            const auto* signature =
                signatureValue ? reader.object(*signatureValue, "$.registry.integrity.signature")
                               : nullptr;
            if (signature) {
                reader.members(*signature, {"algorithm", "key_id", "value"},
                               "$.registry.integrity.signature");
                workflow.global_registry.integrity.signature = profile::SignatureRecord{
                    reader.string(*signature, "algorithm", "$.registry.integrity.signature"),
                    reader.string(*signature, "key_id", "$.registry.integrity.signature"),
                    reader.string(*signature, "value", "$.registry.integrity.signature")};
            }
        }
    }
    product.trusted_registry_signatures = std::move(trustedSignatures);
    if (product.trusted_registry_signatures.empty())
        reader.errors.push_back("the independently loaded trust file must not be empty");
    workflow.registry_verifier =
        [trusted = product.trusted_registry_signatures](std::string_view, std::string_view key,
                                                        std::string_view signature) {
            const auto found = trusted.find(std::string(key));
            return found != trusted.end() && found->second == signature;
        };
    const auto* observationsValue = reader.member(*root, "observations", "$");
    const auto* observations =
        observationsValue ? reader.array(*observationsValue, "$.observations") : nullptr;
    if (observations)
        for (std::size_t i = 0; i < observations->size(); ++i) {
            const auto path = "$.observations[" + std::to_string(i) + "]";
            const auto* item = reader.object((*observations)[i], path);
            if (!item)
                continue;
            if (liveMode) {
                reader.members(*item,
                               {"observation_id", "independence_id", "reference_id",
                                "indicated_center_frequency_hz"},
                               path);
                application::ObservationRequest liveObservation;
                liveObservation.observation_id = reader.string(*item, "observation_id", path);
                liveObservation.independence_id = reader.string(*item, "independence_id", path);
                liveObservation.reference_id = reader.string(*item, "reference_id", path);
                liveObservation.indicated_center_frequency_hz =
                    reader.number(*item, "indicated_center_frequency_hz", path);
                workflow.observations.push_back(std::move(liveObservation));
                continue;
            }
            reader.members(*item,
                           {"observation_id", "independence_id", "reference_id",
                            "indicated_center_frequency_hz",
                            "effective_indicated_center_frequency_hz", "input_path", "sha256",
                            "maximum_bytes", "sample_count", "duration_seconds",
                            "signal_to_noise_db", "clipped_samples", "missing_samples",
                            "discontinuities", "frequency_instability_hz",
                            "interference_to_carrier_db", "reference_conditions_met"},
                           path);
            RecordedObservation record;
            record.request.observation_id = reader.string(*item, "observation_id", path);
            record.request.independence_id = reader.string(*item, "independence_id", path);
            record.request.reference_id = reader.string(*item, "reference_id", path);
            record.request.indicated_center_frequency_hz =
                reader.number(*item, "indicated_center_frequency_hz", path);
            record.effective_indicated_center_frequency_hz =
                reader.number(*item, "effective_indicated_center_frequency_hz", path);
            const auto input = std::filesystem::path(reader.string(*item, "input_path", path));
            const bool escapes =
                std::ranges::any_of(input, [](const auto& component) { return component == ".."; });
            if (input.empty() || input.is_absolute() || escapes)
                reader.errors.push_back(path + ".input_path must be a contained relative path");
            else
                record.input_path = (baseDirectory / input).lexically_normal();
            record.sha256 = reader.string(*item, "sha256", path);
            if (!profile::isSha256Hex(record.sha256))
                reader.errors.push_back(path + ".sha256 is invalid");
            const auto maximumBytes = reader.integer(*item, "maximum_bytes", path);
            if (maximumBytes <= 0 ||
                static_cast<std::uint64_t>(maximumBytes) > kRecordedInputMaximumBytes)
                reader.errors.push_back(path + ".maximum_bytes must be from 1 through " +
                                        std::to_string(kRecordedInputMaximumBytes) +
                                        " for this build's recorded-input resource policy");
            else
                record.maximum_bytes = static_cast<std::uint64_t>(maximumBytes);
            const auto sampleCount = reader.integer(*item, "sample_count", path);
            if (sampleCount <= 0)
                reader.errors.push_back(path + ".sample_count must be positive");
            else
                record.sample_count = static_cast<std::uint64_t>(sampleCount);
            record.duration_seconds = reader.number(*item, "duration_seconds", path);
            record.signal_to_noise_db = reader.number(*item, "signal_to_noise_db", path);
            auto count = [&](const char* name) {
                const auto value = reader.integer(*item, name, path);
                if (value < 0)
                    reader.errors.push_back(path + "." + name + " must be non-negative");
                return value < 0 ? std::uint64_t{0} : static_cast<std::uint64_t>(value);
            };
            record.clipped_samples = count("clipped_samples");
            record.missing_samples = count("missing_samples");
            record.discontinuities = count("discontinuities");
            record.frequency_instability_hz =
                reader.number(*item, "frequency_instability_hz", path);
            record.interference_to_carrier_db =
                reader.number(*item, "interference_to_carrier_db", path);
            record.reference_conditions_met =
                reader.boolean(*item, "reference_conditions_met", path);
            workflow.observations.push_back(record.request);
            product.observations.push_back(std::move(record));
        }
    const auto* uncertaintyValue = reader.member(*root, "uncertainty", "$");
    const auto* uncertainty =
        uncertaintyValue ? reader.object(*uncertaintyValue, "$.uncertainty") : nullptr;
    if (uncertainty) {
        reader.members(*uncertainty,
                       {"version", "coverage_factor", "components", "correlations",
                        "excluded_or_unknown_material_components"},
                       "$.uncertainty");
        workflow.uncertainty_budget.version =
            reader.string(*uncertainty, "version", "$.uncertainty");
        workflow.uncertainty_budget.coverage_factor =
            reader.number(*uncertainty, "coverage_factor", "$.uncertainty");
        workflow.uncertainty_budget.excluded_or_unknown_material_components = strings(
            *uncertainty, "excluded_or_unknown_material_components", reader, "$.uncertainty");
        const auto* componentsValue = reader.member(*uncertainty, "components", "$.uncertainty");
        const auto* components =
            componentsValue ? reader.array(*componentsValue, "$.uncertainty.components") : nullptr;
        if (components)
            for (std::size_t i = 0; i < components->size(); ++i) {
                const auto path = "$.uncertainty.components[" + std::to_string(i) + "]";
                if (const auto* item = reader.object((*components)[i], path)) {
                    reader.members(*item,
                                   {"name", "standard_uncertainty_hz", "sensitivity_coefficient"},
                                   path);
                    workflow.uncertainty_budget.components.push_back(
                        {reader.string(*item, "name", path),
                         reader.number(*item, "standard_uncertainty_hz", path),
                         reader.number(*item, "sensitivity_coefficient", path)});
                }
            }
        const auto* correlationsValue =
            reader.member(*uncertainty, "correlations", "$.uncertainty");
        const auto* correlations =
            correlationsValue ? reader.array(*correlationsValue, "$.uncertainty.correlations")
                              : nullptr;
        if (correlations)
            for (std::size_t i = 0; i < correlations->size(); ++i) {
                const auto path = "$.uncertainty.correlations[" + std::to_string(i) + "]";
                if (const auto* item = reader.object((*correlations)[i], path)) {
                    reader.members(*item, {"first_component", "second_component", "coefficient"},
                                   path);
                    const auto first = reader.integer(*item, "first_component", path);
                    const auto second = reader.integer(*item, "second_component", path);
                    if (first < 0 || second < 0)
                        reader.errors.push_back(path + " indexes must be non-negative");
                    else
                        workflow.uncertainty_budget.correlations.push_back(
                            {static_cast<std::size_t>(first), static_cast<std::size_t>(second),
                             reader.number(*item, "coefficient", path)});
                }
            }
    }
    const auto* assuranceValue = reader.member(*root, "assurance_components", "$");
    const auto* assurance =
        assuranceValue ? reader.array(*assuranceValue, "$.assurance_components") : nullptr;
    if (assurance)
        for (std::size_t i = 0; i < assurance->size(); ++i) {
            const auto path = "$.assurance_components[" + std::to_string(i) + "]";
            if (const auto* item = reader.object((*assurance)[i], path)) {
                reader.members(*item, {"name", "score", "basis"}, path);
                workflow.assurance_components.push_back(
                    {reader.string(*item, "name", path),
                     static_cast<int>(reader.integer(*item, "score", path)),
                     reader.string(*item, "basis", path)});
            }
        }
    const auto* evidenceValue = reader.member(*root, "evidence", "$");
    const auto* evidence = evidenceValue ? reader.object(*evidenceValue, "$.evidence") : nullptr;
    if (evidence) {
        reader.members(
            *evidence,
            {"bundle_id", "monotonic_duration_ms", "maximum_bundle_bytes", "maximum_duration_ms"},
            "$.evidence");
        workflow.evidence.bundle_id = reader.string(*evidence, "bundle_id", "$.evidence");
        workflow.evidence.monotonic_duration_ms =
            reader.integer(*evidence, "monotonic_duration_ms", "$.evidence");
        workflow.evidence.maximum_bundle_bytes = static_cast<std::uint64_t>(
            reader.integer(*evidence, "maximum_bundle_bytes", "$.evidence"));
        workflow.evidence.maximum_duration_ms =
            reader.integer(*evidence, "maximum_duration_ms", "$.evidence");
        workflow.evidence.created_at = workflow.created_at;
        workflow.evidence.source_identity = workflow.calibration_run_id;
    }
    if (root->contains("interoperability")) {
        const auto* value = reader.member(*root, "interoperability", "$");
        const auto* object = value ? reader.object(*value, "$.interoperability") : nullptr;
        if (object) {
            reader.members(*object, {"wsjtx_version", "configuration_name"}, "$.interoperability");
            workflow.interoperability = application::InteroperabilityRequest{
                reader.string(*object, "wsjtx_version", "$.interoperability"),
                reader.string(*object, "configuration_name", "$.interoperability")};
        }
    }
    const auto* estimatorValue = reader.member(*root, "estimator_policy", "$");
    const auto* estimator =
        estimatorValue ? reader.object(*estimatorValue, "$.estimator_policy") : nullptr;
    if (estimator) {
        reader.members(*estimator,
                       {"minimum_samples", "minimum_mean_power", "minimum_model_coherence",
                        "maximum_absolute_frequency_fraction", "robust_iterations",
                        "huber_threshold_radians"},
                       "$.estimator_policy");
        const auto minimumSamples =
            reader.integer(*estimator, "minimum_samples", "$.estimator_policy");
        const auto iterations =
            reader.integer(*estimator, "robust_iterations", "$.estimator_policy");
        if (minimumSamples <= 0 || iterations <= 0)
            reader.errors.push_back("$.estimator_policy counts must be positive");
        else {
            workflow.estimator_options.minimum_samples = static_cast<std::size_t>(minimumSamples);
            workflow.estimator_options.robust_iterations = static_cast<std::size_t>(iterations);
        }
        workflow.estimator_options.minimum_mean_power =
            reader.number(*estimator, "minimum_mean_power", "$.estimator_policy");
        workflow.estimator_options.minimum_model_coherence =
            reader.number(*estimator, "minimum_model_coherence", "$.estimator_policy");
        workflow.estimator_options.maximum_absolute_frequency_fraction =
            reader.number(*estimator, "maximum_absolute_frequency_fraction", "$.estimator_policy");
        workflow.estimator_options.huber_threshold_radians =
            reader.number(*estimator, "huber_threshold_radians", "$.estimator_policy");
    }
    const auto* acceptanceValue = reader.member(*root, "acceptance_policy", "$");
    const auto* acceptance =
        acceptanceValue ? reader.object(*acceptanceValue, "$.acceptance_policy") : nullptr;
    if (acceptance) {
        reader.members(*acceptance,
                       {"version", "minimum_duration_seconds", "minimum_signal_to_noise_db",
                        "maximum_clipped_fraction", "maximum_missing_samples",
                        "maximum_discontinuities", "maximum_absolute_drift_hz_per_second",
                        "maximum_frequency_instability_hz", "maximum_interference_to_carrier_db"},
                       "$.acceptance_policy");
        workflow.acceptance_policy.version =
            reader.string(*acceptance, "version", "$.acceptance_policy");
        workflow.acceptance_policy.minimum_duration_seconds =
            reader.number(*acceptance, "minimum_duration_seconds", "$.acceptance_policy");
        workflow.acceptance_policy.minimum_signal_to_noise_db =
            reader.number(*acceptance, "minimum_signal_to_noise_db", "$.acceptance_policy");
        workflow.acceptance_policy.maximum_clipped_fraction =
            reader.number(*acceptance, "maximum_clipped_fraction", "$.acceptance_policy");
        const auto missing =
            reader.integer(*acceptance, "maximum_missing_samples", "$.acceptance_policy");
        const auto discontinuities =
            reader.integer(*acceptance, "maximum_discontinuities", "$.acceptance_policy");
        if (missing < 0 || discontinuities < 0)
            reader.errors.push_back("$.acceptance_policy counts must be non-negative");
        else {
            workflow.acceptance_policy.maximum_missing_samples =
                static_cast<std::uint64_t>(missing);
            workflow.acceptance_policy.maximum_discontinuities =
                static_cast<std::uint64_t>(discontinuities);
        }
        workflow.acceptance_policy.maximum_absolute_drift_hz_per_second = reader.number(
            *acceptance, "maximum_absolute_drift_hz_per_second", "$.acceptance_policy");
        workflow.acceptance_policy.maximum_frequency_instability_hz =
            reader.number(*acceptance, "maximum_frequency_instability_hz", "$.acceptance_policy");
        workflow.acceptance_policy.maximum_interference_to_carrier_db =
            reader.number(*acceptance, "maximum_interference_to_carrier_db", "$.acceptance_policy");
    }
    if (liveMode && product.live_acquisition && !workflow.observations.empty()) {
        const auto& live = *product.live_acquisition;
        capture::CaptureRequest captureRequest;
        captureRequest.device_arguments = live.device_arguments;
        captureRequest.rx_channel = live.rx_channel;
        captureRequest.center_frequency_hz =
            workflow.observations.front().indicated_center_frequency_hz;
        captureRequest.sample_rate_sps =
            static_cast<double>(workflow.requested_configuration.sample_rate_hz);
        if (workflow.requested_configuration.bandwidth_hz)
            captureRequest.bandwidth_hz =
                static_cast<double>(*workflow.requested_configuration.bandwidth_hz);
        captureRequest.gain_db = live.gain_db;
        captureRequest.duration_seconds = live.duration_seconds;
        captureRequest.sample_count = live.sample_count;
        captureRequest.read_timeout = live.read_timeout;
        captureRequest.setting_policy = capture::SettingPolicy::strict;
        const auto preflight = capture::validateMemoryAcquisitionRequestBeforeDevice(
            captureRequest, {}, {live.maximum_memory_bytes});
        for (const auto& error : preflight)
            reader.errors.push_back("$.live_acquisition preflight failed: " + error.message);
    }
    if (!reader.errors.empty()) {
        result.errors = std::move(reader.errors);
        return result;
    }
    result.request = std::move(product);
    return result;
}

std::uint64_t recordedInputMaximumBytes() noexcept {
    return kRecordedInputMaximumBytes;
}

std::string productUsage() {
    return "Usage: sdrcal calibrate --request FILE --trust-file FILE --output-dir DIR\n"
           "       sdrcal --help\n       sdrcal --version\n";
}

ProductExit runProductCommand(const ProductArguments& arguments, std::ostream& output,
                              std::ostream& diagnostics, ProductCancellationCheck cancelled,
                              LiveBoundaryFactory liveBoundaryFactory) {
    if (!arguments.ok() || arguments.action != ProductAction::calibrate) {
        output << terminalJson("usage_error", static_cast<int>(ProductExit::usage),
                               arguments.errors.empty() ? "invalid action"
                                                        : arguments.errors.front());
        return ProductExit::usage;
    }
    if (cancelled && cancelled()) {
        output << terminalJson("cancelled", static_cast<int>(ProductExit::cancelled),
                               "cancelled before input");
        return ProductExit::cancelled;
    }
    std::string error;
    const auto text = readText(arguments.request_path, error);
    if (!error.empty()) {
        output << terminalJson("input_error", static_cast<int>(ProductExit::input), error);
        return ProductExit::input;
    }
    std::string trustError;
    const auto trustText = readText(arguments.trust_path, trustError, 1024U * 1024U);
    std::map<std::string, std::string> trust;
    if (trustError.empty()) {
        const auto trustJson = profile::parseJson(trustText);
        if (!trustJson.success)
            trustError = "trust file is invalid JSON";
        else if (const auto* object = std::get_if<JsonObject>(&trustJson.value.value)) {
            for (const auto& [key, value] : *object) {
                const auto* signature = std::get_if<std::string>(&value.value);
                if (key.empty() || !signature || signature->empty()) {
                    trustError = "trust file must map non-empty key IDs to pinned signatures";
                    break;
                }
                trust.emplace(key, *signature);
            }
        } else
            trustError = "trust file root must be an object";
    }
    if (!trustError.empty()) {
        output << terminalJson("input_error", static_cast<int>(ProductExit::input), trustError);
        return ProductExit::input;
    }
    const auto parsed =
        parseProductRequest(text, arguments.request_path.parent_path(), std::move(trust));
    if (!parsed.ok()) {
        const auto reason =
            parsed.errors.empty() ? "request validation failed" : parsed.errors.front();
        diagnostics << "request rejected: " << reason << '\n';
        output << terminalJson("input_error", static_cast<int>(ProductExit::input), reason);
        return ProductExit::input;
    }
    std::string inputError;
    std::unique_ptr<application::DeviceWorkflowBoundary> boundary;
    if (parsed.request->input_mode == ProductInputMode::recorded) {
        boundary = std::make_unique<RecordedBoundary>(*parsed.request, cancelled, inputError);
    } else {
        if (!liveBoundaryFactory) {
            output << terminalJson("input_error", static_cast<int>(ProductExit::input),
                                   "live input is unavailable in this build");
            return ProductExit::input;
        }
        try {
            boundary = liveBoundaryFactory(*parsed.request);
        } catch (const std::exception& exception) {
            output << terminalJson("input_error", static_cast<int>(ProductExit::input),
                                   "live boundary construction failed: " +
                                       std::string(exception.what()));
            return ProductExit::input;
        } catch (...) {
            output << terminalJson("input_error", static_cast<int>(ProductExit::input),
                                   "live boundary construction failed");
            return ProductExit::input;
        }
        if (!boundary) {
            output << terminalJson("input_error", static_cast<int>(ProductExit::input),
                                   "live boundary construction returned no boundary");
            return ProductExit::input;
        }
    }
    std::error_code ec;
    if (std::filesystem::exists(arguments.output_directory, ec) || ec) {
        output << terminalJson("output_error", static_cast<int>(ProductExit::output),
                               "output destination already exists or cannot be inspected");
        return ProductExit::output;
    }
    const auto staging = arguments.output_directory.string() + ".staging";
    if (std::filesystem::exists(staging, ec) || !std::filesystem::create_directories(staging, ec)) {
        output << terminalJson("output_error", static_cast<int>(ProductExit::output),
                               "unique staging directory cannot be created");
        return ProductExit::output;
    }
    const auto cleanup = [&] {
        std::error_code ignored;
        std::filesystem::remove_all(staging, ignored);
    };
    diagnostics << "progress: running shared calibration workflow\n";
    application::CalibrationWorkflow workflow;
    auto result = workflow.run(parsed.request->workflow, *boundary, cancelled);
    if (!result.succeeded()) {
        cleanup();
        const bool wasCancelled = result.status == application::WorkflowStatus::cancelled;
        const auto code = wasCancelled
                              ? ProductExit::cancelled
                              : (inputError.empty() ? ProductExit::workflow : ProductExit::input);
        diagnostics << "workflow stopped: " << result.reason << '\n';
        output << terminalJson(
            wasCancelled ? "cancelled" : (inputError.empty() ? "workflow_error" : "input_error"),
            static_cast<int>(code), result.reason);
        return code;
    }
    if (parsed.request->workflow.interoperability &&
        (!result.interoperability || !result.interoperability->succeeded())) {
        cleanup();
        output << terminalJson("workflow_error", static_cast<int>(ProductExit::workflow),
                               "requested WSJT-X projection was not produced");
        return ProductExit::workflow;
    }
    if (cancelled && cancelled()) {
        cleanup();
        output << terminalJson("cancelled", static_cast<int>(ProductExit::cancelled),
                               "cancelled before artifact publication");
        return ProductExit::cancelled;
    }
    const auto evidenceJson = profile::parseJson(result.evidence_manifest);
    if (!evidenceJson.success || !std::holds_alternative<JsonObject>(evidenceJson.value.value)) {
        cleanup();
        output << terminalJson("output_error", static_cast<int>(ProductExit::output),
                               "evidence candidate could not be finalized");
        return ProductExit::output;
    }
    auto evidenceObject = std::get<JsonObject>(evidenceJson.value.value);
    evidenceObject["atomic_write_completed"] = true;
    const auto finalizedEvidence = profile::canonicalizeJson(evidenceObject);
    if (!finalizedEvidence.success || !result.profile->provenance.evidence_bundle) {
        cleanup();
        output << terminalJson("output_error", static_cast<int>(ProductExit::output),
                               "evidence publication binding could not be finalized");
        return ProductExit::output;
    }
    result.profile->provenance.evidence_bundle->sha256 =
        profile::sha256Hex(finalizedEvidence.value);
    std::string integrityError;
    if (!profile::refreshIntegrity(*result.profile, &integrityError)) {
        cleanup();
        output << terminalJson("output_error", static_cast<int>(ProductExit::output),
                               "profile integrity refresh failed: " + integrityError);
        return ProductExit::output;
    }
    std::vector<std::string> artifacts{"profile.json", "evidence.json", "summary.json"};
    if (!writeFile(std::filesystem::path(staging) / "profile.json",
                   profile::serializeProfile(*result.profile)) ||
        !writeFile(std::filesystem::path(staging) / "evidence.json", finalizedEvidence.value) ||
        !writeFile(std::filesystem::path(staging) / "summary.json", result.evidence_summary)) {
        cleanup();
        output << terminalJson("output_error", static_cast<int>(ProductExit::output),
                               "artifact write failed");
        return ProductExit::output;
    }
    if (result.interoperability && result.interoperability->succeeded()) {
        if (!writeFile(std::filesystem::path(staging) / "wsjtx.ini",
                       result.interoperability->text)) {
            cleanup();
            output << terminalJson("output_error", static_cast<int>(ProductExit::output),
                                   "WSJT-X projection write failed");
            return ProductExit::output;
        }
        artifacts.emplace_back("wsjtx.ini");
    }
    if (cancelled && cancelled()) {
        cleanup();
        output << terminalJson("cancelled", static_cast<int>(ProductExit::cancelled),
                               "cancelled before output publication");
        return ProductExit::cancelled;
    }
    std::filesystem::rename(staging, arguments.output_directory, ec);
    if (ec) {
        cleanup();
        output << terminalJson("output_error", static_cast<int>(ProductExit::output),
                               "atomic output publication failed");
        return ProductExit::output;
    }
    diagnostics << "progress: output transaction published\n";
    output << terminalJson("success", static_cast<int>(ProductExit::success), {}, artifacts);
    return ProductExit::success;
}

} // namespace sdrcal::cli
