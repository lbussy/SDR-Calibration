# Platform and device qualification matrix

This matrix is the authoritative index of positive platform and device claims.
`Build-covered` means only that the named source configuration built and passed
hardware-free tests. `Capture-qualified` is narrower than end-to-end calibration
qualification. A blank or planned CI job never creates evidence.

## Platform matrix

| Platform | Architecture | Build state | Packaging state | Signing state | Evidence |
| --- | --- | --- | --- | --- | --- |
| macOS 26.5.2 Phase 14.4 DMG | arm64 | Locally build-covered at `3623154` with Apple clang 21.0.0, CMake 4.4.2, and Qt 6.11.1 | Exact DMG payload/runtime and Qt license disposition passed; mounted application launch smoke passed; SHA-256 `7fbcff9f6bbd52ebc02c79227c9fac56f132034783882289b7f5c340405d5a17`; clean-host install not qualified | Developer ID signed with hardened runtime; application and DMG notarized, stapled, and accepted by Gatekeeper | Local Phase 14.4 evidence under `build/macos-official-release/macos-package/evidence` |
| macOS 15 CI | arm64 | Build-covered | Portable archive content audited in CI; clean-host install not qualified | Not qualified | [Phase 14 CI run 31895283367](https://github.com/lbussy/SDR-Calibration/actions/runs/31895283367) |
| Ubuntu 24.04 | x86_64 | Build-covered, including strict Qt/SoapySDR release | Portable archive content audited in CI; clean-host install not qualified | Not applicable to the current portable archive | [Phase 14 CI run 31895283367](https://github.com/lbussy/SDR-Calibration/actions/runs/31895283367) |
| Ubuntu 24.04.4 Phase 14.3 DEB | x86_64 | Build-covered at `e16a07d` with GCC 13.3.0, CMake 3.31.6, and Qt 6.4.2 | Extracted DEB payload/runtime audited; SHA-256 `61ba677ef389b7544df9c1d59bc354f917e64f4b39ab8f752f39ac645cfc23d5`; Phase 14.4 disposition and clean-host install not qualified for this earlier artifact | Not applicable; repository/package signing is not implemented | [Phase 14.3 CI run 31899087299](https://github.com/lbussy/SDR-Calibration/actions/runs/31899087299) |
| Ubuntu 24.04.4 Phase 14.4 DEB | x86_64 | Build-covered at `71a141a` with GCC 13.3.0, CMake 3.31.6, and Qt 6.4.2 | Extracted payload/runtime and exact dependency-license disposition passed; SHA-256 `f736d213c74b985cfada038eef8c1c6928089ea5bc5eb5b9f93954eec6458021`; clean-host install not qualified | Not applicable; repository/package signing is not implemented | [Phase 14.4 CI run 31899855376](https://github.com/lbussy/SDR-Calibration/actions/runs/31899855376) |
| Windows Server 2025 CI | x86_64 | Build-covered | Portable archive content audited in CI; clean-host install not qualified | Not qualified | [Phase 14 CI run 31895283367](https://github.com/lbussy/SDR-Calibration/actions/runs/31895283367) |
| Windows Phase 14.2 package input | x86_64 | Tooling implemented; exact package run pending | Signed MSI tooling implemented; artifact and clean-host install not qualified | Tooling implemented; exact signed artifact not qualified | No retained Phase 14.2 package run |
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

The Phase 14.2 Windows gate requires a Windows x64 release build, WiX Toolset
v4, the configured Qt `windeployqt`, SignTool, a code-signing certificate, and
an HTTPS RFC 3161 timestamp service. Tooling presence is not retained evidence
for a signed MSI; only the exact manifest and artifact hash from a passing run
can establish that narrower claim.

The Phase 14.3 Ubuntu gate requires Ubuntu 24.04 x86_64, Debian package tools,
and the configured dynamic Qt runtime. It derives system-package dependencies
from the exact ELF payload and retains an extracted-package audit. Tooling
presence is not retained evidence for a DEB; only the exact manifest and
artifact hash from a passing run can establish that narrower claim.

Phase 14.4 tooling is implemented. The exact macOS and Ubuntu rows above retain
artifact-specific disposition evidence. A Windows row still requires a new
signed MSI run with exact runtime and Qt source hashes before it can be
relabeled. Evidence does not transfer between artifacts or architectures.

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
