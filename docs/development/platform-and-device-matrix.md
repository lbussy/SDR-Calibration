# Platform and device qualification matrix

This matrix is the authoritative index of positive platform and device claims.
`Build-covered` means only that the named source configuration built and passed
hardware-free tests. `Capture-qualified` is narrower than end-to-end calibration
qualification. A blank or planned CI job never creates evidence.

## Platform matrix

| Platform | Architecture | Build state | Packaging state | Signing state | Evidence |
| --- | --- | --- | --- | --- | --- |
| macOS 26.5.2 | arm64 | Locally build-covered | Portable archive content audited locally; clean-host install not qualified | Not qualified | Phase 14 validation at the committed revision |
| macOS 15 CI | arm64 | Build-covered | Portable archive content audited in CI; clean-host install not qualified | Not qualified | [Phase 14 CI run 31895283367](https://github.com/lbussy/SDR-Calibration/actions/runs/31895283367) |
| Ubuntu 24.04 | x86_64 | Build-covered, including strict Qt/SoapySDR release | Portable archive content audited in CI; clean-host install not qualified | Not applicable to the current portable archive | [Phase 14 CI run 31895283367](https://github.com/lbussy/SDR-Calibration/actions/runs/31895283367) |
| Windows Server 2025 CI | x86_64 | Build-covered | Portable archive content audited in CI; clean-host install not qualified | Not qualified | [Phase 14 CI run 31895283367](https://github.com/lbussy/SDR-Calibration/actions/runs/31895283367) |
| Raspberry Pi OS | arm64 | Not qualified | Not qualified | Not qualified | No retained passing run |

The minimum source-build baselines are CMake 3.20, a C++20 compiler, Qt 6.2
Core/Widgets/Concurrent for the GUI, and SoapySDR 0.8 for device capture. They
are accepted build inputs, not claims that every patch release, compiler, OS,
driver, or package manager combination is supported.

The current Phase 14.1 package has a compile-time deployment target of macOS
14.0: the project code requires at least 13.3 for Apple standard-library
floating-point `std::to_chars`, while the configured Qt 6.11.1 frameworks were
built for 14.0. This exact-input binary baseline is not clean-host or general
macOS qualification.

## Device matrix

| Device and driver | Platform | Capture | End-to-end calibration | Evidence |
| --- | --- | --- | --- | --- |
| Airspy HF+ Discovery / SoapyAirspyHF | Exact macOS configuration in the record | Capture-qualified | Not qualified | [Qualification Gate 1](qualification-gate-1-airspyhf.md) |
| All other combinations | Any | Not qualified | Not qualified | None |

## Required evidence for a positive cell

A positive cell must retain the source revision; OS and architecture; compiler,
Qt, SoapySDR, and device-module versions; configuration; package hash; exact
test list and result; clean-install or hardware setup; cleanup result; and a
stable evidence location. Device claims must additionally retain exact hardware
identity and bounded test conditions. Evidence does not transfer between rows.
