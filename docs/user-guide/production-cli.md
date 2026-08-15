# Production command-line application

The Phase 12 `sdrcal` command calibrates from explicitly selected, previously
captured CF32LE observations. It runs the same shared calibration workflow that
the planned desktop interface will use. This path does not discover or access
an SDR, and completing it does not qualify a device or establish calibration
accuracy.

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

## Run-request contract

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
`maximum_bytes` exceeds that build's ceiling. The provisional Raspberry Pi CLI
configuration uses 256 MiB. That value is an engineering bound pending native
resource measurement, not a Raspberry Pi support or maximum-artifact claim.

## Output transaction

Success atomically publishes one new directory containing `profile.json`,
`evidence.json`, `summary.json`, and, when requested and representable, an
explicitly lossy `wsjtx.ini`. The profile binds the finalized evidence digest.
Files are synchronized in staging before publication. Failure or cancellation
removes staging output and never replaces an existing destination. The profile
is unsigned until production signing and key management are implemented.
