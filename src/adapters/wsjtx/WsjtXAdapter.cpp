#include "adapters/wsjtx/WsjtXAdapter.h"

#include "profile/ProfileEngine.h"
#include "profile/Sha256.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <locale>
#include <regex>
#include <set>
#include <sstream>

namespace sdrcal::wsjtx {
namespace {
const std::vector<std::string> omitted = {"device_identity",         "effective_configuration",
                                          "frequency_domain",        "validity_conditions",
                                          "measurement_uncertainty", "provenance",
                                          "reliability_quotient",    "integrity_signature"};

bool supportedVersion(const std::string& version) {
    static const std::regex pattern(R"(^(2\.(6|7))\.[0-9]+(?:[-+].*)?$)");
    return std::regex_match(version, pattern);
}
bool timestamp(const std::string& value) {
    static const std::regex pattern(
        R"(^[0-9]{4}-(0[1-9]|1[0-2])-([0-2][0-9]|3[01])T([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9](\.[0-9]+)?(Z|[+-]([01][0-9]|2[0-3]):[0-5][0-9])$)");
    std::smatch match;
    if (!std::regex_match(value, match, pattern))
        return false;
    const int year = std::stoi(value.substr(0, 4));
    const int month = std::stoi(match[1].str());
    const int day = std::stoi(match[2].str());
    static constexpr int days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maximum = days[month];
    if (month == 2 && (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0)))
        maximum = 29;
    return day <= maximum;
}
bool timeOfDay(const std::string& value) {
    if (value.size() != 8U || value[2] != ':' || value[5] != ':')
        return false;
    for (const auto index : {0U, 1U, 3U, 4U, 6U, 7U})
        if (value[index] < '0' || value[index] > '9')
            return false;
    const int hour = (value[0] - '0') * 10 + value[1] - '0';
    const int minute = (value[3] - '0') * 10 + value[4] - '0';
    const int second = (value[6] - '0') * 10 + value[7] - '0';
    return hour < 24 && minute < 60 && second < 60;
}
std::string trim(std::string value) {
    const auto first = value.find_first_not_of(" \t\r");
    if (first == std::string::npos)
        return {};
    const auto last = value.find_last_not_of(" \t\r");
    return value.substr(first, last - first + 1U);
}
bool number(std::string_view text, double& value) {
    std::istringstream input{std::string(text)};
    input.imbue(std::locale::classic());
    input >> std::noskipws >> value;
    return input && input.peek() == std::char_traits<char>::eof() && std::isfinite(value);
}
profile::AdapterRecord record(std::string id, std::string direction, const std::string& version,
                              const std::string& created_at, const std::string& digest,
                              bool compatibility = true) {
    profile::AdapterRecord value;
    value.adapter_id = std::move(id);
    value.direction = std::move(direction);
    value.application_version = version;
    value.created_at = created_at;
    value.artifact_sha256 = digest;
    value.lossy = true;
    value.omitted_capabilities = omitted;
    value.compatibility =
        profile::AdapterCompatibility{"WSJT-X 2.6.x-2.7.x", "wsjtx-sign-v1", compatibility};
    return value;
}
std::string settingsText(double intercept, double slope) {
    std::ostringstream out;
    out << std::setprecision(std::numeric_limits<double>::max_digits10);
    out << "[Configuration]\nCalibrationIntercept=" << intercept
        << "\nCalibrationSlopePPM=" << slope << '\n';
    return out.str();
}
} // namespace

SettingsResult importSettings(std::string_view text, std::string version, std::string configuration,
                              std::string imported_at) {
    SettingsResult result;
    if (text.size() > maximum_input_bytes) {
        result.status = Status::resource_limit;
        result.reason = "settings input exceeds byte limit";
        return result;
    }
    if (!supportedVersion(version)) {
        result.status = Status::unsupported_version;
        result.reason = "WSJT-X version has no verified sign mapping";
        return result;
    }
    if (configuration.empty() || !timestamp(imported_at)) {
        result.status = Status::malformed;
        result.reason = "configuration name and import timestamp are required";
        return result;
    }
    bool in_configuration = false;
    std::optional<double> intercept;
    std::optional<double> slope;
    std::istringstream input{std::string(text)};
    std::string line;
    while (std::getline(input, line)) {
        line = trim(line);
        if (line.empty() || line[0] == ';' || line[0] == '#')
            continue;
        if (line.front() == '[' && line.back() == ']') {
            in_configuration = line == "[Configuration]";
            continue;
        }
        if (!in_configuration)
            continue;
        const auto separator = line.find('=');
        if (separator == std::string::npos)
            continue;
        const auto key = trim(line.substr(0, separator));
        const auto raw = trim(line.substr(separator + 1U));
        if (key != "CalibrationIntercept" && key != "CalibrationSlopePPM")
            continue;
        double parsed = 0.0;
        auto& target = key == "CalibrationIntercept" ? intercept : slope;
        if (target || !number(raw, parsed)) {
            result.status = target ? Status::ambiguous : Status::malformed;
            result.reason = target ? "duplicate calibration key" : "invalid calibration number";
            return result;
        }
        target = parsed;
    }
    if (!intercept || !slope || std::abs(*intercept) > 10000.0 || std::abs(*slope) > 1000.0) {
        result.status = Status::malformed;
        result.reason = "required calibration keys are missing or out of range";
        return result;
    }
    const auto digest = profile::sha256Hex(text);
    result.projection =
        SettingsProjection{*intercept, *slope,          version, configuration,  imported_at,
                           digest,     "wsjtx-sign-v1", true,    {true, omitted}};
    result.adapter_record = record("wsjt-x-settings", "import", version, imported_at, digest);
    result.adapter_record->metadata = {{"configuration_name", configuration},
                                       {"intercept_hz", *intercept},
                                       {"slope_ppm", *slope},
                                       {"informational_only", true}};
    result.status = Status::success;
    return result;
}

