# Production command-line application

The `sdrcal` command supports two explicit request schemas over the same shared
calibration workflow. Recorded mode consumes previously captured CF32LE
observations. Live mode uses the receive-only SoapySDR boundary for strict
readback and bounded in-memory observations. Neither mode falls back to the
other. Implementation does not qualify a device or establish calibration
accuracy.

The CLI is included in the initial macOS 14.0 or later Apple Silicon and Windows
11 x64 desktop targets. Live mode is available only in builds configured with
SoapySDR. Raspberry Pi OS 13 ARM64 on Raspberry Pi 4 is a recorded-input
CLI-only target; SoapySDR and live-device operation are outside that Raspberry
Pi scope. Ubuntu CLI implementation remains available but is unsupported and
currently unvalidated.

## Invocation

```shell
sdrcal calibrate \
  --request recorded-calibration.json \
  --trust-file registry-trust.json \
  --output-dir calibration-result
```

`--help` and `--version` perform no input or output access. The output directory
must not already exist. Input paths in the request are relative to the request
file and may not be absolute or contain `..`.

Progress and operator diagnostics go to standard error. A calibration attempt
emits one canonical JSON terminal result to standard output. Stable exit values
are 0 for success, 2 for usage, 3 for request/input/authentication, 4 for a
scientific workflow rejection, 5 for output publication, and 130 for
cancellation.

## Recorded run-request contract

The JSON root uses `schema_name` `sdrcal-recorded-calibration-request` and
`schema_version` `1.0.0`. Unknown members fail closed. It requires run/profile
identity, one device identity, separate requested/effective configuration,
temperature validity, a complete signed reference registry, at least two
independent observations, uncertainty and assurance inputs, and bounded
evidence policy. Each observation retains requested and effective indicated
center frequency, a relative CF32LE path, SHA-256, byte limit, duration,
quality metrics, and its reference-condition result.

The trust file is a JSON object mapping a registry signature key ID to the exact
pinned signature value. This is a bounded local pin mechanism, not a production
Ed25519 trust store or key-management system. Keeping it outside the run request
prevents a request from declaring the trust anchor used to authenticate itself.

Every observation file must be non-empty, no larger than both its declared
limit and 2 GiB, aligned to complete little-endian complex float32 pairs, free
of non-finite samples, and identical to its declared SHA-256 digest.

The source default permits a declared bound through 2 GiB. A distribution may
configure a smaller fail-closed ceiling to match a measured resource envelope;
the request is rejected before sample input or staging output when any declared
`maximum_bytes` exceeds that build's ceiling. The Raspberry Pi CLI configuration
uses 128 MiB, selected from the first native resource measurement and subject to
the exact fixture's retained qualification. It is not a general Raspberry Pi
support or maximum-artifact claim.

## Live run-request contract

Live mode uses `schema_name` `sdrcal-live-calibration-request` and
`schema_version` `1.0.0`. It preserves the common identity, configuration,
registry, uncertainty, assurance, estimator, acceptance, evidence, and output
fields. Its `observations` contain only observation ID, independence ID,
reference ID, and requested indicated center frequency. Supplying effective
readback or quality metrics is rejected; the application derives them.

`live_acquisition` requires exact string-valued `device_arguments`, a
non-negative `rx_channel`, one positive `sample_count` or `duration_seconds`, a
positive `read_timeout_ms`, and a positive `maximum_memory_bytes`. Optional
`gain_db` is applied under strict requested/effective policy. Enumeration index
is not part of this schema. Expected identity and configuration, including the
Soapy argument and effective gain/gain-mode binding extension, must match the
actual readback or the run fails before model fitting.

Reference conditions are not supplied by a live observation. When the
authenticated registry's condition list is exactly `["none"]`, the application
records that signed registry fact as condition evidence. Any other condition
remains unmet in this slice. A request cannot assert that it was satisfied.

Invoking live mode can enumerate, configure, and receive from the explicitly
selected SDR. It should be done only under a separately reviewed device,
reference, settings, duration, abort, cleanup, and evidence plan. Ordinary unit
and integration tests use injected fake APIs and never access hardware.

## Output transaction

Success atomically publishes one new directory containing `profile.json`,
`evidence.json`, `summary.json`, and, when requested and representable, an
explicitly lossy `wsjtx.ini`. The profile binds the finalized evidence digest.
Files are synchronized in staging before publication. Failure or cancellation
removes staging output and never replaces an existing destination. The profile
is unsigned until production signing and key management are implemented.
