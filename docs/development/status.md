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
- Hardware-free carrier estimator for in-memory complex samples, including
  midpoint frequency, linear drift, explicit failure states, diagnostics, and
  deterministic synthetic golden fixtures

## Planned, not implemented

- Observation acceptance and versioned reliability-quotient component scoring
- Uncertainty calculation and model fitting
- Profile serialization, validation, signing, and evaluation
- Reference-station manifest and registry updater
- Real-device SoapySDR discovery, configuration, and sample acquisition
- Command-line application
- Qt 6 Widgets application
- WSJT-X settings and `fmt.all` adapters
- Evidence bundle generation
- Packaging and binary-license compliance automation

## Not established

- Supported SDR device matrix
- Minimum supported Qt and SoapySDR versions
- Raspberry Pi models and Raspberry Pi OS releases that have passed qualification
- Calibration accuracy claims
- Release schedule or stable artifact compatibility guarantee

Source scaffolding, successful CMake configuration, and schema syntax checks do
not establish any calibration or hardware capability.
