# Development

The project targets C++20 and CMake on macOS 14.0 or later on Apple Silicon,
Windows 11 x64, and Raspberry Pi OS 13 ARM64 on Raspberry Pi 4. The two desktop
targets provide the Qt 6 Widgets GUI and CLI; Raspberry Pi provides the
recorded-input CLI only and excludes SoapySDR and live-device operation. Ubuntu
source and packaging remain available for portability work but are outside the
supported target list and currently unvalidated.

The calibration core remains independent of the GUI, SDR driver boundary,
operating-system paths, and interoperability formats.

Start with the [implementation status](status.md) and
[development roadmap](roadmap.md), then consult the
[repository layout](repository-layout.md), [application architecture](application-architecture.md),
[complex-IQ capture contract](capture-contract.md),
[Qualification Gate 1 Airspy HF+ Discovery record](qualification-gate-1-airspyhf.md),
[platform and device matrix](platform-and-device-matrix.md),
[end-to-end calibration qualification gate](end-to-end-qualification.md),
[carrier estimator](carrier-estimator.md),
[observation acceptance](observation-acceptance.md),
[measurement model](measurement-model.md), [uncertainty model](uncertainty-model.md),
[reference registry contract](reference-registry-contract.md),
[failure-state model](failure-state-model.md), [testing strategy](testing.md), and
[decision log](decisions/index.md).

Phase execution records include the
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
