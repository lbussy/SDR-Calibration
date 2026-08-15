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

Model and uncertainty golden tests verify indicated-error sign, fitted linear
coefficients, inclusive validity boundaries, extrapolation rejection,
uncertainty correlation handling, and reliability-quotient ceilings. They use
synthetic values and do not establish the truth of acceptance, independence,
uncertainty, or assurance inputs supplied by a future application workflow.

Native-profile tests verify schema-facing and cross-field validation, RFC 8785
canonicalization fixtures, SHA-256 vectors, injected signing and verification
hooks, fail-closed evaluation, revocation, and last-known-good preservation.
They do not validate a production key store, network registry, physical device,
reference authority, calibration accuracy, or end-to-end workflow.
