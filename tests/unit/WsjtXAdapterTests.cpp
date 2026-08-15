#include "adapters/wsjtx/WsjtXAdapter.h"
#include "profile/ProfileEngine.h"
#include "profile/Sha256.h"

#include <cmath>
#include <iostream>

namespace {
int failures = 0;
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            std::cerr << __FILE__ << ':' << __LINE__ << " CHECK(" #x ") failed\n";                 \
            ++failures;                                                                            \
        }                                                                                          \
    } while (false)

sdrcal::profile::CalibrationProfile profile() {
    using namespace sdrcal;
    profile::CalibrationProfile p;
    p.profile_id = "profile-1";
    p.profile_status = profile::ProfileStatus::qualification_capable;
    p.created_at = "2026-08-15T12:00:00Z";
    p.device = {"driver", "maker", "model", "serial", profile::IdentityStrength::hardware_serial};
    p.configuration.clock_source = "internal";
    p.configuration.sample_rate_hz = 768000;
    profile::FrequencySegment segment;
    segment.segment_id = "segment-1";
    segment.minimum_frequency_hz = 1'000'000.0;
    segment.maximum_frequency_hz = 30'000'000.0;
    segment.model_type = profile::SegmentModelType::linear;
    segment.reference_frequency_hz = 10'000'000.0;
    segment.intercept_error_hz = 21.0;
    segment.slope_ppm = 2.0;
    segment.uncertainty.coverage_factor = 2.0;
    segment.uncertainty.base_hz = 1.0;
    segment.uncertainty.included_components = {"reference"};
    p.segments = {segment};
    p.assurance.reference_class = core::ReferenceClass::authority_confirmed;
    p.assurance.reference_score_ceiling = 100;
    p.assurance.reliability_quotient = 92;
    p.assurance.limiting_components = {"evidence_completeness", "reference_provenance"};
    p.assurance.components = {{"reference_provenance", 92, "traceability"},
                              {"received_signal_suitability", 95, "accepted"},
                              {"device_binding", 96, "serial"},
                              {"environmental_validity", 94, "measured"},
                              {"observation_quality", 93, "accepted"},
                              {"model_quality", 97, "fit"},
                              {"artifact_integrity", 98, "digest"},
                              {"evidence_completeness", 92, "bundle"}};
    p.validity = {
        "2026-08-15T11:00:00Z", "2027-08-15T11:00:00Z", 0, {20.0, 10.0, 30.0, "enclosure"}};
    p.provenance.calibration_run_id = "run-1";
    p.provenance.software = {"SDR Calibration", "0.1.0"};
    p.provenance.reference_set = {{"ref-1",
                                   core::ReferenceClass::authority_confirmed,
                                   10'000'000.0,
                                   100,
                                   {},
                                   {},
                                   std::string(64, 'a')}};
    p.provenance.observation_ids = {"obs-1"};
    CHECK(profile::refreshIntegrity(p));
    return p;
}

