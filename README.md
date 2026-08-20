# SDR Calibration

This repository targets a C++ application for producing per-device SDR
frequency-calibration profiles. Its initial desktop targets are macOS 14.0 or
later on Apple Silicon and Windows 11 x64. Raspberry Pi OS 13 ARM64 on
Raspberry Pi 4 is a recorded-input CLI-only target. Ubuntu implementation and
packaging remain in the repository as an unsupported, currently unvalidated
portability path. SDR discovery, configuration, and sample acquisition use
SoapySDR on the desktop targets; live-device operation is outside the
Raspberry Pi target.

The native calibration artifact remains language-neutral so C++, Python, and
other consuming applications can interpret the same results.

Original project source code and documentation are licensed under the MIT
License. The GUI uses dynamically linked Qt 6 modules under their applicable
licenses. Distributed builds must satisfy Qt and all other third-party license
obligations; see [Third-party notices](THIRD_PARTY_NOTICES.md).

- [Documentation](docs/index.md)
- [Calibration profile contract](docs/development/profile-contract.md)
- [Calibration profile JSON Schema](schemas/sdr-calibration-profile.schema.json)
- [Application architecture](docs/development/application-architecture.md)
- [Licensing and distribution policy](docs/development/licensing-and-distribution.md)
- [Implementation status](docs/development/status.md)
- [Platform and device matrix](docs/development/platform-and-device-matrix.md)
- [Contributing](CONTRIBUTING.md)
- [Privacy policy](PRIVACY.md)

The native calibration profile is authoritative. External formats such as
WSJT-X settings and `fmt.all` are explicitly lossy interoperability adapters.
