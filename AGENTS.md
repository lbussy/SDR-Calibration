# Project instructions

## Scope and authority

- Preserve user changes and inspect repository state before modifying files.
- Keep work within the requested slice; do not perform adjacent refactors.
- Do not initialize Git, create branches, stage, commit, push, publish releases,
  or modify external services unless explicitly requested.
- Do not access SDR hardware, start sample streams, or perform RF-dependent
  testing unless the exact device and bounded test have been authorized.
- Normal validation must remain hardware-free, network-free, and safe to repeat.

## Project contracts

- Original project source and documentation use the MIT License unless the
  project owner explicitly chooses another license.
- Third-party components retain their own licenses. Follow
  `docs/development/licensing-and-distribution.md` before adding or distributing
  a dependency.
- The native SDR Calibration Profile is authoritative. WSJT-X formats are lossy
  adapters and must never silently replace the native profile.
- `target_frequency_hz` belongs to an application record, not the semi-durable
  per-device calibration profile.
- Keep device identity, effective device configuration, calibration validity,
  uncertainty, and provenance explicit and independently reviewable.
- Permit locally generated and ad-hoc references only when their class,
  evidence, conditions, and limitations are explicit. Apply the documented
  reference-class ceiling to the resulting reliability quotient.
- Treat the reliability quotient as a versioned assurance index, not a
  probability, confidence level, or substitute for measurement uncertainty.

## Architecture

- Use C++20 and CMake.
- Keep all project-owned C++ source and public headers under `src/`.
- Keep the calibration core independent of Qt, SoapySDR, operating-system
  paths, network access, and external file formats.
- Use SoapySDR only through the SDR boundary.
- Use Qt 6 Widgets for the GUI. Dynamically link the approved LGPL-compatible
  Qt modules by default.
- Do not introduce a new Qt module or other dependency without reviewing its
  license, transitive dependencies, packaging effect, and platform support.
- Keep CLI and GUI behavior aligned through shared application services.

## Source and tests

- Follow `.editorconfig` and `.clang-format`.
- Treat new compiler and static-analysis warnings in changed code as defects.
- Add deterministic unit tests with implementation changes.
- Keep real-device tests opt-in and separate from normal unit tests.
- Preserve requested versus effective SDR settings distinctly.
- Fail closed when identity, configuration, validity, integrity, or uncertainty
  requirements cannot be established.

## Documentation

- Store user-facing and developer documentation under `docs/` as MyST Markdown.
- Keep the Sphinx toctree in `docs/index.md` current when pages are added.
- Clearly distinguish implemented behavior, planned behavior, non-goals, and
  qualification still required.
- Update the decision log when a durable architectural, licensing, artifact, or
  compatibility decision changes.

## Validation

Use the narrowest applicable checks. The baseline non-hardware validation is:

```text
cmake --preset default
cmake --build --preset default
ctest --preset default
sphinx-build -W -b html docs docs/_build/html
```

If a required tool is unavailable, report the skipped check rather than
claiming it passed. Do not install dependencies unless authorized.
