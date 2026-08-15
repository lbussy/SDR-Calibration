# Testing strategy

## Test classes

### Unit tests

Unit tests are deterministic and require no SDR, vendor driver, network,
display server, or external service. They cover calibration math, uncertainty,
profile validation, configuration matching, and parsers using reviewed fixtures.

### Integration tests

Integration tests cover boundaries such as SoapySDR, Qt, filesystem behavior,
and WSJT-X files. Tests that require installed optional components are labeled
and selected explicitly.

### Real-device tests

Real-device tests are never part of an ordinary test run. Each test states:

- exact SDR identity and driver;
- antenna or terminated-input arrangement;
- requested frequency, rate, bandwidth, gain, and clock source;
- duration and observation bound;
- expected evidence and acceptance criteria;
- abort method and cleanup.

This application is receive-oriented. Tests must not cause or imply RF
transmission.

## Baseline commands

```shell
cmake --preset default
cmake --build --preset default
ctest --preset default
```

A zero-test result is reported as `no tests`, not as evidence that behavior is
validated.

Documentation validation is:

```shell
sphinx-build -W -b html docs docs/_build/html
```

## Required fixtures

Planned fixtures include:

- synthetic complex-IQ carriers with known offset, drift, noise, and clipping;
- profile examples that pass and fail each contract gate;
- positive and negative WSJT-X sign-convention cases;
- valid, rejected, malformed, and partially understood `fmt.all` lines;
- mocked SoapySDR capability and stream failures;
- fake capture sources covering exact reads, short reads, timeouts,
  discontinuities, cancellation, and cleanup failures;
- complete and explicitly incomplete CF32 capture artifacts with manifests;
- atomic-write and interrupted-update cases.

Fixtures must be small, reviewable, redistributable, and free of credentials or
unnecessary personal and hardware identifiers.

The implemented Phase 2 `capture_unit_tests` target exercises the generic
sample-source boundary, bounded recorder, CF32LE encoding, manifest generation,
artifact publication, cancellation, timeout, stream-event, and cleanup paths.
It does not load SoapySDR or establish real-device behavior.
