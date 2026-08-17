# Implementation status

## Present

- Project layout and CMake presets
- MIT project licensing and third-party distribution policy
- Read the Docs-compatible Sphinx/MyST documentation scaffold
- Draft SDR Calibration Profile contract and JSON Schema
- Architectural boundaries for C++20, SoapySDR, Qt 6 Widgets, CLI, and WSJT-X
  adapters
- Contributor, formatting, static-analysis, and testing policy
- Accepted scope, claim-level, reference-assurance, uncertainty, validity,
  integrity, offline, evidence, and platform policies
- Planned bounded complex-IQ capture request, artifact, failure, and reusable
  component contracts
- Hardware-free capture request validation, planning, sample-source interface,
  bounded recorder, CF32LE writer, JSON manifest writer, and unit tests
- Conditional receive-only SoapySDR adapter with unambiguous selection,
  configuration/readback, CF32 stream lifecycle, error translation, and
  hardware-free fake-boundary tests
- Thin `sdrcal-capture` diagnostic CLI with explicit device selection, bounded
  request parsing, requested/effective reporting, cancellation, stable exit
  categories, and hardware-free parser and injected-workflow tests
- Qualification Gate 1 evidence for one bounded receive-only capture using the
  exact Airspy HF+ Discovery, SoapyAirspyHF, macOS, antenna, and settings recorded
  in the [qualification report](qualification-gate-1-airspyhf.md)
- Qualification Gate 1 evidence for one bounded receive-only capture using the
  exact SDRplay RSP1B, SoapySDRPlay3, macOS, antenna, and settings recorded in
  the [RSP1B qualification report](qualification-gate-1-sdrplay-rsp1b.md); the
  retained antenna sample was separately rejected as an ambiguous signal
- Hardware-free carrier estimator for in-memory complex samples, including
  midpoint frequency, linear drift, explicit failure states, diagnostics, and
  deterministic synthetic golden fixtures
- Versioned, hardware-free observation-acceptance policy with deterministic
  thresholds, fail-closed configuration and reference checks, and reviewable
  rejection codes and reasons
- Hardware-free frequency-error fitting and bounded evaluation for local and
  linear models, with accepted-observation and independence provenance
- Versioned uncertainty-budget combination with explicit correlation-matrix
  validation, coverage factor, and incomplete-component flag
- Versioned reliability-quotient scoring with the complete required component
  set, weakest-component rule, and reference-class ceilings
- Hardware-free native profile domain types, structural and cross-field
  validation, deterministic RFC 8785 serialization, SHA-256 integrity,
  injected Ed25519 signing and verification hooks, revocation-aware bounded
  evaluation, schema-major compatibility checks, and atomic replacement
- Hardware-free reference-registry domain types, semantic validation,
  deterministic serialization and integrity, injected signature verification,
  atomic activation, separate local overlays, offline resolution, expiry, and
  assurance-ceiling enforcement
- Hardware-free bounded evidence-bundle manifests with explicit reduced,
  raw-IQ, and expanded-diagnostic classes, privacy filtering, byte and time
  limits, partial-failure reporting, and atomic-write status
- Hardware-free WSJT-X 2.6.x/2.7.x adapter core with bounded settings import
  and deterministic single-linear-segment export, explicit loss reports,
  verified sign-convention fixtures, source hashes, and line-preserving
  eight-column `fmt.all` import with manual-review states
- Hardware-free shared calibration workflow with injected exact-device
  discovery/acquisition, authenticated reference resolution, effective-setting
  checks, estimation and acceptance, linear fitting, uncertainty and assurance,
  validated native profile and bounded evidence candidates, cancellation and
  stage tracing, reduced-assurance handling, and optional lossy WSJT-X export
- Synthetic end-to-end workflow qualification through the production profile
  consumer boundary, including ordered stage completion, semantic and SHA-256
  integrity, evidence digest linkage, finite model and uncertainty output,
  reliability-quotient policy output, in-domain evaluation and sign convention,
  and fail-closed tampering and extrapolation. This makes no physical accuracy
  or reference-suitability claim.
- Documentation-only GPSDO and reference-path readiness package covering exact
  source and receiver dossiers, clock-versus-RF topology selection, worst-case
  level and attenuation calculations, component inventory, pre-connection
  review, bounded-plan fields, abort/cleanup, evidence, and stop criteria. The
  physical path remains explicitly not ready and no reference suitability or
  calibration claim is established.
- Production `sdrcal calibrate` CLI for strict, bounded, digest-verified
  recorded CF32LE observations, with an independent local registry-signature
  pin file, canonical terminal JSON, stable exit categories, progress,
  cancellation, and coordinated new-directory artifact publication
- Qt 6 Widgets `sdrcal-gui` application over the same recorded/live production
  service, with explicit path selection, schema-aware bounded review,
  unchanged-request gating, deliberate live-run confirmation, responsive
  execution, cooperative cancellation, live progress, accessible controls, and
  read-only published-artifact review
- Phase 14 packaging foundation with explicit CMake 3.20, Qt 6.2, and SoapySDR
  0.8 baselines; strict full-release dependency checks; install rules; portable
  CPack archives; deterministic configured SPDX 2.3 inventory; isolated staged-
  package audit; and macOS/Windows hardware-free CI definitions
