# SDR Calibration Application Architecture

Status: Draft 0.1

## 1. Product target

The project will produce a multi-platform C++ application that runs on macOS,
Linux, and Windows. It will use SoapySDR as its SDR support boundary.

The application will:

1. Discover and identify supported SDR devices through SoapySDR.
2. Capture the active device configuration that can affect frequency results.
3. Acquire samples from authoritative reference transmissions.
4. Estimate carrier frequency and reject unsuitable observations.
5. Fit a frequency-error model and calculate its uncertainty.
6. Write a validated SDR Calibration Profile.
7. Import and export supported interoperability formats, initially WSJT-X.
8. Produce an auditable evidence bundle for each calibration run.

The application will not embed vendor-specific SDR APIs in the calibration
engine. Device-specific accommodations must remain behind the SoapySDR adapter
or in explicit, testable capability policies.

## 2. Portability requirements

- Implementation language: C++20.
- Build system: CMake.
- SDR interface: SoapySDR public API.
- GUI toolkit: Qt 6 Widgets.
- Supported operating systems: macOS, Linux, and Windows.
- Native profile encoding: JSON conforming to the published JSON Schema.
- Platform-specific paths, locking, and atomic replacement remain isolated from
  calibration and profile-model code.
- The core library must not depend on a GUI toolkit.

The Qt 6 Widgets GUI and the terminal interface call the same application
services and calibration engine. Qt must not leak into the calibration core.

## 3. Component boundaries

```text
User interface or CLI
        |
Application services
        |-- update and reference-registry service
        |-- calibration workflow coordinator
        |-- profile import/export service
        |-- evidence-bundle service
        |
Calibration core
        |-- carrier estimator
        |-- observation acceptance policy
        |-- model fitter
        |-- uncertainty evaluator
        |-- profile evaluator
        |
SDR boundary
        |-- SoapySDR device discovery
        |-- SoapySDR configuration snapshot
        |-- SoapySDR stream acquisition
        |-- capability normalization
        |
External adapters
        |-- WSJT-X settings
        `-- WSJT-X fmt.all
```

The calibration core consumes normalized observations. It must not call
SoapySDR directly, inspect WSJT-X files, or depend on operating-system paths.

The initial GUI dependency set is Qt Core, Widgets, Network, and Concurrent,
with OpenGLWidgets added only if measurement-display performance requires it.
The application dynamically links Qt by default. GPL-only Qt modules are not
part of the initial architecture.

## 4. Proposed source layout

```text
CMakeLists.txt
cmake/
src/
  core/
  capture/
  soapy/
  application/
  platform/
  adapters/wsjtx/
  cli/
schemas/
docs/
tests/
  unit/
  integration/
  fixtures/
```

This is a target layout, not an instruction to create empty production
components before their contracts and tests exist.

`capture/` owns Soapy-independent request/result types, the normalized sample
source interface, bounded recording, and capture-artifact writers. `soapy/`
implements that source interface. The detailed planned contract is defined in
[Complex-IQ Capture Contract](capture-contract.md).

## 5. SoapySDR contract

The SoapySDR layer is responsible for:

- enumerating devices and preserving the complete discovery arguments;
- obtaining stable serial, hardware key, driver key, and hardware information
  when the driver exposes them;
- enumerating clock sources, antennas, gains, sample rates, bandwidths, and
  frequency ranges;
- applying a requested configuration and reading back the effective values;
- detecting driver-applied frequency correction;
- activating and deactivating streams with bounded cleanup;
- timestamping sample blocks and reporting overflows, timeouts, and discontinuities;
- converting supported native stream formats into the normalized sample format
  accepted by the calibration core.

The layer must preserve the difference between a requested setting and the
effective setting reported by the driver. Unsupported or unverifiable settings
must not be silently represented as successful.

## 6. Device identity

SoapySDR device discovery information is evidence, not automatically a globally
unique identity. The application derives the profile's `identity_strength`
according to documented rules:

- hardware-provided immutable serial: `hardware_serial`;
- stable serial reported only through the driver: `driver_serial`;
- operator-created identifier bound to a stored device fingerprint:
  `operator_assigned`;
- no defensible distinction between devices: `insufficient`.

The complete discovery fingerprint is retained in profile provenance or its
evidence bundle. A consumer must not match devices by display label alone.

## 7. Calibration workflow

The initial workflow is:

1. Load and verify the reference-station manifest and registry.
2. Discover the selected SDR and record its identity and capabilities.
3. Apply the requested SDR configuration and read back effective settings.
4. Enforce warm-up and environmental requirements.
5. Acquire bounded observations of selected reference carriers.
6. Preserve raw acquisition metadata and estimator diagnostics.
7. Reject interference, clipping, discontinuities, inadequate SNR, and unstable
   observations using deterministic policies.
8. Fit the supported model without extrapolating beyond accepted observations.
9. Calculate the uncertainty budget.
10. Generate and validate the profile against the bundled schema.
11. Write the profile and evidence bundle atomically.
12. Optionally create a separately recorded, lossy WSJT-X projection.

Failure at any step must not overwrite the last known-good profile.

The Phase 11 shared service implements this sequence through an injected device
boundary and returns validated native-profile and evidence candidates. It does
not choose filesystem destinations or claim a coordinated durable write; those
production policies remain outside the hardware-free coordinator. The service
currently produces linear native profiles only because schema 1.0 cannot
represent a zero-span local-constant model without inventing a validity range.

## 8. External interoperability

The native SDR Calibration Profile is authoritative. `wsjt-x-settings` and
`wsjt-x-fmt-all` are adapter formats governed by the main contract.

External adapters depend on profile-domain types but do not define them. An
adapter must report information loss, preserve source hashes, and fail closed on
unknown format revisions or unverified sign conventions.

## 9. Testing requirements

The design must support testing without attached SDR hardware.

Required test surfaces include:

- deterministic synthetic complex-IQ fixtures;
- a fake normalized sample source for calibration-core tests;
- model fitting and uncertainty golden cases;
- device identity and configuration-matching cases;
- profile JSON Schema validation cases;
- positive and negative WSJT-X sign-convention fixtures;
- malformed and partially understood `fmt.all` fixtures;
- mocked SoapySDR discovery and stream failures;
- opt-in integration tests for installed SoapySDR modules and real hardware.

Normal unit tests must not tune hardware, start streams, transmit RF, or require
a vendor driver.

## 10. Dependency direction

Dependencies point inward:

```text
UI/CLI -> application services -> calibration core
Capture CLI -> capture service -> capture recorder and artifact writers
SoapySDR adapter -------------> normalized core interfaces
SoapySDR RX adapter ----------> capture sample-source interface
WSJT-X adapters --------------> profile and observation interfaces
```

The calibration core must build and run its unit tests without SoapySDR,
network access, WSJT-X, or physical hardware. This keeps the scientific model
independently reviewable and allows consumers to reuse the profile evaluator
without acquiring samples.

The calibration core does not depend on the capture component. Capture output is
diagnostic evidence and does not become a calibration profile without a later
application workflow that evaluates and records it under the profile contract.
