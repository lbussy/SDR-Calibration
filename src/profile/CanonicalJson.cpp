#include "profile/CanonicalJson.h"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <cstdio>
#include <limits>
#include <locale>
#include <set>
#include <sstream>
#include <stdexcept>

namespace sdrcal::profile {
namespace {

std::string enumString(ProfileStatus value) {
    switch (value) {
    case ProfileStatus::qualification_capable:
        return "qualification_capable";
    case ProfileStatus::calibrated:
        return "calibrated";
    case ProfileStatus::informational_only:
        return "informational_only";
    case ProfileStatus::revoked:
        return "revoked";
    }
    return {};
}

std::string enumString(IdentityStrength value) {
    switch (value) {
    case IdentityStrength::hardware_serial:
        return "hardware_serial";
    case IdentityStrength::driver_serial:
        return "driver_serial";
    case IdentityStrength::operator_assigned:
        return "operator_assigned";
    case IdentityStrength::insufficient:
        return "insufficient";
    }
    return {};
}

std::string enumString(core::ReferenceClass value) {
    switch (value) {
    case core::ReferenceClass::authority_confirmed:
        return "authority_confirmed";
    case core::ReferenceClass::derived_traceable:
        return "derived_traceable";
    case core::ReferenceClass::locally_characterized:
        return "locally_characterized";
    case core::ReferenceClass::ad_hoc:
        return "ad_hoc";
    case core::ReferenceClass::unknown:
        return "unknown";
    }
    return {};
}

JsonArray strings(const std::vector<std::string>& values) {
    JsonArray result;
    result.reserve(values.size());
    for (const auto& value : values)
        result.emplace_back(value);
    return result;
}

void optionalString(JsonObject& object, const char* key, const std::optional<std::string>& value) {
    if (value)
        object.emplace(key, *value);
}

std::vector<std::uint16_t> utf16Units(std::string_view input) {
    std::vector<std::uint16_t> result;
    for (std::size_t i = 0; i < input.size();) {
        const auto first = static_cast<unsigned char>(input[i]);
        std::uint32_t cp = 0;
        std::size_t count = 0;
        if (first < 0x80U) {
            cp = first;
            count = 1;
        } else if ((first & 0xE0U) == 0xC0U) {
            cp = first & 0x1FU;
            count = 2;
        } else if ((first & 0xF0U) == 0xE0U) {
            cp = first & 0x0FU;
            count = 3;
        } else if ((first & 0xF8U) == 0xF0U) {
            cp = first & 0x07U;
            count = 4;
        } else
            throw std::runtime_error("invalid UTF-8 leading byte");
        if (i + count > input.size())
            throw std::runtime_error("truncated UTF-8");
        for (std::size_t j = 1; j < count; ++j) {
            const auto next = static_cast<unsigned char>(input[i + j]);
            if ((next & 0xC0U) != 0x80U)
                throw std::runtime_error("invalid UTF-8 continuation");
            cp = (cp << 6U) | (next & 0x3FU);
        }
        if ((count == 2 && cp < 0x80U) || (count == 3 && cp < 0x800U) ||
            (count == 4 && cp < 0x10000U) || cp > 0x10FFFFU || (cp >= 0xD800U && cp <= 0xDFFFU))
            throw std::runtime_error("invalid UTF-8 scalar");
        if (cp <= 0xFFFFU)
            result.push_back(static_cast<std::uint16_t>(cp));
        else {
            cp -= 0x10000U;
            result.push_back(static_cast<std::uint16_t>(0xD800U + (cp >> 10U)));
            result.push_back(static_cast<std::uint16_t>(0xDC00U + (cp & 0x3FFU)));
        }
        i += count;
    }
    return result;
}

std::string escapeString(std::string_view input) {
    (void)utf16Units(input);
    std::string out = "\"";
    constexpr char hex[] = "0123456789abcdef";
    for (const char character : input) {
        const auto byte = static_cast<unsigned char>(character);
        switch (byte) {
        case '"':
            out += "\\\"";
            break;
        case '\\':
            out += "\\\\";
            break;
        case '\b':
            out += "\\b";
            break;
        case '\f':
            out += "\\f";
            break;
        case '\n':
            out += "\\n";
            break;
        case '\r':
            out += "\\r";
            break;
        case '\t':
            out += "\\t";
            break;
        default:
            if (byte < 0x20U) {
                out += "\\u00";
                out.push_back(hex[byte >> 4U]);
                out.push_back(hex[byte & 0x0FU]);
            } else
                out.push_back(static_cast<char>(byte));
        }
    }
    out.push_back('"');
    return out;
}

std::string numberString(double value) {
    if (!std::isfinite(value))
        throw std::runtime_error("non-finite JSON number");
    if (value == 0.0)
        return "0";
    char buffer[128];
    const auto conversion = std::to_chars(buffer, buffer + sizeof(buffer), value);
    if (conversion.ec != std::errc{})
        throw std::runtime_error("number conversion failed");
    std::string raw(buffer, conversion.ptr);
    const auto epos = raw.find_first_of("eE");
    if (epos == std::string::npos)
        return raw;

    const bool negative = raw.front() == '-';
    const std::size_t start = negative ? 1U : 0U;
    std::string mantissa = raw.substr(start, epos - start);
    const int exponent = std::stoi(raw.substr(epos + 1));
    const auto dot = mantissa.find('.');
    const int before =
        dot == std::string::npos ? static_cast<int>(mantissa.size()) : static_cast<int>(dot);
    mantissa.erase(std::remove(mantissa.begin(), mantissa.end(), '.'), mantissa.end());
    const int decimal_exponent = exponent + before - 1;
    std::string out = negative ? "-" : "";
    if (decimal_exponent >= -6 && decimal_exponent < 21) {
        const int decimal_position = decimal_exponent + 1;
        if (decimal_position <= 0) {
            out += "0." + std::string(static_cast<std::size_t>(-decimal_position), '0') + mantissa;
        } else if (decimal_position >= static_cast<int>(mantissa.size())) {
            out += mantissa + std::string(static_cast<std::size_t>(
                                              decimal_position - static_cast<int>(mantissa.size())),
                                          '0');
        } else {
            out += mantissa.substr(0, static_cast<std::size_t>(decimal_position));
            out.push_back('.');
            out += mantissa.substr(static_cast<std::size_t>(decimal_position));
        }
        return out;
    }
    out.push_back(mantissa.front());
    if (mantissa.size() > 1U)
        out += "." + mantissa.substr(1);
    out += decimal_exponent >= 0 ? "e+" : "e-";
    out += std::to_string(std::abs(decimal_exponent));
    return out;
}

void appendCanonical(const JsonValue& value, std::string& out) {
    std::visit(
        [&out](const auto& item) {
            using T = std::decay_t<decltype(item)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>)
                out += "null";
            else if constexpr (std::is_same_v<T, bool>)
                out += item ? "true" : "false";
            else if constexpr (std::is_same_v<T, std::int64_t>) {
                constexpr std::int64_t maximum_exact_integer = 9'007'199'254'740'991LL;
                if (item < -maximum_exact_integer || item > maximum_exact_integer)
                    throw std::runtime_error("integer exceeds exact IEEE-754 range");
                out += std::to_string(item);
            } else if constexpr (std::is_same_v<T, double>)
                out += numberString(item);
            else if constexpr (std::is_same_v<T, std::string>)
                out += escapeString(item);
            else if constexpr (std::is_same_v<T, JsonArray>) {
                out.push_back('[');
                for (std::size_t i = 0; i < item.size(); ++i) {
                    if (i != 0U)
                        out.push_back(',');
                    appendCanonical(item[i], out);
                }
                out.push_back(']');
            } else {
                std::vector<const JsonObject::value_type*> members;
                members.reserve(item.size());
                for (const auto& member : item)
                    members.push_back(&member);
                std::sort(members.begin(), members.end(), [](const auto* lhs, const auto* rhs) {
                    return utf16Units(lhs->first) < utf16Units(rhs->first);
                });
                out.push_back('{');
                for (std::size_t i = 0; i < members.size(); ++i) {
                    if (i != 0U)
                        out.push_back(',');
                    out += escapeString(members[i]->first);
                    out.push_back(':');
                    appendCanonical(members[i]->second, out);
                }
                out.push_back('}');
            }
        },
        value.value);
}

class JsonParser {
  public:
    explicit JsonParser(std::string_view input) : input_(input) {}

