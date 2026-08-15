# Source layout

All project-owned C++ implementation and public headers live under `src/`.

- `core/` — calibration math, estimators, fitting, and uncertainty
- `profile/` — profile model, validation, serialization, and evaluation
- `soapy/` — SoapySDR discovery, configuration, and sample acquisition
- `application/` — workflow coordination and application services
- `adapters/wsjtx/` — WSJT-X settings and `fmt.all` interoperability
- `cli/` — headless command-line application
- `gui/` — Qt 6 Widgets application

The calibration core remains independent of Qt, SoapySDR, operating-system
paths, and WSJT-X formats.

The current `core/` implementation includes the Phase 5 in-memory carrier
estimator, the Phase 6 observation-acceptance policy, and the Phase 7
frequency-error model, uncertainty budget, and reliability quotient. `profile/`
now implements the Phase 8 native profile domain, validation, canonical JSON,
integrity hooks, bounded evaluation, and atomic replacement. It does not own
keys, reference registries, application workflows, or external adapters.
