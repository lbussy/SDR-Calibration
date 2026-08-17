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

**Status: Implemented and hardware-free validated; physical combinations remain
qualified only by their separate retained evidence.**

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
on 2026-08-15 and the exact SDRplay RSP1B/macOS combination recorded on
2026-08-17.** See the [Airspy qualification record](qualification-gate-1-airspyhf.md)
and [RSP1B qualification record](qualification-gate-1-sdrplay-rsp1b.md).

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

**Status: Implemented and hardware-free validated with an injected fake-device
boundary; production persistence and device integration remain separate gates.**

Implement the common workflow coordinator used by every user interface:
discovery, configuration, acquisition, estimation, observation acceptance,
model fitting, profile generation, evidence production, and interoperability.

The implemented coordinator performs authenticated reference resolution, exact
device selection, bounded acquisition through an injected boundary, effective-
configuration verification, estimation, acceptance, linear fitting,
uncertainty and assurance calculation, native profile candidate generation,
bounded evidence candidate generation, cancellation, stage tracing, and
optional lossy WSJT-X export. It does not persist artifacts, access hardware,
or expose a production user interface. Local-constant profile generation
remains unavailable because the current native schema cannot truthfully encode
its zero-span validity domain.

Exit requires a complete deterministic workflow using fake devices and
fixtures without Qt, network access, or physical hardware.

## Phase 12: Production CLI

**Status: Implemented and hardware-free validated for recorded CF32LE and
explicit live-device input; production trust management and physical
calibration qualification remain separate gates.**

Deliver the supported command-line application over the shared services.
Define stable arguments, automation-safe exit statuses, progress reporting,
cancellation, diagnostics, and output behavior.

Exit requires the CLI and GUI to use the same application-service
contracts and product semantics.

The implemented `sdrcal calibrate` command accepts explicit, strict versioned
schemas for bounded digest-verified recorded observations and bounded live
acquisition, plus an independently selected local registry-signature pin file.
It emits canonical terminal JSON, stable exit categories, progress diagnostics,
cancellation, and a coordinated new-directory publication containing the
native profile, finalized evidence, reduced summary, and optional lossy WSJT-X
projection. Normal validation remains hardware-free; actual live use is a
separately authorized and qualified operation.

## Phase 13: Qt desktop application

**Status: Implemented and hardware-free validated for recorded- and live-input
workflows; production trust/profile management and physical calibration
qualification remain separate gates.**

Deliver the Qt 6 Widgets workflow for device selection, measurement review,
calibration, profile management, evidence review, and interoperability.

The implemented application explicitly selects and reviews a Phase 12 run
request, its independent trust-pin file, and a new output directory. It uses
the same production service as the CLI, runs asynchronously with cooperative
cancellation and live progress, and reviews the published native profile,
evidence, summary, and optional lossy WSJT-X projection. Device selection and
measurements are explicit in the selected request; profile management is
read-only inspection. Live requests require schema-aware review and deliberate
confirmation before the shared production service can access an SDR. The
application does not provide discovery-driven controls, mutate profiles or
installed applications, or provide production trust and signing.

Exit requires accessibility, cancellation, failure recovery, long-operation
behavior, and CLI/GUI alignment to be reviewed and tested. UI completion alone
does not establish calibration accuracy or device qualification.

## Phase 14: Portability, packaging, and qualification

**Status: Packaging foundation and exact-payload license-disposition gates are
implemented; exact same-host evidence and owner-confirmed clean-host lifecycle
passes are retained for the indexed macOS, Windows, and Raspberry Pi packages.
Upgrade, Windows Store certification/delivery, device, and end-to-end
qualification gates remain open.**

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

The implemented foundation provides strict dependency baselines, install rules,
portable CPack archives, a configured SPDX inventory, isolated package-content
audit, three-OS hardware-free CI definitions, an evidence-indexed support
matrix, and the full calibration qualification record contract. The passing
cross-platform run is retained in the matrix. The portable archives do not
deploy runtime dependencies or constitute native signed installers.

Phase 14.1 added the fail-closed macOS DMG production path: stable bundle
metadata, dynamic Qt deployment, Developer ID signing, notarization, stapling,
Gatekeeper checks, and hash-indexed evidence. The current exact DMG also passed
the Phase 14.4 license gate and a same-host lifecycle. The project owner later
confirmed a clean-host lifecycle pass for that indexed hash. Prior-version
upgrade remains open.

Phase 14.2 added the fail-closed optional Windows x64 MSI testing path: dynamic Qt
deployment, Authenticode signing, MSI construction, administrative extraction,
payload and startup auditing, and hash-indexed evidence. The current mode uses
a locally trusted self-signed development certificate without timestamping;
the retained public-trust mode requires RFC 3161 timestamping. A self-signed
result applies only to the exact retained MSI on systems where that certificate
is explicitly trusted and does not close public-trust or SmartScreen gates. The
retained exact MSI subsequently passed the Phase 14.4 license gate and a
same-host lifecycle, and the project owner confirmed a clean-host lifecycle
pass for that indexed hash. Decision 0025 later removed public-trust MSI signing
from the release gates; this self-signed MSI remains testing-only.

