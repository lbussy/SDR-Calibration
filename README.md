# SDR Calibration

This repository targets a multi-platform C++ application for producing
per-device SDR frequency-calibration profiles. SDR discovery, configuration,
and sample acquisition use SoapySDR so the calibration engine is not tied to a
single SDR manufacturer or driver.

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

The native calibration profile is authoritative. External formats such as
WSJT-X settings and `fmt.all` are explicitly lossy interoperability adapters.