    JsonValue parse() {
        skipWhitespace();
        auto result = parseValue();
        skipWhitespace();
        if (position_ != input_.size())
            fail("trailing JSON data");
        return result;
    }

  private:
    [[noreturn]] void fail(const char* message) const {
        throw std::runtime_error(std::string(message) + " at byte " + std::to_string(position_));
    }
    void skipWhitespace() {
        while (position_ < input_.size() &&
               (input_[position_] == ' ' || input_[position_] == '\t' ||
                input_[position_] == '\n' || input_[position_] == '\r'))
            ++position_;
    }
    char take() {
        if (position_ >= input_.size())
            fail("unexpected end of JSON");
        return input_[position_++];
    }
    bool consume(std::string_view token) {
        if (input_.substr(position_, token.size()) != token)
            return false;
        position_ += token.size();
        return true;
    }
    static void appendUtf8(std::string& output, std::uint32_t cp) {
        if (cp <= 0x7FU)
            output.push_back(static_cast<char>(cp));
        else if (cp <= 0x7FFU) {
            output.push_back(static_cast<char>(0xC0U | (cp >> 6U)));
            output.push_back(static_cast<char>(0x80U | (cp & 0x3FU)));
        } else if (cp <= 0xFFFFU) {
            output.push_back(static_cast<char>(0xE0U | (cp >> 12U)));
            output.push_back(static_cast<char>(0x80U | ((cp >> 6U) & 0x3FU)));
            output.push_back(static_cast<char>(0x80U | (cp & 0x3FU)));
        } else {
            output.push_back(static_cast<char>(0xF0U | (cp >> 18U)));
            output.push_back(static_cast<char>(0x80U | ((cp >> 12U) & 0x3FU)));
            output.push_back(static_cast<char>(0x80U | ((cp >> 6U) & 0x3FU)));
            output.push_back(static_cast<char>(0x80U | (cp & 0x3FU)));
        }
    }
    std::uint32_t hexQuad() {
        std::uint32_t value = 0;
        for (int i = 0; i < 4; ++i) {
            const char digit = take();
            value <<= 4U;
            if (digit >= '0' && digit <= '9')
                value |= static_cast<unsigned>(digit - '0');
            else if (digit >= 'a' && digit <= 'f')
                value |= static_cast<unsigned>(digit - 'a' + 10);
            else if (digit >= 'A' && digit <= 'F')
                value |= static_cast<unsigned>(digit - 'A' + 10);
            else
                fail("invalid Unicode escape");
        }
        return value;
    }
    std::string parseString() {
        if (take() != '"')
            fail("expected string");
        std::string output;
        while (true) {
            const char value = take();
            if (value == '"')
                break;
            if (static_cast<unsigned char>(value) < 0x20U)
                fail("unescaped control character");
            if (value != '\\') {
                output.push_back(value);
                continue;
            }
            switch (take()) {
            case '"':
                output.push_back('"');
                break;
            case '\\':
                output.push_back('\\');
                break;
            case '/':
                output.push_back('/');
                break;
            case 'b':
                output.push_back('\b');
                break;
            case 'f':
                output.push_back('\f');
                break;
            case 'n':
                output.push_back('\n');
                break;
            case 'r':
                output.push_back('\r');
                break;
            case 't':
                output.push_back('\t');
                break;
            case 'u': {
                std::uint32_t cp = hexQuad();
                if (cp >= 0xD800U && cp <= 0xDBFFU) {
                    if (!consume("\\u"))
                        fail("missing low surrogate");
                    const auto low = hexQuad();
                    if (low < 0xDC00U || low > 0xDFFFU)
                        fail("invalid low surrogate");
                    cp = 0x10000U + ((cp - 0xD800U) << 10U) + (low - 0xDC00U);
                } else if (cp >= 0xDC00U && cp <= 0xDFFFU)
                    fail("unpaired low surrogate");
                appendUtf8(output, cp);
                break;
            }
            default:
                fail("invalid string escape");
            }
        }
        (void)utf16Units(output);
        return output;
    }
    JsonValue parseNumber() {
        const std::size_t start = position_;
        if (position_ < input_.size() && input_[position_] == '-')
            ++position_;
        if (position_ >= input_.size())
            fail("incomplete number");
        if (input_[position_] == '0')
            ++position_;
        else {
            if (input_[position_] < '1' || input_[position_] > '9')
                fail("invalid number");
            while (position_ < input_.size() && input_[position_] >= '0' &&
                   input_[position_] <= '9')
                ++position_;
        }
        bool integer = true;
        if (position_ < input_.size() && input_[position_] == '.') {
            integer = false;
            ++position_;
            const auto digits = position_;
            while (position_ < input_.size() && input_[position_] >= '0' &&
                   input_[position_] <= '9')
                ++position_;
            if (digits == position_)
                fail("fraction requires a digit");
        }
        if (position_ < input_.size() && (input_[position_] == 'e' || input_[position_] == 'E')) {
            integer = false;
            ++position_;
            if (position_ < input_.size() && (input_[position_] == '+' || input_[position_] == '-'))
                ++position_;
            const auto digits = position_;
            while (position_ < input_.size() && input_[position_] >= '0' &&
                   input_[position_] <= '9')
                ++position_;
            if (digits == position_)
                fail("exponent requires a digit");
        }
        const auto token = input_.substr(start, position_ - start);
        if (integer) {
            std::int64_t value = 0;
            const auto converted =
                std::from_chars(token.data(), token.data() + token.size(), value);
            if (converted.ec == std::errc{})
                return value;
        }
        double value = 0.0;
        std::istringstream numberStream{std::string(token)};
        numberStream.imbue(std::locale::classic());
        numberStream >> value;
        if (!numberStream || numberStream.peek() != std::char_traits<char>::eof() ||
            !std::isfinite(value))
            fail("number is outside supported finite range");
        return value;
    }
    JsonValue parseValue() {
        skipWhitespace();
        if (position_ >= input_.size())
            fail("missing JSON value");
        if (input_[position_] == '"')
            return parseString();
        if (input_[position_] == '{') {
            ++position_;
            JsonObject object;
            skipWhitespace();
            if (position_ < input_.size() && input_[position_] == '}') {
                ++position_;
                return object;
            }
            while (true) {
                skipWhitespace();
                if (position_ >= input_.size() || input_[position_] != '"')
                    fail("object key must be string");
                auto key = parseString();
                skipWhitespace();
                if (take() != ':')
                    fail("expected colon");
                auto value = parseValue();
                if (!object.emplace(std::move(key), std::move(value)).second)
                    fail("duplicate object key");
                skipWhitespace();
                const char delimiter = take();
                if (delimiter == '}')
                    break;
                if (delimiter != ',')
                    fail("expected object delimiter");
            }
            return object;
        }
        if (input_[position_] == '[') {
            ++position_;
            JsonArray array;
            skipWhitespace();
            if (position_ < input_.size() && input_[position_] == ']') {
                ++position_;
                return array;
            }
            while (true) {
                array.push_back(parseValue());
                skipWhitespace();
                const char delimiter = take();
                if (delimiter == ']')
                    break;
                if (delimiter != ',')
                    fail("expected array delimiter");
            }
            return array;
        }
        if (consume("true"))
            return true;
        if (consume("false"))
            return false;
        if (consume("null"))
            return nullptr;
        if (input_[position_] == '-' || (input_[position_] >= '0' && input_[position_] <= '9'))
            return parseNumber();
        fail("invalid JSON value");
    }