- Evidence-indexed platform/device matrix and a fail-closed end-to-end
  calibration qualification record that keeps build, package, signing, capture,
  and calibration claims separate
- Fail-closed macOS DMG tooling with stable bundle metadata, dynamic Qt
  deployment, Developer ID signing, notarization, stapling, Gatekeeper checks,
  runtime-path audit, and retained hash/evidence output
- Fail-closed Windows x64 MSI tooling with dynamic Qt deployment, explicit
  locally trusted self-signed and future public-trust Authenticode modes,
  extracted-payload audit, and retained hash/environment evidence; only the
  public-trust mode requires RFC 3161 timestamp verification
- Fail-closed Ubuntu 24.04 x86_64 DEB tooling with Debian-derived system Qt
  dependencies, extracted-payload/runtime audit, and retained hash/environment
  evidence
- Fail-closed Phase 14.4 exact-payload license disposition: bundled macOS and
  Windows Qt requires complete hash-pinned corresponding source, runtime
  inventory, no-modification record, and replacement guidance; Ubuntu retains
  exact system-package versions and Debian copyright dispositions
- Cross-platform GUI product identity established in package definitions:
  `SDR Calibration.app` on macOS with internal `sdrcal-gui`, an icon-bearing
  Windows installed-app entry and Start menu shortcut named `SDR Calibration`,
  and a freedesktop `SDR Calibration` launcher with resolved hicolor icons
- Project-owned icon provenance, retained raster masters, reproducible native
  asset generation, conversion-tool versions, SHA-256 manifest, installed
  provenance records, and deterministic hardware-free branding checks; the
  macOS path now uses one Icon Composer source for the macOS 26 `Assets.car`
  rendition and its generated pre-26 ICNS, with packaged visual qualification
  on both OS generations still open
- Exact macOS ARM64 signed/notarized DMG qualification at source `5eb3670`,
  including payload/license gates and a reversible same-host install, first
  launch, normal relaunch, CLI help, removal, and restoration lifecycle
- Exact Windows 11 x64 self-signed MSI qualification at source `15ed17b`,
  including payload/license gates and same-host install, launch,
  maintenance-install, and uninstall lifecycle; this MSI is optional testing
  evidence and is not a required release artifact
- Exact Windows 11 x64 Store-MSIX native qualification at source `99d96a6`,
  including 19/19 hardware-free tests, package/runtime/license audit, MakeAppx
  validation, and same-host development-signed install, CLI alias, GUI
  launch/relaunch, uninstall, and certificate cleanup; Store submission,
  certification, Microsoft signing, and Store delivery remain open
- Prepared offline Microsoft Store submission-readiness packet with proposed
  English listing copy, `runFullTrust` justification, preliminary privacy and
  age-rating assessments, screenshot plan, and fail-closed preflight; no
  Partner Center edit, package upload, certification request, or publication
  has occurred
- Owner-confirmed clean-host lifecycle passes for the exact indexed macOS DMG,
  self-signed Windows MSI, and Raspberry Pi OS 13 ARM64 CLI DEB hashes; Windows
  application/icon review at relevant scaling settings also passed; Windows
  Store certification/delivery and applicable prior-version transitions remain
  open
- Accepted initial compatibility and release-artifact boundary: schema-major-1
  native-profile semantics are durable; intended binaries are recorded-input-
  only macOS ARM64 DMG, Microsoft Store Windows x64 MSIX, and Raspberry Pi OS 13
  ARM64 CLI DEB with required source, license, checksum, and evidence materials
- Fail-closed initial release-documentation templates for notes, known
  limitations, upgrade/rollback, vulnerability reporting, checksums, and the
  evidence ledger; final candidate population and approval remain open
- Exact host-local, read-only, content-addressed `0.1.0` macOS DMG and Raspberry
  Pi DEB upgrade baselines plus an optional Windows MSI migration fixture; no
  upgrade, rollback, release, or off-host-backup claim
- Prepared upgrade/rollback qualification procedures for macOS bundle
  replacement/restoration, Windows Store update/no-predecessor and optional-MSI
  migration/coexistence, and Raspberry Pi `dpkg` upgrade/downgrade, with seeded-
  state preservation, fail-closed cleanup, and independent-review criteria;
  execution remains open until a version-consistent candidate above `0.1.0`
  is frozen
- Prepared release evidence package assembly plan with deterministic public
  layout, public/private evidence separation, manifest and SHA-256 rules,
  split qualification cells, privacy review, change control, independent
  verification, and fail-closed completion states; candidate values and final
  assembly remain open
- Centralized production version identity on the configured CMake project
  version for both CLIs, the Qt GUI, platform packages, CPack, and SPDX output,
  with hardware-free source-contract and CLI runtime tests; version `0.1.1` is
  selected and bound to an exact Windows MSIX hash for Store-candidate
  preparation but is not designated as a release candidate