Phase 14.3 adds the fail-closed Ubuntu 24.04 x86_64 DEB production path:
system-package Qt dependencies derived from the staged ELF payload, rootless
archive construction, extracted-payload/runtime auditing, and hash-indexed
evidence. Its result applies only to an exact retained DEB and does not close
clean-install, repository, distribution-license, or general Ubuntu-support
gates.

Phase 14.4 binds license disposition to the exact staged payload. macOS and
Windows packages must carry complete hash-pinned corresponding Qt source, the
deployed-code inventory, license disposition, and replacement instructions.
Ubuntu records exact system dependency versions and copyright dispositions
while proving those libraries are not conveyed. Each artifact must retain its
own passing evidence; implementation does not retroactively qualify earlier
packages.

The Phase 14.5 pre-qualification branding slice prevents the internal
`sdrcal-gui` target name from becoming any platform's product identity. Package
definitions now use `SDR Calibration` for macOS, Windows, and Linux launchers,
shortcuts, and installed-application listings; the installed macOS bundle is
`SDR Calibration.app` while its internal executable remains `sdrcal-gui`.
Every GUI package definition carries the same approved, project-owned icon from
retained raster masters and reproducible `.icns`, `.ico`, executable/installer,
and Linux icon-theme generation. Provenance, conversion versions, SHA-256
records, installed evidence files, and hardware-free contract checks are
implemented. The exact renamed, icon-bearing macOS and Windows packages have
same-host execution evidence, and macOS has retained human visual review. The
project owner confirmed clean-host lifecycle passes for the exact indexed
macOS and Windows hashes. Prior-version transitions, Windows Store delivery, and
candidate binding remain open. The project owner accepted the Windows
application/icon at relevant scaling settings. Earlier packages remain
historical evidence.

The Raspberry Pi CLI track may advance before the Phase 14.5 multi-platform
clean-host matrix. Its Phase 1 source foundation provides a CLI-only ARM64
preset, a provisional fail-closed resource ceiling, payload absence checks, and
rootless Raspberry Pi OS DEB tooling. Raspberry Pi 4 fixture `wspr4` now supplies
a native passing build, measured 128 MiB-per-observation resource envelope, and
an audited exact ARM64 DEB. The project owner confirmed a clean-host lifecycle
pass for that indexed DEB hash. SoapySDR capture and end-to-end calibration
remain separate later gates.

## Live-device production integration

**Status: Contract, bounded in-memory acquisition, signal-quality analyzer,
injected hardware-free Soapy workflow boundary, production CLI integration,
and Qt GUI integration implemented; physical qualification not started.**

The [live-device calibration contract](live-device-calibration-contract.md)
defines the boundary among SoapySDR, bounded in-memory acquisition, versioned
signal-quality analysis, authenticated reference evidence, the shared workflow,
and coordinated artifact publication. It forbids treating operator-entered
acceptance metrics as application-derived evidence.

The Soapy-independent bounded in-memory acquisition component now enforces an
explicit memory ceiling, exact read bounds, cancellation, non-finite-sample and
stream-error rejection, exception containment, and known-safe cleanup before it
returns samples. The versioned signal-quality analyzer now derives coherent
residual SNR, normalized clipping, bounded-window frequency instability, and
the strongest non-carrier FFT component from deterministic in-memory inputs.
The injected workflow boundary now verifies stable identity and effective
configuration, preflights memory before device construction, composes bounded
acquisition with carrier estimation and the analyzer, maps missing/discontinuity
diagnostics only from stream evidence, and requires reverse-order known-safe
cleanup. The production CLI now selects this boundary only for the explicit
`sdrcal-live-calibration-request` schema, rejects live mode when Soapy support is
absent, derives diagnostics rather than accepting them from the request, and
uses the existing coordinated publication transaction. All validation remains
fake-API and hardware-free. The Qt application forwards the same injected
production boundary after schema-aware request review and explicit live-run
confirmation. Separately authorized physical-device qualification follows as a
distinct reviewed slice.

The first exact Airspy HF+ Discovery/macOS end-to-end candidate completed its
hardware-free readiness review and stopped before enumeration. Retained capture
evidence lacks required manufacturer/model, clock-source, and effective
frequency-correction readback for the fail-closed production boundary. The
[readiness record](airspyhf-end-to-end-readiness.md) identifies the blocker;
capture qualification does not transfer to end-to-end calibration.

The subsequent hardware-free adapter slice resolves that source blocker with
explicit provenance-bearing normalization and negative fake-driver tests. It
does not establish that the currently connected device reports the expected
capabilities or that either WWV observation is usable; serial-bound enumeration
and bounded reception remain separately authorized physical gates.

## Release candidate

**Status: Not ready to freeze.** See the
[release-candidate readiness audit](release-candidate-readiness.md) for the
current blocker ledger and ordered resumption path. The intended compatibility
promise and artifact set are accepted in
[decision 0022](decisions/0022-initial-compatibility-and-release-artifacts.md),
but have not been bound to a frozen candidate.

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