SettingsResult exportSettings(const profile::CalibrationProfile& source, std::string version,
                              std::string configuration, std::string exported_at) {
    SettingsResult result;
    if (!supportedVersion(version)) {
        result.status = Status::unsupported_version;
        result.reason = "WSJT-X version has no verified sign mapping";
        return result;
    }
    if (configuration.empty() || !timestamp(exported_at)) {
        result.status = Status::malformed;
        result.reason = "configuration name and export timestamp are required";
        return result;
    }
    if (!profile::validateProfile(source).valid()) {
        result.status = Status::invalid_profile;
        result.reason = "native profile is not valid";
        return result;
    }
    if (source.segments.size() != 1U ||
        source.segments.front().model_type != profile::SegmentModelType::linear) {
        result.status = Status::unrepresentable;
        result.reason = "WSJT-X requires exactly one linear segment";
        return result;
    }
    const auto& segment = source.segments.front();
    const double intercept =
        segment.intercept_error_hz - segment.slope_ppm * segment.reference_frequency_hz / 1e6;
    if (!std::isfinite(intercept) || std::abs(intercept) > 10000.0 ||
        std::abs(segment.slope_ppm) > 1000.0) {
        result.status = Status::unrepresentable;
        result.reason = "projected values exceed WSJT-X bounds";
        return result;
    }
    result.text = settingsText(intercept, segment.slope_ppm);
    const auto digest = profile::sha256Hex(result.text);
    result.projection =
        SettingsProjection{intercept, segment.slope_ppm, version, configuration,  exported_at,
                           digest,    "wsjtx-sign-v1",   true,    {true, omitted}};
    result.adapter_record = record("wsjt-x-settings", "export", version, exported_at, digest);
    result.adapter_record->metadata = {{"configuration_name", configuration},
                                       {"intercept_hz", intercept},
                                       {"slope_ppm", segment.slope_ppm}};
    result.status = Status::success;
    return result;
}

FmtResult importFmtAll(std::string_view text, std::string version, std::string imported_at) {
    FmtResult result;
    if (text.size() > maximum_input_bytes) {
        result.status = Status::resource_limit;
        result.reason = "fmt.all input exceeds byte limit";
        return result;
    }
    if (!supportedVersion(version)) {
        result.status = Status::unsupported_version;
        result.reason = "WSJT-X version has no verified format mapping";
        return result;
    }
    if (!timestamp(imported_at)) {
        result.status = Status::malformed;
        result.reason = "import timestamp is invalid";
        return result;
    }
    result.source_sha256 = profile::sha256Hex(text);
    std::istringstream input{std::string(text)};
    std::string line;
    std::size_t recognized = 0;
    std::size_t line_number = 0;
    while (std::getline(input, line)) {
        ++line_number;
        if (line_number > maximum_fmt_lines) {
            result.status = Status::resource_limit;
            result.reason = "fmt.all input exceeds line limit";
            return result;
        }
        FmtObservation observation;
        observation.original_line = line;
        observation.line_number = line_number;
        auto candidate = trim(line);
        if (candidate.empty()) {
            observation.manual_review_required = false;
            result.lines.push_back(std::move(observation));
            continue;
        }
        if (candidate[0] == '#' || candidate[0] == '!') {
            observation.rejected_by_source = true;
            candidate = trim(candidate.substr(1));
        }
        std::istringstream fields(candidate);
        std::string trailing;
        if ((fields >> observation.time >> observation.frequency_khz >> observation.mode_flag >>
             observation.expected_audio_hz >> observation.measured_audio_hz >>
             observation.delta_hz >> observation.snr_db >> observation.metric) &&
            !(fields >> trailing) && timeOfDay(observation.time) &&
            std::isfinite(observation.frequency_khz) && observation.frequency_khz > 0.0 &&
            std::isfinite(observation.expected_audio_hz) &&
            std::isfinite(observation.measured_audio_hz) && std::isfinite(observation.delta_hz) &&
            std::isfinite(observation.snr_db) && std::isfinite(observation.metric)) {
            observation.parsed = true;
            ++recognized;
        } else {
            observation.manual_review_required = true;
            observation.reason = "line does not match supported eight-column FreqCal layout";
        }
        result.lines.push_back(std::move(observation));
    }
    if (recognized == 0U) {
        result.status = Status::unknown_format;
        result.reason = "no recognized fmt.all observation lines";
        return result;
    }
    result.adapter_record =
        record("wsjt-x-fmt-all", "import", version, imported_at, result.source_sha256);
    const auto manual_review_count = static_cast<std::int64_t>(
        std::count_if(result.lines.begin(), result.lines.end(),
                      [](const auto& value) { return value.manual_review_required; }));
    result.adapter_record->metadata = {
        {"line_count", static_cast<std::int64_t>(result.lines.size())},
        {"recognized_observation_count", static_cast<std::int64_t>(recognized)},
        {"manual_review_count", manual_review_count}};
    result.status = Status::success;
    return result;
}

} // namespace sdrcal::wsjtx