void settingsTests() {
    using namespace sdrcal;
    const std::string input = "[Other]\nx=1\n[Configuration]\nCalibrationIntercept=-2.5\n"
                              "CalibrationSlopePPM=0.75\n";
    auto imported = wsjtx::importSettings(input, "2.7.0", "Default", "2026-08-15T13:00:00Z");
    CHECK(imported.succeeded());
    CHECK(imported.projection->informational_only);
    CHECK(imported.projection->artifact_sha256 == profile::sha256Hex(input));
    CHECK(imported.projection->loss.lossy);
    CHECK(imported.projection->intercept_hz == -2.5);
    CHECK(!imported.adapter_record->metadata.empty());
    CHECK(!wsjtx::importSettings(input, "2.7.0", "", "2026-08-15T13:00:00Z").succeeded());
    CHECK(!wsjtx::importSettings(input, "2.7.0", "Default", "not-a-time").succeeded());
    CHECK(!wsjtx::importSettings(input, "2.7.0", "Default", "2026-02-31T13:00:00Z").succeeded());
    CHECK(!wsjtx::importSettings(input, "2.8.0", "Default", "2026-08-15T13:00:00Z").succeeded());
    CHECK(
        !wsjtx::importSettings("[Configuration]\nCalibrationIntercept=nan\nCalibrationSlopePPM=1\n",
                               "2.7.0", "Default", "2026-08-15T13:00:00Z")
             .succeeded());
    CHECK(wsjtx::importSettings("[Configuration]\nCalibrationIntercept=1\nCalibrationIntercept="
                                "2\nCalibrationSlopePPM=1\n",
                                "2.7.0", "Default", "2026-08-15T13:00:00Z")
              .status == wsjtx::Status::ambiguous);

    auto p = profile();
    auto exported = wsjtx::exportSettings(p, "2.6.1", "Rig", "2026-08-15T13:00:00Z");
    CHECK(exported.succeeded());
    CHECK(std::abs(exported.projection->intercept_hz - 1.0) < 1e-12);
    CHECK(exported.projection->slope_ppm == 2.0);
    const auto round_trip =
        wsjtx::importSettings(exported.text, "2.6.1", "Rig", "2026-08-15T13:00:00Z");
    CHECK(round_trip.succeeded());
    CHECK(round_trip.projection->intercept_hz == exported.projection->intercept_hz);
    CHECK(round_trip.projection->slope_ppm == exported.projection->slope_ppm);
    CHECK(exported.adapter_record->compatibility->mapping_verified);
    p.segments.push_back(p.segments.front());
    CHECK(wsjtx::exportSettings(p, "2.7.0", "Rig", "2026-08-15T13:00:00Z").status ==
          wsjtx::Status::invalid_profile);
    p = profile();
    p.segments.front().model_type = profile::SegmentModelType::constant_ppm;
    CHECK(profile::refreshIntegrity(p));
    CHECK(wsjtx::exportSettings(p, "2.7.0", "Rig", "2026-08-15T13:00:00Z").status ==
          wsjtx::Status::unrepresentable);
}

void signTests() {
    using namespace sdrcal;
    auto p = profile();
    auto result = wsjtx::exportSettings(p, "2.7.0", "Rig", "2026-08-15T13:00:00Z");
    CHECK(result.succeeded());
    const double frequency_mhz = 20.0;
    const double wsjtx_error =
        result.projection->intercept_hz + result.projection->slope_ppm * frequency_mhz;
    const auto& segment = p.segments.front();
    const double native_error =
        segment.intercept_error_hz +
        segment.slope_ppm * (frequency_mhz * 1e6 - segment.reference_frequency_hz) / 1e6;
    CHECK(std::abs(wsjtx_error - native_error) < 1e-12);
    p.segments.front().intercept_error_hz = -21.0;
    p.segments.front().slope_ppm = -2.0;
    CHECK(profile::refreshIntegrity(p));
    result = wsjtx::exportSettings(p, "2.7.0", "Rig", "2026-08-15T13:00:00Z");
    CHECK(result.succeeded());
    CHECK(result.projection->intercept_hz < 0.0 && result.projection->slope_ppm < 0.0);
}

void fmtTests() {
    using namespace sdrcal;
    const std::string text = "19:17:59 750 1 1500 1497.074 -2.926 26.6 48.9\n"
                             "# 19:18:29 1090 1 1500 1500.094 0.094 33.1 42.0\n"
                             "malformed retained line\n";
    const auto result = wsjtx::importFmtAll(text, "2.7.0", "2026-08-15T13:00:00Z");
    CHECK(result.succeeded());
    CHECK(result.source_sha256 == profile::sha256Hex(text));
    CHECK(result.lines.size() == 3U);
    CHECK(result.lines[0].parsed && !result.lines[0].rejected_by_source);
    CHECK(result.lines[1].parsed && result.lines[1].rejected_by_source);
    CHECK(!result.lines[2].parsed && result.lines[2].manual_review_required);
    CHECK(result.lines[2].original_line == "malformed retained line");
    CHECK(!result.adapter_record->metadata.empty());
    CHECK(
        wsjtx::importFmtAll("29:99:99 750 1 1500 1497 -3 20 40\n", "2.7.0", "2026-08-15T13:00:00Z")
            .status == wsjtx::Status::unknown_format);
    CHECK(wsjtx::importFmtAll("not fmt data\n", "2.7.0", "2026-08-15T13:00:00Z").status ==
          wsjtx::Status::unknown_format);
    std::string large(wsjtx::maximum_input_bytes + 1U, 'x');
    CHECK(wsjtx::importFmtAll(large, "2.7.0", "2026-08-15T13:00:00Z").status ==
          wsjtx::Status::resource_limit);
}
} // namespace

int main() {
    settingsTests();
    signTests();
    fmtTests();
    if (failures)
        std::cerr << failures << " failure(s)\n";
    return failures == 0 ? 0 : 1;
}
