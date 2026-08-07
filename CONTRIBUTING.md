# Contributing

Thank you for contributing to SDR Calibration.

## Before changing code

Read:

- [Application architecture](docs/development/application-architecture.md)
- [Repository layout](docs/development/repository-layout.md)
- [Testing](docs/development/testing.md)
- [Licensing and distribution](docs/development/licensing-and-distribution.md)
- [Decision log](docs/development/decisions/index.md)

Keep changes focused and preserve a clear distinction between implemented
behavior and future plans.

## Building

The project uses C++20 and CMake presets:

```shell
cmake --preset default
cmake --build --preset default
ctest --preset default
```

The scaffold currently contains no production or test targets. Commands that
complete with no work or no tests do not constitute application validation.

## Documentation

Documentation uses Sphinx, MyST Markdown, and the Read the Docs theme:

```shell
python3 -m venv docs/.venv
. docs/.venv/bin/activate
python3 -m pip install -r docs/requirements.txt
sphinx-build -W -b html docs docs/_build/html
```

Dependency installation is an explicit developer action. Generated HTML under
`docs/_build/` is not committed.

## Dependencies

Do not add a dependency solely for convenience. Document why it is needed,
which component owns it, supported versions, license, transitive dependencies,
and packaging consequences.

Qt additions require special attention because not every Qt module has the same
open-source licensing options. SoapySDR vendor modules also retain independent
licenses and deployment requirements.

## Hardware-dependent work

Unit tests must not require SDR hardware, vendor drivers, network access, or a
display server. Real-device tests belong in the opt-in integration suite and
must state the device, configuration, duration, expected evidence, abort path,
and cleanup.

## Licensing

Original contributions are accepted under the project's MIT License. Do not
copy material whose license is incompatible with the project or its intended
distribution.
