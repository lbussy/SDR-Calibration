# Development roadmap

This roadmap defines the gated path from the completed capture foundation to
an initial SDR Calibration release. It is an overall sequence, not a release
schedule and not authorization to begin a phase. Each phase starts from the
then-current repository state and requires its own reviewed scope.

Implementation status, future plans, qualification, and release claims remain
distinct. A successful hardware-free test does not establish device support,
and a successful test with one physical device qualifies only the exact tested
combination and conditions.

## Phase workflow

New phases normally use the following review cycle:

1. Render a research and contract prompt from the current committed state.
2. Review and approve its exact scope.
3. Render and execute an implementation prompt for only the approved slice.
4. Render and execute an independent adversarial-review prompt.
5. Correct findings and run the applicable validation.
6. Review the complete diff and retained evidence.
7. Separately authorize integration and publication actions.
8. Render the next phase prompt from a clean, synchronized state.

Prompts are working instructions, not durable product contracts. Durable
architectural, artifact, licensing, and compatibility decisions belong in the
project documentation and decision log.

Hardware access, packaging, signing, publication, and release require separate
bounded authorization. Normal validation remains hardware-free, network-free,
and safe to repeat.

## Phase 1: Bounded complex-IQ capture contract

**Status: Complete.**

Define the reusable, single-channel capture contract: headerless CF32LE sample
data, a versioned JSON manifest, requested and effective settings,
strict/permissive readback policy, resource limits, partial-artifact handling,
cleanup evidence, and a hardware gate. Record the durable artifact decision.

Exit evidence establishes the capture contract without accessing or claiming
support for an SDR.

## Phase 2: Hardware-free capture foundation

**Status: Complete.**

Implement the SoapySDR-independent capture library: request validation,
planning, the normalized `SampleSource` boundary, bounded recording, CF32LE and
manifest writers, safe incomplete artifacts, cleanup reporting, and
deterministic unit tests.

Exit requires the default and SoapySDR-disabled configurations to build and
pass their hardware-free tests. This establishes the reusable capture
foundation, not device support.

## Phase 3: SoapySDR adapter

**Status: In progress; not yet accepted.**

Implement deterministic device selection, construction, configuration,
readback, stream setup, activation, sample-read translation, cleanup, metadata
capture, and injectable fake-API tests behind the SoapySDR boundary.

Exit requires independent review, successful SoapySDR-enabled and disabled
builds, accurate documentation, and hardware-free tests covering configuration,
readback, lifecycle, error, and cleanup paths. No physical SDR is required or
qualified by this phase.

## Phase 4: Diagnostic capture CLI

**Status: Implemented and hardware-free validated; real-device use remains gated.**

Build a thin diagnostic command-line interface over the capture and SoapySDR
components. It must expose explicit device selection, channel, frequency,
sample rate, bandwidth, gain policy, capture bounds, and output paths while
reporting requested and effective settings distinctly.

Exit requires hardware-free parser and workflow tests. Invalid, ambiguous, or
unbounded requests must fail before device construction.

## Qualification gate 1: First bounded SDR capture test

**Status: Passed for the exact Airspy HF+ Discovery/macOS combination recorded
on 2026-08-15.** See the [qualification record](qualification-gate-1-airspyhf.md).

Exercise only the capture stack with one exact receive-only SDR, driver, input
arrangement, configuration, duration, abort procedure, cleanup procedure, and
evidence location under a separately authorized test plan. This gate does not
exercise or qualify frequency estimation, model fitting, uncertainty, or
profile generation.

Exit requires verified cleanup and retained evidence. A successful result
qualifies only the tested device, driver, platform, settings, and conditions; it
does not establish a general supported-device matrix.

## Phase 5: Carrier estimator

**Status: Implemented and hardware-free validated; recorded-input review and
observation acceptance remain separate gates.**

Estimate carrier frequency from deterministic synthetic and reviewed recorded
complex-IQ inputs. Keep the estimator independent of SoapySDR, Qt,
operating-system paths, and calibration-profile serialization.

Exit requires golden fixtures covering known frequency offsets, drift, noise,
clipping, interference, and unsuitable signals.

## Phase 6: Observation acceptance

**Status: Implemented and hardware-free validated; external metric derivation,
recorded-input review, and model integration remain separate gates.**

Implement deterministic observation-quality policy for signal-to-noise ratio,
clipping, discontinuities, instability, interference, effective-configuration
validity, and reference suitability.

Exit requires every rejected observation to carry a reviewable reason. An
unusable or unverifiable observation must not reach model fitting.

## Phase 7: Model and uncertainty

**Status: Implemented and hardware-free validated; application integration,
recorded-observation review, and calibration qualification remain separate
gates.**

