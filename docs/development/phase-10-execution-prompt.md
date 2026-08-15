# Phase 10 execution prompt: WSJT-X interoperability

## Objective

Implement the roadmap's Phase 10 WSJT-X settings and `fmt.all` adapters as a
deterministic C++20 library. Keep the native SDR Calibration Profile
authoritative and make every conversion explicitly lossy, reviewable, and
hardware-free.

## Governing contracts

Follow the profile contract sections on artifact separation, frequency-error
sign, adapter records, WSJT-X interoperability, and round trips; ADR 0002; the
application architecture; the failure-state model; and the project
instructions. Support only WSJT-X 2.6.x and 2.7.x mappings verified by Phase 10
fixtures. Other versions require rejection rather than guessed compatibility.

## Required implementation

1. Add an `sdrcal_wsjtx` library below `src/adapters/wsjtx/`, depending inward
   on profile-domain and integrity helpers but not Qt, SoapySDR, network access,
   operating-system paths, or application services.
2. Parse WSJT-X INI text without silently accepting missing, duplicate,
   non-finite, out-of-range, or malformed calibration keys. Preserve the source
   digest, application version, configuration name, import time, parsed values,
   compatibility decision, and information-loss report.
3. Do not manufacture a qualification-capable native profile from settings.
   Represent an import as an informational projection because device identity,
   frequency domain, validity, uncertainty, provenance, and integrity evidence
   are absent.
4. Export only a validated native profile having exactly one linear segment.
   Algebraically project its reference-centered native model to WSJT-X's
   `dial_error_hz = intercept_hz + slope_ppm * frequency_mhz` form. Reject
   multiple segments, constant-ppm or unknown models, non-finite output, and
   unverified WSJT-X versions.
5. Emit deterministic settings projection text containing only the
   configuration section and two calibration keys. Application services must
   later preserve unrelated settings when applying that projection. This phase
   does not discover paths, inspect processes, or replace live files.
6. Mark every settings import/export as lossy and enumerate omitted native
   capabilities. Produce adapter records with source/destination SHA-256 and a
   verified sign-mapping fixture identifier.
7. Import bounded `fmt.all` text line by line. Preserve every original line,
   parse the supported eight-column FreqCal layout, preserve explicit rejection
   markers, and mark malformed or partially understood lines for manual review.
   Reject an artifact as unknown when it contains data but no recognized line.
8. Validate numeric ranges and bound input bytes and line count before parsing.
   Never infer uncertainty, device identity, calibration time, or acceptance
   from decimal precision, file timestamps, or WSJT-X markings.
9. Add deterministic tests for positive and negative sign cases, reference-
   frequency intercept conversion, import/export round trip, version rejection,
   malformed and duplicate INI keys, unrepresentable profiles, source hashes,
   valid/rejected/malformed `fmt.all` lines, unknown format, and resource bounds.
10. Update roadmap, status, user guidance, testing documentation, and source
    layout descriptions to distinguish implemented adapter-core behavior from
    deferred safe file replacement and application integration.

## Non-goals

- No WSJT-X process detection, default-path discovery, filesystem backup,
  locking, atomic replacement, live interface, or automatic settings mutation.
- No application-service, production CLI, or Qt integration.
- No model fitting from `fmt.all`, native-profile generation from external
  settings, or replacement of a last known-good profile.
- No SDR access, RF testing, network access, or WSJT-X installation/runtime
  dependency.

## Validation and exit gate

Run the default configure, build, and CTest presets; a SoapySDR-disabled build
and tests; Sphinx with warnings as errors; formatting/diff checks; and an
adversarial review against this prompt and the governing contracts. Correct
every material finding and repeat review and validation until no blocker
remains. Phase 10 may be marked implemented and hardware-free validated only
when this gate passes. Do not claim safe live-file modification, workflow
integration, device support, RF qualification, or calibration accuracy.

## Adversarial findings injected into the exit gate

The first review required these additional invariants:

- Adapter records retain the settings configuration, parsed numeric values,
  informational-only state, and `fmt.all` line/manual-review counts rather than
  leaving required import/export detail only in an ephemeral result object.
- Empty configuration names and malformed creation timestamps fail before an
  adapter record can be produced; timestamp validation includes calendar dates.
- `fmt.all` time fields require real `HH:MM:SS` digit and range validation, not
  only delimiter and width checks.
- Phase 10 emits a projection artifact; preservation of unrelated settings and
  safe installed-file replacement remain explicitly deferred.
