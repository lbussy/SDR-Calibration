# Development

The project targets C++20, CMake, SoapySDR, and Qt 6 Widgets across Windows,
macOS, and Linux. Raspberry Pi OS is a project-qualified Linux test fixture.

The calibration core remains independent of the GUI, SDR driver boundary,
operating-system paths, and interoperability formats.

Start with the [implementation status](status.md), then consult the
[repository layout](repository-layout.md), [application architecture](application-architecture.md),
[measurement model](measurement-model.md), [uncertainty model](uncertainty-model.md),
[reference registry contract](reference-registry-contract.md),
[failure-state model](failure-state-model.md), [testing strategy](testing.md), and
[decision log](decisions/index.md).