Implement the supported frequency-error model, uncertainty budget, validity
range, extrapolation prohibition, and versioned reliability-quotient scoring.

Exit requires reviewed mathematical golden cases and boundary tests. The
implementation and documentation must keep measurement uncertainty, evidence
assurance, and the reliability quotient conceptually distinct; the quotient is
not a probability or confidence level.

## Phase 8: Native profile engine

**Status: Implemented and hardware-free validated; application integration,
real signing-provider integration, and calibration qualification remain separate
gates.**

Implement authoritative calibration-profile domain types, semantic validation,
JSON serialization, RFC 8785 canonicalization, SHA-256 integrity, signing and
verification hooks, revocation semantics, profile evaluation, compatibility
handling, and atomic replacement.

Exit requires schema and cross-field semantic tests. Failure must preserve the
last known-good profile. `target_frequency_hz` remains part of an application
or evaluation record rather than the semi-durable device profile.

## Phase 9: References and evidence

**Status: Implemented and hardware-free validated; network transport,
production trust-store integration, application integration, and calibration
qualification remain separate gates.**

Implement reference manifests, reference-registry handling, locally generated
and ad-hoc reference classes, provenance, evidence bundles, privacy controls,
assurance ceilings, authenticated registry updates, and offline behavior.

Exit requires fail-closed handling when identity, validity, integrity,
uncertainty, provenance, or required reference evidence cannot be established.
Local and ad-hoc references remain usable only within their documented class,
conditions, limitations, and reliability-quotient ceiling.

## Phase 10: WSJT-X interoperability

**Status: Implemented and hardware-free validated; safe settings-file
replacement and application integration remain separate gates.**

Implement explicitly lossy WSJT-X settings and `fmt.all` import/export adapters
with sign-convention tests, information-loss reporting, source hashes, and
unknown-format rejection.

The implemented adapter core supports verified WSJT-X 2.6.x and 2.7.x mappings,
bounded INI and eight-column `fmt.all` parsing, deterministic settings
projection, source hashes, preserved source lines, explicit manual-review
states, and loss reports. It does not discover or modify installed WSJT-X
instances, fit a native model from imported observations, or create an
authoritative profile from external settings.

Exit requires the native SDR Calibration Profile to remain authoritative.
Malformed, unknown, or ambiguous external data must not silently replace or
weaken it.

## Phase 11: Shared application services

Implement the common workflow coordinator used by every user interface:
discovery, configuration, acquisition, estimation, observation acceptance,
model fitting, profile generation, evidence production, and interoperability.

Exit requires a complete deterministic workflow using fake devices and
fixtures without Qt, network access, or physical hardware.

## Phase 12: Production CLI

Deliver the supported command-line application over the shared services.
Define stable arguments, automation-safe exit statuses, progress reporting,
cancellation, diagnostics, and output behavior.

Exit requires the CLI and planned GUI to use the same application-service
contracts and product semantics.

## Phase 13: Qt desktop application

Deliver the Qt 6 Widgets workflow for device selection, measurement review,
calibration, profile management, evidence review, and interoperability.

Exit requires accessibility, cancellation, failure recovery, long-operation
behavior, and CLI/GUI alignment to be reviewed and tested. UI completion alone
does not establish calibration accuracy or device qualification.

## Phase 14: Portability, packaging, and qualification

Establish supported macOS, Linux, and Windows builds; minimum Qt and SoapySDR
baselines; dependency pinning; dependency inventories or SBOMs; third-party
notices; installers; applicable platform signing; and the supported-device and
operating-system matrix.

Exit requires clean release builds and the applicable baseline, focused,
integration, installer, licensing, and platform checks. Every claimed device
and platform combination requires retained qualification evidence. Passing
application tests alone does not satisfy the distribution licensing gate.
End-to-end calibration qualification must separately exercise acquisition,
estimation, acceptance, fitting, uncertainty, profile generation, and profile
evaluation against an authoritative reference. Capture-only qualification from
the earlier hardware gate cannot satisfy this requirement.

## Release candidate

Freeze a source revision and the initial schema and artifact compatibility
policy. Complete release notes, known limitations, upgrade and rollback
guidance, supported offline-update behavior, vulnerability-reporting guidance,
checksums, dependency inventory, clean-install tests, representative tests on
qualified devices, and the release evidence ledger.

Exit requires no unresolved release blocker, exact agreement between
documentation and binaries, and successful fresh-install and clean-user-state
verification.

## Initial release

Publish exactly the reviewed candidate with source, binaries, notices,
corresponding-source disposition, checksums, compatibility statement,
supported-device matrix, and known limitations.

Release is complete only after the published artifacts are downloaded and
independently verified against the approved candidate and checksums.