    std::string_view input_;
    std::size_t position_ = 0;
};

JsonObject uncertaintyObject(const SegmentUncertainty& value) {
    JsonObject model{
        {"type", "linear"}, {"base_hz", value.base_hz}, {"ppm_component", value.ppm_component}};
    JsonObject result{{"kind", value.kind == UncertaintyKind::standard ? "standard" : "expanded"},
                      {"coverage_factor", value.coverage_factor},
                      {"model", model},
                      {"included_components", strings(value.included_components)}};
    if (value.confidence_level_approximate)
        result["confidence_level_approximate"] = *value.confidence_level_approximate;
    if (!value.excluded_components.empty())
        result["excluded_components"] = strings(value.excluded_components);
    return result;
}

} // namespace

CanonicalJsonResult canonicalizeJson(const JsonValue& value) {
    CanonicalJsonResult result;
    try {
        appendCanonical(value, result.value);
        result.success = true;
    } catch (const std::exception& error) {
        result.error = error.what();
    }
    return result;
}

JsonParseResult parseJson(std::string_view input) {
    JsonParseResult result;
    try {
        result.value = JsonParser(input).parse();
        result.success = true;
    } catch (const std::exception& error) {
        result.error = error.what();
    }
    return result;
}

JsonObject profileJsonObject(const CalibrationProfile& p, bool include_integrity) {
    JsonObject root{{"schema", JsonObject{{"name", p.schema_name}, {"version", p.schema_version}}},
                    {"profile_id", p.profile_id},
                    {"profile_status", enumString(p.profile_status)},
                    {"created_at", p.created_at}};
    if (p.supersedes_profile_id)
        root["supersedes_profile_id"] = *p.supersedes_profile_id;
    root["device"] = JsonObject{{"driver", p.device.driver},
                                {"manufacturer", p.device.manufacturer},
                                {"model", p.device.model},
                                {"identifier", p.device.identifier},
                                {"identity_strength", enumString(p.device.strength)}};
    JsonObject config{{"clock_source", p.configuration.clock_source},
                      {"sample_rate_hz", p.configuration.sample_rate_hz},
                      {"frequency_correction_ppm", p.configuration.frequency_correction_ppm}};
    if (p.configuration.bandwidth_hz)
        config["bandwidth_hz"] = *p.configuration.bandwidth_hz;
    optionalString(config, "driver_version", p.configuration.driver_version);
    optionalString(config, "firmware_version", p.configuration.firmware_version);
    optionalString(config, "antenna_port", p.configuration.antenna_port);
    optionalString(config, "tuner_path", p.configuration.tuner_path);
    if (!p.configuration.binding_extension.empty())
        config["binding_extension"] = p.configuration.binding_extension;
    root["configuration"] = config;
    JsonArray segments;
    for (const auto& segment : p.segments) {
        JsonObject model;
        if (segment.model_type == SegmentModelType::constant_ppm)
            model = {{"type", "constant_ppm"}, {"error_ppm", segment.error_ppm}};
        else
            model = {{"type", "linear"},
                     {"reference_frequency_hz", segment.reference_frequency_hz},
                     {"intercept_error_hz", segment.intercept_error_hz},
                     {"slope_ppm", segment.slope_ppm}};
        segments.emplace_back(JsonObject{{"segment_id", segment.segment_id},
                                         {"minimum_frequency_hz", segment.minimum_frequency_hz},
                                         {"maximum_frequency_hz", segment.maximum_frequency_hz},
                                         {"model", model},
                                         {"uncertainty", uncertaintyObject(segment.uncertainty)}});
    }
    root["frequency_model"] =
        JsonObject{{"error_definition", "indicated_minus_true"}, {"segments", segments}};
    JsonArray components;
    for (const auto& c : p.assurance.components)
        components.emplace_back(
            JsonObject{{"name", c.name}, {"score", c.score}, {"basis", c.basis}});
    root["assurance"] =
        JsonObject{{"scoring_policy_version", p.assurance.scoring_policy_version},
                   {"reliability_quotient", p.assurance.reliability_quotient},
                   {"reference_class", enumString(p.assurance.reference_class)},
                   {"reference_score_ceiling", p.assurance.reference_score_ceiling},
                   {"limiting_components", strings(p.assurance.limiting_components)},
                   {"components", components}};
    root["validity"] = JsonObject{
        {"calibrated_at", p.validity.calibrated_at},
        {"not_valid_after", p.validity.not_valid_after},
        {"minimum_warmup_seconds", p.validity.minimum_warmup_seconds},
        {"temperature",
         JsonObject{{"reference_c", p.validity.temperature.reference_c},
                    {"minimum_c", p.validity.temperature.minimum_c},
                    {"maximum_c", p.validity.temperature.maximum_c},
                    {"measurement_location", p.validity.temperature.measurement_location}}}};
    JsonObject provenance{{"calibration_run_id", p.provenance.calibration_run_id},
                          {"software", JsonObject{{"name", p.provenance.software.name},
                                                  {"version", p.provenance.software.version}}},
                          {"observation_ids", strings(p.provenance.observation_ids)}};
    JsonArray references;
    for (const auto& r : p.provenance.reference_set) {
        JsonObject record{{"reference_id", r.reference_id},
                          {"kind", enumString(r.kind)},
                          {"nominal_frequency_hz", r.nominal_frequency_hz},
                          {"assurance_score_ceiling", r.assurance_score_ceiling},
                          {"evidence_sha256", r.evidence_sha256}};
        if (r.frequency_uncertainty_hz)
            record["frequency_uncertainty_hz"] = *r.frequency_uncertainty_hz;
        optionalString(record, "description", r.description);
        references.emplace_back(record);
    }
    provenance["reference_set"] = references;
    if (p.provenance.reference_registry)
        provenance["reference_registry"] =
            JsonObject{{"schema_version", p.provenance.reference_registry->schema_version},
                       {"registry_version", p.provenance.reference_registry->registry_version},
                       {"sha256", p.provenance.reference_registry->sha256}};
    if (p.provenance.evidence_bundle) {
        JsonObject bundle{{"media_type", p.provenance.evidence_bundle->media_type},
                          {"sha256", p.provenance.evidence_bundle->sha256}};
        optionalString(bundle, "location", p.provenance.evidence_bundle->location);
        provenance["evidence_bundle"] = bundle;
    }
    root["provenance"] = provenance;
    if (!p.adapters.empty()) {
        JsonArray adapters;
        for (const auto& a : p.adapters) {
            JsonObject record{{"adapter_id", a.adapter_id},
                              {"direction", a.direction},
                              {"application", a.application},
                              {"application_version", a.application_version},
                              {"created_at", a.created_at},
                              {"artifact_sha256", a.artifact_sha256},
                              {"lossy", a.lossy},
                              {"metadata", a.metadata}};
            if (!a.omitted_capabilities.empty())
                record["omitted_capabilities"] = strings(a.omitted_capabilities);
            if (a.compatibility)
                record["compatibility"] =
                    JsonObject{{"version_range", a.compatibility->version_range},
                               {"mapping_test_id", a.compatibility->mapping_test_id},
                               {"mapping_verified", a.compatibility->mapping_verified}};
            adapters.emplace_back(record);
        }
        root["interoperability"] = JsonObject{{"adapters", adapters}};
    }
    for (const auto& extension : p.optional_extensions)
        root.emplace(extension.first, extension.second);
    if (include_integrity) {
        JsonObject integrity{{"canonicalization", p.integrity.canonicalization},
                             {"sha256", p.integrity.sha256}};
        if (p.integrity.signature)
            integrity["signature"] = JsonObject{{"algorithm", p.integrity.signature->algorithm},
                                                {"key_id", p.integrity.signature->key_id},
                                                {"value", p.integrity.signature->value}};
        root["integrity"] = integrity;
    }
    return root;
}

std::string serializeProfile(const CalibrationProfile& profile, bool include_integrity) {
    auto result = canonicalizeJson(profileJsonObject(profile, include_integrity));
    return result.success ? result.value : std::string{};
}

} // namespace sdrcal::profile
