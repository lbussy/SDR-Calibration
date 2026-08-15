#include "capture/CaptureManifestWriter.h"

#include "capture/CapturePlan.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <locale>
#include <sstream>
#include <stdexcept>

namespace sdrcal::capture {
namespace {

std::string quoted(const std::string& value) {
    std::ostringstream output;
    output << '"';
    for (const char rawCharacter : value) {
        const auto character = static_cast<unsigned char>(rawCharacter);
        switch (character) {
        case '"': output << "\\\""; break;
        case '\\': output << "\\\\"; break;
        case '\b': output << "\\b"; break;
        case '\f': output << "\\f"; break;
        case '\n': output << "\\n"; break;
        case '\r': output << "\\r"; break;
        case '\t': output << "\\t"; break;
        default:
            if (character < 0x20U) {
                output << "\\u" << std::hex << std::setw(4) << std::setfill('0')
                       << static_cast<unsigned int>(character) << std::dec;
            } else {
                output << static_cast<char>(character);
            }
        }
    }
    output << '"';
    return output.str();
}

std::string number(double value) {
    if (!std::isfinite(value)) {
        throw std::invalid_argument("manifest numbers must be finite");
    }
    std::ostringstream output;
    output.imbue(std::locale::classic());
    output << std::setprecision(17) << value;
    return output.str();
}

std::string optionalNumber(const std::optional<double>& value) {
    return value.has_value() ? number(*value) : "null";
}

std::string optionalString(const std::optional<std::string>& value) {
    return value.has_value() ? quoted(*value) : "null";
}

std::string optionalBool(const std::optional<bool>& value) {
    if (!value.has_value()) {
        return "null";
    }
    return *value ? "true" : "false";
}

std::string stringMap(const std::map<std::string, std::string>& values, int indent) {
    if (values.empty()) {
        return "{}";
    }
    std::ostringstream output;
    output << "{\n";
    std::size_t index = 0;
    for (const auto& [key, value] : values) {
        output << std::string(static_cast<std::size_t>(indent + 2), ' ') << quoted(key) << ": "
               << quoted(value);
        output << (++index == values.size() ? "\n" : ",\n");
    }
    output << std::string(static_cast<std::size_t>(indent), ' ') << '}';
    return output.str();
}

std::string setting(const EffectiveSetting& value, int indent) {
    std::ostringstream output;
    output << "{\n"
           << std::string(static_cast<std::size_t>(indent + 2), ' ')
           << "\"requested\": " << optionalNumber(value.requested) << ",\n"
           << std::string(static_cast<std::size_t>(indent + 2), ' ')
           << "\"effective\": " << optionalNumber(value.effective) << ",\n"
           << std::string(static_cast<std::size_t>(indent + 2), ' ')
           << "\"state\": " << quoted(toString(value.state)) << '\n'
           << std::string(static_cast<std::size_t>(indent), ' ') << '}';
    return output.str();
}

} // namespace

std::string CaptureManifestWriter::serialize(
    const CapturePlan& plan,
    const DeviceMetadata& device,
    const CaptureResult& result,
    const std::filesystem::path& rawFilename) {
    std::ostringstream output;
    output.imbue(std::locale::classic());
    output << "{\n"
           << "  \"format\": {\"name\": \"sdrcal_capture\", \"version\": 1},\n"
           << "  \"artifact\": {\n"
           << "    \"raw_filename\": " << quoted(rawFilename.filename().string()) << ",\n"
           << "    \"datatype\": \"cf32_le\",\n"
           << "    \"layout\": \"interleaved_iq\",\n"
           << "    \"channel_count\": 1,\n"
           << "    \"bytes_per_sample\": 8,\n"
           << "    \"byte_count\": " << result.stream.written_bytes << ",\n"
           << "    \"atomic_publication\": "
           << (result.atomic_raw_publication ? "true" : "false") << "\n"
           << "  },\n"
           << "  \"request\": {\n"
           << "    \"device_arguments\": " << stringMap(plan.request.device_arguments, 4) << ",\n"
           << "    \"enumeration_index\": ";
    if (plan.request.enumeration_index.has_value()) {
        output << *plan.request.enumeration_index;
    } else {
        output << "null";
    }
    output << ",\n"
           << "    \"rx_channel\": " << plan.request.rx_channel << ",\n"
           << "    \"center_frequency_hz\": " << number(plan.request.center_frequency_hz) << ",\n"
           << "    \"sample_rate_sps\": " << number(plan.request.sample_rate_sps) << ",\n"
           << "    \"bandwidth_hz\": " << optionalNumber(plan.request.bandwidth_hz) << ",\n"
           << "    \"gain_db\": " << optionalNumber(plan.request.gain_db) << ",\n"
           << "    \"duration_seconds\": " << optionalNumber(plan.request.duration_seconds) << ",\n"
           << "    \"sample_count\": ";
    if (plan.request.sample_count.has_value()) {
        output << *plan.request.sample_count;
    } else {
        output << "null";
    }
    output << ",\n"
           << "    \"planned_sample_count\": " << plan.target_samples << ",\n"
           << "    \"rounding_rule\": " << quoted(plan.rounding_rule) << ",\n"
           << "    \"read_timeout_ms\": " << plan.request.read_timeout.count() << ",\n"
           << "    \"setting_policy\": " << quoted(toString(plan.request.setting_policy)) << ",\n"
           << "    \"output_path\": "
           << quoted(plan.request.output_path.filename().string()) << ",\n"
           << "    \"purpose\": " << optionalString(plan.request.purpose) << "\n"
           << "  },\n"
           << "  \"device\": {\n"
           << "    \"resolved_arguments\": " << stringMap(device.resolved_arguments, 4) << ",\n"
           << "    \"driver_key\": " << optionalString(device.driver_key) << ",\n"
           << "    \"hardware_key\": " << optionalString(device.hardware_key) << ",\n"
           << "    \"serial\": " << optionalString(device.serial) << ",\n"
           << "    \"hardware_info\": " << stringMap(device.hardware_info, 4) << ",\n"
           << "    \"library_version\": " << optionalString(device.library_version) << ",\n"
           << "    \"driver_version\": " << optionalString(device.driver_version) << ",\n"
           << "    \"firmware_version\": " << optionalString(device.firmware_version) << ",\n"
           << "    \"antenna\": " << optionalString(device.antenna) << ",\n"
           << "    \"clock_source\": " << optionalString(device.clock_source) << ",\n"
           << "    \"tuner_path\": " << optionalString(device.tuner_path) << "\n"
           << "  },\n"
           << "  \"effective\": {\n"
           << "    \"center_frequency_hz\": "
           << setting(plan.effective.center_frequency_hz, 4) << ",\n"
           << "    \"sample_rate_sps\": " << setting(plan.effective.sample_rate_sps, 4) << ",\n"
           << "    \"bandwidth_hz\": " << setting(plan.effective.bandwidth_hz, 4) << ",\n"
           << "    \"gain_db\": " << setting(plan.effective.gain_db, 4) << ",\n"
           << "    \"automatic_gain\": " << optionalBool(plan.effective.automatic_gain) << ",\n"
           << "    \"frequency_correction_ppm\": "
           << optionalNumber(plan.effective.frequency_correction_ppm) << "\n"
           << "  },\n"
           << "  \"stream\": {\n"
           << "    \"format\": \"CF32\",\n"
           << "    \"mtu\": null,\n"
           << "    \"target_samples\": " << result.stream.target_samples << ",\n"
           << "    \"written_samples\": " << result.stream.written_samples << ",\n"
           << "    \"written_bytes\": " << result.stream.written_bytes << ",\n"
           << "    \"read_calls\": " << result.stream.read_calls << ",\n"
           << "    \"short_reads\": " << result.stream.short_reads << ",\n"
           << "    \"timeouts\": " << result.stream.timeouts << ",\n"
           << "    \"overflows\": " << result.stream.overflows << ",\n"
           << "    \"discontinuities\": " << result.stream.discontinuities << ",\n"
           << "    \"timestamps_available\": "
           << (result.stream.timestamps_available ? "true" : "false") << "\n"
           << "  },\n"
           << "  \"timing\": {\n"
           << "    \"utc_start\": " << quoted(result.utc_start) << ",\n"
           << "    \"utc_end\": " << quoted(result.utc_end) << ",\n"
           << "    \"monotonic_elapsed_seconds\": "
           << number(result.monotonic_elapsed_seconds) << "\n"
           << "  },\n"
           << "  \"outcome\": {\n"
           << "    \"status\": " << quoted(toString(result.status)) << ",\n"
           << "    \"error_category\": " << quoted(toString(result.error.category)) << ",\n"
           << "    \"error_message\": " << quoted(result.error.message) << ",\n"
           << "    \"source_cleanup\": {\"attempted\": "
           << (result.source_cleanup.attempted ? "true" : "false")
           << ", \"succeeded\": " << (result.source_cleanup.succeeded ? "true" : "false")
           << ", \"detail\": " << quoted(result.source_cleanup.detail) << "},\n"
           << "    \"raw_cleanup\": {\"attempted\": "
           << (result.raw_cleanup.attempted ? "true" : "false")
           << ", \"succeeded\": " << (result.raw_cleanup.succeeded ? "true" : "false")
           << ", \"detail\": " << quoted(result.raw_cleanup.detail) << "},\n"
           << "    \"manifest_cleanup\": {\"attempted\": "
           << (result.manifest_cleanup.attempted ? "true" : "false")
           << ", \"succeeded\": " << (result.manifest_cleanup.succeeded ? "true" : "false")
           << ", \"detail\": " << quoted(result.manifest_cleanup.detail) << "},\n"
           << "    \"final_state\": " << quoted(toString(result.final_state)) << "\n"
           << "  }\n"
           << "}\n";
    return output.str();
}

bool CaptureManifestWriter::write(
    const std::filesystem::path& path,
    const std::string& json,
    std::string& error) {
    std::ofstream output(path, std::ios::binary | std::ios::out | std::ios::trunc);
    if (!output) {
        error = "cannot open manifest output: " + path.string();
        return false;
    }
    output.write(json.data(), static_cast<std::streamsize>(json.size()));
    output.flush();
    if (!output) {
        error = "failed writing manifest output";
        return false;
    }
    output.close();
    if (output.fail()) {
        error = "failed closing manifest output";
        return false;
    }
    return true;
}

} // namespace sdrcal::capture
