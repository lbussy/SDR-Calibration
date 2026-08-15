# Tests

- `unit/` will contain deterministic, hardware-free tests.
- `integration/` will contain explicitly selected integration tests.
- `fixtures/` will contain synthetic IQ and interoperability fixtures small
  enough to review and retain in Git.

Real-device and RF-dependent tests must be opt-in and clearly identified.

Carrier-estimator golden tests use generated in-memory complex samples with
fixed parameters and a fixed noise seed. They do not read recorded RF data or
establish observation acceptance, uncertainty, calibration accuracy, or device
qualification.

Observation-acceptance tests supply deterministic estimator results and quality,
configuration, and reference diagnostics. They verify policy boundaries and
fail-closed rejection behavior without deriving metrics from RF samples or
establishing calibration accuracy, reference authority, or qualification.