- Clean synchronized Windows construction of the unsigned `0.1.1` Store MSIX
  at source `2e678b8`, including 19/19 hardware-free tests, package/runtime and
  license audit, MakeAppx creation/unpack inspection, exact artifact hash, and
  a validated synthetic Windows certification fixture; WACK, exact-hash local
  lifecycle, screenshots, Store submission, and Microsoft signing remain open
- Public repository with GitHub Private Vulnerability Reporting enabled;
  retained non-vulnerability evidence covers the maintainer-side advisory
  create/retrieve/close control and one eligible external reporter's private
  ingress through maintainer receipt, acknowledgement, closure, and independent
  final-state retrieval. The finalized security policy defines latest-stable
  support, response targets, disclosure, CVE, credit, privacy, retention, and
  the explicit absence of a backup security maintainer; candidate-specific
  access revalidation remains open
- Hardware-free Raspberry Pi CLI Phase 1 source foundation: a CLI-only ARM64
  preset, configurable fail-closed recorded-input ceiling, deterministic limit
  test, GUI/Qt/SoapySDR-free package audits, and rootless Raspberry Pi OS DEB
  tooling
- Frozen production live-device calibration contract covering exact identity,
  requested/effective settings, bounded in-memory acquisition, acceptance-input
  provenance, cancellation, cleanup, evidence, and a gated hardware-free
  implementation sequence; live CLI and GUI wiring are implemented while
  physical qualification remains open
- SoapySDR-independent bounded in-memory acquisition with explicit memory
  limits, exact read bounds, non-finite-sample rejection, cancellation,
  exception containment, stream statistics, known-safe cleanup enforcement, and
  deterministic hardware-free tests
- Versioned hardware-free signal-quality analyzer deriving coherent residual
  SNR, normalized clipping, bounded-window frequency instability, and strongest
  non-carrier FFT component with deterministic synthetic golden fixtures
- Injected hardware-free Soapy workflow boundary composing exact device
  selection, requested/effective readback, preflighted bounded in-memory
  acquisition, carrier estimation, versioned signal-quality metrics,
  stream-derived missing/discontinuity evidence, explicit reference-condition
  evidence, and reverse-order known-safe cleanup. The shared workflow now
  requires the returned carrier estimate to be bound to the exact sample count.
- Production CLI live-request wiring with an explicit schema, no recorded/live
  fallback, strict bounded acquisition inputs, injected boundary construction,
  authenticated-registry condition evidence, coordinated publication, and
  hardware-free frontend tests. Physical execution remains unqualified.
- Qt desktop live-request wiring through the same production service, including
  schema-aware review, unchanged-request gating, explicit live-run confirmation,
  injected production-boundary construction, and hardware-free widget tests.
  Physical execution remains unqualified.
- Hardware-free readiness review for the exact Airspy HF+ Discovery/macOS
  end-to-end candidate. It stopped before enumeration because retained driver
  evidence cannot establish all identity/configuration fields required by the
  fail-closed live boundary; physical execution remains not started.
- Provenance-bearing AirspyHF normalization for sparse Soapy metadata, including
  exact family policy, explicit driver-default clock classification, unsupported
  correction as effective zero, capture-manifest capability fields, and
  negative fake-API cases. This source-only slice made no physical claim.
- Exact-key SDRplay RSP1B identity normalization for sparse generic
  manufacturer/model metadata, with a versioned profile binding and
  hardware-free positive and partial-key negative tests. Effective bandwidth,
  aggregate gain, AGC, clock, and correction remain readback-derived rather
  than device defaults; this source-only slice makes no new physical claim.
- Exact 10 MHz Airspy diagnostic transport passed with known-safe cleanup, but
  the immutable production estimator rejected the retained sample as
  `ambiguous_signal` at 0.2596 coherence. WWV suitability and end-to-end
  calibration remain unqualified; no 15 MHz run was attempted.

## Planned, not implemented

- Production Ed25519 key-provider and trust-store integration
- Profile-evaluation orchestration for production clients
- Network reference-registry transport and production trust-store integration
- Additional real-device SoapySDR combinations and end-to-end calibration
  qualification
- Production profile activation, signing, revocation, replacement, and
  evaluation orchestration in desktop clients
- Safe WSJT-X instance discovery, backup, locking, atomic settings replacement,
  and readback verification
- Evidence archive/container production beyond the Phase 12 coordinated
  profile/evidence/summary directory
- Native-profile representation for zero-span local-constant fitted models
- Binding the accepted compatibility/artifact policy to an exact candidate,
  plus release documentation and its evidence ledger; see the
  [readiness audit](release-candidate-readiness.md)
- Applicable prior-version transition evidence for the frozen candidate on
  macOS and Raspberry Pi, verified first-Store-release N/A or a future Store
  update result on Windows, and optional-MSI migration/coexistence evidence

## Not established

- Current Ubuntu platform, package, installation, device, or calibration support;
  retained Ubuntu implementation and evidence are historical portability assets
- Supported SDR device matrix
- An end-to-end calibration-qualified Raspberry Pi combination
- Calibration accuracy claims
- Release schedule or stable artifact compatibility guarantee

Source scaffolding, successful CMake configuration, and schema syntax checks do
not establish any calibration or hardware capability.
