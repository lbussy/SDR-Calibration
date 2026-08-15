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
- Production `sdrcal calibrate` CLI for strict, bounded, digest-verified
  recorded CF32LE observations, with an independent local registry-signature
  pin file, canonical terminal JSON, stable exit categories, progress,
  cancellation, and coordinated new-directory artifact publication
- Qt 6 Widgets `sdrcal-gui` application over the same recorded-input production
  service, with explicit path selection, bounded request/device/measurement
  review, responsive execution, cooperative cancellation, live progress,
  accessible controls, and read-only published-artifact review
- Phase 14 packaging foundation with explicit CMake 3.20, Qt 6.2, and SoapySDR
  0.8 baselines; strict full-release dependency checks; install rules; portable
  CPack archives; deterministic configured SPDX 2.3 inventory; isolated staged-
  package audit; and macOS/Linux/Windows hardware-free CI definitions
- Evidence-indexed platform/device matrix and a fail-closed end-to-end
  calibration qualification record that keeps build, package, signing, capture,
  and calibration claims separate

## Planned, not implemented

- Production Ed25519 key-provider and trust-store integration
- Profile-evaluation orchestration for production clients
- Network reference-registry transport and production trust-store integration
- Additional real-device SoapySDR combinations and end-to-end calibration
  qualification
- Live-device integration for the production command-line application
- Live-device integration for the Qt desktop application
- Production profile activation, signing, revocation, replacement, and
  evaluation orchestration in desktop clients
- Safe WSJT-X instance discovery, backup, locking, atomic settings replacement,
  and readback verification
- Evidence archive/container production beyond the Phase 12 coordinated
  profile/evidence/summary directory
- Native-profile representation for zero-span local-constant fitted models
- Completed binary-license disposition and corresponding-source bundle for the
  exact dependencies redistributed by a release
- Native installers, runtime dependency deployment, platform signing,
  notarization, and clean-host installation evidence

## Not established

- Supported SDR device matrix
- Passing macOS/Linux/Windows CI evidence beyond the locally validated macOS
  arm64 configuration
- Raspberry Pi models and Raspberry Pi OS releases that have passed qualification
- Calibration accuracy claims
- Release schedule or stable artifact compatibility guarantee

Source scaffolding, successful CMake configuration, and schema syntax checks do
not establish any calibration or hardware capability.
