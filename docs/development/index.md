# Development

The project targets C++20, CMake, SoapySDR, and Qt 6 Widgets across Windows,
macOS, and Linux. Raspberry Pi OS is a project-qualified Linux test fixture.

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
[Phase 14.2 execution prompt](phase-14-2-execution-prompt.md).
The Ubuntu DEB package slice is governed by the
[Phase 14.3 execution prompt](phase-14-3-execution-prompt.md).
