# Development

The project targets C++20 and CMake on macOS 14.0 or later on Apple Silicon,
Windows 11 x64, and Raspberry Pi OS 13 ARM64 on Raspberry Pi 4. The two desktop
targets provide the Qt 6 Widgets GUI and CLI; Raspberry Pi provides the
recorded-input CLI only and excludes SoapySDR and live-device operation. Ubuntu
source and packaging remain available for portability work but are outside the
supported target list and currently unvalidated.

The calibration core remains independent of the GUI, SDR driver boundary,
operating-system paths, and interoperability formats.

The planned production live-device path is governed by the
[live-device calibration contract](live-device-calibration-contract.md). Its
initial contract-freezing work is retained in the
[Phase 1 execution prompt](live-device-integration-phase-1-execution-prompt.md).
The first implementation slice is retained in the
[bounded in-memory acquisition prompt](live-memory-acquisition-execution-prompt.md).
The analyzer slice is retained in the
[signal-quality execution prompt](signal-quality-analyzer-execution-prompt.md).
The production live-CLI slice is governed by the
[live-CLI execution prompt](live-cli-execution-prompt.md).
The production live-GUI slice is governed by the
[live-GUI execution prompt](live-gui-execution-prompt.md).
The hardware-free cross-component gate is governed by the
[synthetic end-to-end workflow qualification prompt](synthetic-end-to-end-workflow-qualification-prompt.md).
Future local-reference work is held by the
[GPSDO/reference-path readiness package](gpsdo-reference-path-readiness.md),
prepared under its [execution prompt](gpsdo-reference-path-readiness-execution-prompt.md).
The current macOS exact-host package lifecycle is governed by the
[same-host qualification prompt](macos-same-host-package-qualification-prompt.md).
The current release-candidate state is recorded in the
[readiness audit](release-candidate-readiness.md), prepared under its
[execution prompt](release-candidate-readiness-audit-prompt.md).
The intended initial compatibility promise and release artifact set are fixed
by [decision 0022](decisions/0022-initial-compatibility-and-release-artifacts.md),
prepared under the
[compatibility and artifact-scope prompt](initial-compatibility-and-artifact-scope-execution-prompt.md).
Release notes, limitations, lifecycle, security, checksum, and evidence-ledger
working files are collected in the
[release template pack](release/index.md), prepared under its
[execution prompt](release-documentation-template-pack-execution-prompt.md).
The package layout, manifest, checksums, evidence-cell matrix, privacy boundary,
and final assembly checks are defined by the
[release evidence package plan](release/evidence-package-assembly-plan.md),
prepared under its
[execution prompt](release-evidence-package-preparation-execution-prompt.md).
The exact host-local `0.1.0` upgrade inputs are governed by the
[prior-version baseline retention prompt](prior-version-baseline-retention-execution-prompt.md)
and recorded in the
[baseline manifest](https://github.com/lbussy/SDR-Calibration/blob/main/evidence/release-baselines/0.1.0/README.md).
The prepared platform procedures and acceptance criteria are defined by the
[upgrade and rollback qualification plan](upgrade-rollback-qualification-plan.md),
created under its
[preparation execution prompt](upgrade-rollback-qualification-preparation-execution-prompt.md).
Production executable and package version identity is centralized under the
[version identity execution prompt](version-identity-centralization-execution-prompt.md).
The private security-reporting channel and supported-version policy are governed
by the
[private vulnerability reporting prompt](private-vulnerability-reporting-execution-prompt.md).
The first exact local end-to-end candidate is governed by the
[Airspy HF+ qualification prompt](airspyhf-end-to-end-qualification-prompt.md),
with its current [hardware-free readiness result](airspyhf-end-to-end-readiness.md).
The readiness blocker is addressed by the hardware-free
[AirspyHF normalization prompt](airspyhf-normalization-execution-prompt.md).
The corresponding hardware-free SDRplay RSP1B identity review is retained in
the [RSP1B normalization prompt](sdrplay-rsp1b-normalization-execution-prompt.md).
The retained 10 MHz diagnostic is evaluated under the hardware-free
[AirspyHF offline-analysis prompt](airspyhf-offline-analysis-prompt.md).

Start with the [implementation status](status.md) and
[development roadmap](roadmap.md), then consult the
[repository layout](repository-layout.md), [application architecture](application-architecture.md),
[complex-IQ capture contract](capture-contract.md),
[live-device calibration contract](live-device-calibration-contract.md),
[Qualification Gate 1 Airspy HF+ Discovery record](qualification-gate-1-airspyhf.md),
[Qualification Gate 1 SDRplay RSP1B record](qualification-gate-1-sdrplay-rsp1b.md),
[platform and device matrix](platform-and-device-matrix.md),
[end-to-end calibration qualification gate](end-to-end-qualification.md),
[carrier estimator](carrier-estimator.md),
[signal-quality analyzer](signal-quality-analyzer.md),
[observation acceptance](observation-acceptance.md),
[measurement model](measurement-model.md), [uncertainty model](uncertainty-model.md),
[reference registry contract](reference-registry-contract.md),
[failure-state model](failure-state-model.md), [testing strategy](testing.md), and
[decision log](decisions/index.md).

Phase execution records include the
[live-device integration Phase 1 prompt](live-device-integration-phase-1-execution-prompt.md),
the
[Phase 12 production CLI prompt](phase-12-execution-prompt.md) and
[Phase 13 Qt desktop prompt](phase-13-execution-prompt.md).
The current packaging and qualification foundation is governed by the
[Phase 14 execution prompt](phase-14-execution-prompt.md).
The signed macOS package slice is governed by the
[Phase 14.1 execution prompt](phase-14-1-execution-prompt.md).
The signed Windows package slice is governed by the
[Phase 14.2 execution prompt](phase-14-2-execution-prompt.md). Preparation and
bounded execution on the named Windows qualification host are governed by the
[`ecm-mule` candidate prompt](windows-ecm-mule-candidate-execution-prompt.md).
The separate Microsoft Store MSIX path is described in the
[Store MSIX guide](windows-store-msix.md), governed by
[decision 0024](decisions/0024-windows-store-msix-distribution.md) and its
[execution prompt](windows-store-msix-execution-prompt.md).
The Ubuntu DEB package slice is governed by the
[Phase 14.3 execution prompt](phase-14-3-execution-prompt.md), and exact payload
license disposition is governed by the
[Phase 14.4 execution prompt](phase-14-4-execution-prompt.md). The cross-platform
GUI product name, icon, and clean-host candidate gate is governed by the
[Phase 14.5 execution prompt](phase-14-5-execution-prompt.md). Its bounded
pre-qualification implementation slice is governed by the
[Phase 14.5 branding prompt](phase-14-5-branding-execution-prompt.md).
The hardware-free Raspberry Pi recorded-input CLI implementation is governed by
the [Raspberry Pi CLI Phase 1 prompt](raspberry-pi-cli-phase-1-execution-prompt.md).
The bounded package and installation procedures are recorded in the
[wspr4 ARM64 prompt](wspr4-raspbian-package-install-execution-prompt.md) and the
[wspr2 exploratory ARMHF prompt](wspr2-raspbian-package-install-execution-prompt.md).
The approved initial operating-system and architecture scope is governed by the
[platform target boundary prompt](platform-target-boundary-execution-prompt.md)
and [decision 0019](decisions/0019-initial-platform-target-boundary.md).
The removal of Ubuntu from the supported target list is governed by
[decision 0020](decisions/0020-remove-ubuntu-target.md).
