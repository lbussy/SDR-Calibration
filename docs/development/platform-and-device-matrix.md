# Platform and device qualification matrix

This matrix is the authoritative index of positive platform and device claims.
`Build-covered` means only that the named source configuration built and passed
hardware-free tests. `Capture-qualified` is narrower than end-to-end calibration
qualification. A blank or planned CI job never creates evidence.

The device matrix is a claim ledger, not a prerequisite that the project
owner's hardware must pass before users can calibrate their own devices. An
initial release may truthfully contain no positive end-to-end physical-device
cell while the user-directed workflow is validated under decision 0028.

## Initial target boundary

The initial desktop targets are macOS 14.0 or later on Apple Silicon and Windows
11 x64. Raspberry Pi OS 13 ARM64 on Raspberry Pi 4 is a recorded-input CLI-only
target. Ubuntu implementation remains in the repository as an unsupported,
currently unvalidated portability path. Target status does not make a matrix
cell positive; only retained evidence does.

Intel macOS, Windows ARM64, Windows 10 unless deliberately retained later,
generic Linux distributions, all Ubuntu variants, 32-bit Raspberry Pi OS,
other Raspberry Pi models, and Raspberry Pi SoapySDR/live-device operation are
outside the initial supported matrix.

## Platform matrix

| Platform | Architecture | Build state | Packaging state | Signing state | Evidence |
| --- | --- | --- | --- | --- | --- |
| macOS 26.5.2 current DMG | arm64 | Locally build-covered at `5eb3670` with Apple clang 21.0.0, CMake 4.4.2, and Qt 6.11.1; 16/16 hardware-free tests passed | Exact DMG payload/runtime and license disposition passed; same-host lifecycle retained; owner-confirmed clean-host lifecycle passed for SHA-256 `94a831d4549b92edd85222c55e0cd64395dbe8acfd1f4bd5c57351f15bf80ad4`; prior-version upgrade not qualified | Developer ID signed with hardened runtime; application and DMG notarized, stapled, and accepted by Gatekeeper | [Clean-host record](https://github.com/lbussy/SDR-Calibration/blob/main/evidence/clean-host/2026-08-17-owner-confirmed/README.md) |
| macOS 15 CI | arm64 | Build-covered | Portable archive content audited in CI; clean-host install not qualified | Not qualified | [Phase 14 CI run 31895283367](https://github.com/lbussy/SDR-Calibration/actions/runs/31895283367) |
| Ubuntu 24.04 | x86_64 | Historical build evidence only; unsupported and currently unvalidated | Historical portable archive audit; not a current package or clean-host claim | Not applicable | [Phase 14 CI run 31895283367](https://github.com/lbussy/SDR-Calibration/actions/runs/31895283367) |
| Ubuntu 24.04.4 Phase 14.3 DEB | x86_64 | Historical build evidence at `e16a07d`; unsupported and currently unvalidated | Historical extracted DEB audit; SHA-256 `61ba677ef389b7544df9c1d59bc354f917e64f4b39ab8f752f39ac645cfc23d5`; not a current package or clean-host claim | Not applicable | [Phase 14.3 CI run 31899087299](https://github.com/lbussy/SDR-Calibration/actions/runs/31899087299) |
| Ubuntu 24.04.4 Phase 14.4 DEB | x86_64 | Historical build evidence at `71a141a`; unsupported and currently unvalidated | Historical extracted payload/runtime and dependency-license audit; SHA-256 `f736d213c74b985cfada038eef8c1c6928089ea5bc5eb5b9f93954eec6458021`; not a current package or clean-host claim | Not applicable | [Phase 14.4 CI run 31899855376](https://github.com/lbussy/SDR-Calibration/actions/runs/31899855376) |
| Windows Server 2025 CI | x86_64 | Build-covered | Portable archive content audited in CI; clean-host install not qualified | Not qualified | [Phase 14 CI run 31895283367](https://github.com/lbussy/SDR-Calibration/actions/runs/31895283367) |
| Windows 11 Pro 25H2 on `ecm-mule` | x86_64 | Locally build-covered at `15ed17b`; 14/14 hardware-free tests passed | Exact self-signed MSI audit and same-host lifecycle retained; owner-confirmed clean-host lifecycle and application/icon review at relevant scaling settings passed for SHA-256 `7be98c92de35bb31024152161f9b3bb4e76cbf6a3d2728001060f3d338d81973`; prior-version upgrade not qualified | Locally trusted self-signed development certificate; public-trust signing, timestamping, and SmartScreen qualification remain open | [Clean-host record](https://github.com/lbussy/SDR-Calibration/blob/main/evidence/clean-host/2026-08-17-owner-confirmed/README.md) |
| Windows 11 Pro 25H2 Store MSIX on `ecm-mule` | x64 | Current candidate must retain its own 19-test result | Historical candidates passed construction, WACK, and same-host development lifecycle, but those results and hashes do not transfer; current-candidate WACK, lifecycle, Store delivery, update, clean-host, and prior-version upgrade remain open | Current package is an unsigned pre-submission artifact; Microsoft Store signing, certification, and public trust are not established | [Historical 0.1.1 WACK and lifecycle record](https://github.com/lbussy/SDR-Calibration/blob/main/evidence/windows-store/2026-08-17-0.1.1-wack-lifecycle/README.md) |
| Raspberry Pi OS 13 on Raspberry Pi 4 Model B Rev 1.1 | arm64 | Native CLI-only build-covered at `5f50d31` with GCC 14.2.0 and CMake 3.31.6; 12/12 hardware-free tests passed | CLI-only archive and rootless DEB payload audited; owner-confirmed clean-host lifecycle passed for DEB SHA-256 `d3de3d6397aaec7c3959e8854ac1871a787e4850706ae7736761a3fa887605ef`; prior-version upgrade not qualified | Not applicable; repository/package signing is not implemented | [Clean-host record](https://github.com/lbussy/SDR-Calibration/blob/main/evidence/clean-host/2026-08-17-owner-confirmed/README.md) |

The minimum source-build baselines are CMake 3.20, a C++20 compiler, Qt 6.2
Core/Widgets/Concurrent for the GUI, and SoapySDR 0.8 for device capture. They
are accepted build inputs, not claims that every patch release, compiler, OS,
driver, or package manager combination is supported.

The current macOS package has a compile-time deployment target of macOS
14.0: the project code requires at least 13.3 for Apple standard-library
floating-point `std::to_chars`, while the configured Qt 6.11.1 frameworks were
built for 14.0. This exact-input binary baseline is not clean-host or general
macOS qualification.

The Phase 14.2 Windows gate requires a Windows x64 release build, WiX Toolset
v4, the configured Qt `windeployqt`, SignTool, and a code-signing certificate.
The current self-signed mode requires the exact certificate to be explicitly
trusted in the current user's Trusted People store and forbids a timestamp
input. The future public-trust mode requires an HTTPS RFC 3161 timestamp
service. Tooling
presence is not retained evidence for a signed MSI; only the exact manifest and
artifact hash from a passing run can establish that narrower, mode-specific
claim.

The retained Phase 14.3 Ubuntu gate requires Ubuntu 24.04 x86_64, Debian package tools,
and the configured dynamic Qt runtime. It derives system-package dependencies
from the exact ELF payload and retains an extracted-package audit. Tooling
presence is not retained evidence for a DEB; only the exact manifest and
artifact hash from a passing run can establish that narrower historical claim.
Ubuntu is not a current supported target, and no current validation is planned.

Phase 14.4 tooling is implemented. The exact macOS, Windows, and Ubuntu rows
above retain artifact-specific disposition evidence. The Windows result remains
limited to its locally trusted self-signed mode and exact-host lifecycle;
public-trust and prior-version upgrade gates remain open. Evidence does not transfer
between artifacts or architectures.

## Device matrix

| Device and driver | Platform | Capture | End-to-end calibration | Evidence |
| --- | --- | --- | --- | --- |
| Airspy HF+ Discovery / SoapyAirspyHF | Exact macOS configuration in the record | Capture-qualified | Not qualified | [Qualification Gate 1](qualification-gate-1-airspyhf.md) |
| SDRplay RSP1B / SoapySDRPlay3 | Exact macOS configuration in the record | Capture-qualified; retained antenna sample was estimator-rejected | Not qualified | [Qualification Gate 1](qualification-gate-1-sdrplay-rsp1b.md) |
| All other combinations | Any | Not qualified | Not qualified | None |

## Required evidence for a positive cell

A positive cell must retain the source revision; OS and architecture; compiler,
Qt, SoapySDR, and device-module versions; configuration; package hash; exact
test list and result; clean-install or hardware setup; cleanup result; and a
stable evidence location. Device claims must additionally retain exact hardware
identity and bounded test conditions. Evidence does not transfer between rows.
