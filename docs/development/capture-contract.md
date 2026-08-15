# Complex-IQ Capture Contract

Status: Phase 4 diagnostic CLI implemented and hardware-free validated; one
exact Airspy HF+ Discovery/macOS capture combination passed Qualification Gate 1

This contract defines the smallest reusable receive-only capture facility needed
by SDR Calibration. It does not establish a general supported-device matrix or
calibration accuracy.

## 1. Scope

The capture facility acquires a bounded, single-channel complex-IQ stream from a
SoapySDR receive device and writes a raw sample artifact with a versioned metadata
manifest. The capture implementation is separate from the command-line parser so
another application can call or adapt it without inheriting SDR Calibration
workflow or presentation code.

The capture artifact is diagnostic evidence. It is not an SDR Calibration
Profile and cannot establish calibration accuracy or device qualification by
itself.

The MVP does not perform DSP, FFT analysis, visualization, resampling, frequency
translation, compression, timed or external triggering, multi-channel or
multi-device acquisition, network control, transmission, calibration-profile
generation, or vendor-API integration. It does not adopt a general recording
container or SigMF; a later adapter may map this contract to another format.

## 2. Capture request

A capture request contains:

| Field | Requirement |
| --- | --- |
| `device_arguments` | Complete SoapySDR construction filter or argument map. |
| `enumeration_index` | Optional interactive convenience only; never durable identity. |
| `rx_channel` | Non-negative channel number; MVP default is `0`. |
| `center_frequency_hz` | Required finite positive requested RF center frequency. |
| `sample_rate_sps` | Required finite positive requested complex sample rate. |
| `bandwidth_hz` | Optional finite positive requested hardware or driver bandwidth. |
| `gain_db` | Optional finite aggregate manual RX gain request. |
| `duration_seconds` | Finite positive bound, mutually exclusive with `sample_count`. |
| `sample_count` | Positive integer bound, mutually exclusive with `duration_seconds`. |
| `output_path` | Destination basename from which both artifact names are derived. |
| `read_timeout_ms` | Required positive and bounded stream-read timeout. |
| `setting_policy` | `strict` or `permissive` effective-setting policy. |
| `purpose` | Optional non-semantic operator note; not calibration evidence. |

Exactly one capture bound is required. The request must be rejected before
device construction when values are missing, non-finite, non-positive, mutually
inconsistent, cannot be represented safely, or would exceed an explicit
application byte limit.

When duration is supplied, the recorder computes the target only after reading
the effective sample rate:

```text
target_samples = ceil(duration_seconds * effective_sample_rate_sps)
```

The multiplication and conversion must detect overflow. Rounding upward ensures
the requested elapsed sample span is not shortened. The manifest records the
original duration, effective sample rate, rounding rule, and resulting target.
Sample-count requests remain exact and are not rescaled.

## 3. Device selection and identity

Selection must resolve to exactly one device. Zero matches and ambiguous matches
fail before configuration or stream activation. An enumeration index may choose
from results displayed within the same invocation, but its resolved complete
device arguments are preserved and the index is not treated as stable identity.

The capture manifest records, when exposed by SoapySDR:

- complete requested and resolved construction arguments;
- driver key and hardware key;
- serial and relevant hardware information;
- driver, library, firmware, and API versions;
- RX channel, antenna, clock source, tuner path, and frequency-correction state.

Unavailable information is represented explicitly as `null` or an availability
state, not invented or inferred from a display label. The generic contract does
not contain RSP1B- or SDRplay-specific selection rules.

## 4. Configuration and readback

The adapter applies sample rate, optional bandwidth, center frequency, and
optional manual aggregate gain before stream activation. It then reads back all
available effective values, including gain mode, gain, antenna, clock source,
and frequency correction when exposed.

Requested and effective settings are distinct records. A setting has one of
these application states:

- `applied_verified` — a readback is available and acceptable;
- `applied_changed` — a readback is available but differs from the request;
- `applied_unverified` — the driver accepted the request but offers no usable
  readback;
- `unsupported` — the capability is unavailable;
- `failed` — applying or reading the setting failed.

`strict` policy rejects `applied_changed`, `applied_unverified`, `unsupported`,
or `failed` for every explicitly requested binding setting. `permissive` policy
may continue after a changed or unverifiable setting only when acquisition is
still safe; the manifest and completion summary must make the condition
conspicuous. A failed required setting never becomes a successful capture.

MVP gain control is aggregate manual RX gain. Named gain elements, automatic
gain policies, and vendor-specific SDRplay controls are deferred. The manifest
still records the effective gain mode and any effective aggregate or named gains
that the driver exposes.

## 5. Frequency coverage

The recorder stores samples relative to the configured RF center; it does not
interpret analysis frequencies. Two locations 137.5 kHz and 741 kHz apart from
or within a passband have different tuning implications:

- if both numbers are positive baseband offsets, retaining +741 kHz requires a
  sample rate greater than 1.482 MS/s; approximately 2 MS/s supplies a nominal
  -1 MHz to +1 MHz Nyquist interval;
- if they are absolute RF frequencies, their separation is 603.5 kHz and the
  selected center and device tuning range determine their baseband positions.

Nominal Nyquist coverage is not proof of usable coverage. The requested and
effective hardware bandwidth, device filters, transition bands, DC region, and
other passband behavior remain part of later diagnostic interpretation.

## 6. Raw sample artifact

The MVP raw artifact is `<output>.cf32` with this fixed version 1 layout:

- one RX channel;
- chronological complex samples;
- interleaved I then Q components;
- IEEE 754 binary32 components;
- little-endian byte order;
- eight bytes per complex sample;
- no embedded header.

CF32 matches the normalized SoapySDR stream format, avoids introducing an
unrecorded integer scaling convention, and simplifies hardware-free fixtures.
CS16 and device-native formats are deferred. At 2 MS/s, CF32 produces about
16 MB/s or 960 MB per minute, so every capture requires sample, time, and byte
bounds.

The recorder requests no more samples than remain. A final stream read may be
short; valid returned samples are written and reading continues until the exact
target is reached or a terminal outcome occurs. Data beyond the target is never
written.

## 7. Capture manifest

The companion `<output>.capture.json` is a versioned capture manifest. Version 1
contains these top-level sections:

| Section | Required contents |
| --- | --- |
| `format` | Contract name and version. |
| `artifact` | Raw filename, `cf32_le`, I/Q layout, channel count, bytes per sample, byte count, and atomic-write state. |
| `request` | Original device, tuning, gain, bound, timeout, policy, output, and optional purpose fields. |
| `device` | Resolved arguments, identity evidence, driver, hardware, version, antenna, clock, tuner, and availability states. |
| `effective` | Effective center, rate, bandwidth, gain mode/gain, frequency correction, and per-setting application states. |
| `stream` | Soapy format, MTU, target/written samples, read calls, short reads, timeouts, overflows, discontinuities, and timestamp availability. |
| `timing` | UTC start/end when available and monotonic elapsed duration. |
| `outcome` | Status, terminal error category/message, cancellation, cleanup results, and final device-state knowledge. |

Required numeric values are numbers, not formatted strings. Optional data that
the driver does not expose is `null` with an associated availability state when
absence affects interpretation. The manifest excludes credentials, tokens,
private keys, and unnecessary personal or device data.

The manifest is metadata for one acquisition. It is not the native calibration
profile schema and is not embedded into that schema. A later evidence bundle may
refer to the capture and its digest.

## 8. Outcomes and artifact finalization

The result status is one of:

- `complete` — the exact target sample count was written and all required
  finalization succeeded;
- `partial` — at least one valid sample was retained, but the target or required
  finalization was not completed;
- `cancelled` — cancellation occurred, with zero or more retained samples;
- `failed` — no usable sample artifact was produced.

Raw data is first written to `<output>.cf32.part`. A complete capture flushes and
closes it before atomic replacement of `<output>.cf32`. The manifest is written
last through `<output>.capture.json.part` and atomically replaced. Existing
complete artifacts are never overwritten by a failed or partial attempt.

If a failed or cancelled attempt has useful samples, it may be retained only
under a collision-free incomplete name with a finalized manifest that states
`partial` or `cancelled`, the exact valid sample and byte counts, and the failure
reason. An orphan `.part` file is never reported as a capture artifact. A disk
failure that prevents a truthful manifest leaves no published partial artifact.

Terminal error categories include validation, selection, configuration,
readback, stream setup, activation, timeout limit, overflow or discontinuity
policy, device removal, driver exception, cancellation, raw write, manifest
write, deactivation, stream close, and device release. Cleanup is attempted in
reverse acquisition order on every exit. The result separately reports each
cleanup operation and whether final device state is `known_safe`, `known_open`,
or `unknown`.

Timeouts are bounded events, not infinite retries. The hardware-free foundation
defaults to a 100 ms read timeout and terminates after 10 consecutive timeouts;
callers may select a positive timeout up to 5 seconds. A successful sample read
resets the consecutive-timeout count. Overflows and discontinuities are recorded
and prevent `complete` status unless a later reviewed policy defines a safe
exception.

## 9. Reusable component boundary

The planned dependency direction is:

```text
SDR Calibration CLI -> capture application service
                         |-- capture recorder -> sample source interface
                         |-- raw-IQ writer
                         `-- capture-manifest writer
SoapySDR RX adapter ----------------------^
Calibration core (no capture dependency)
```

Reusable production-quality components are:

- capture request, effective-setting, result, and error types;
- request validation and duration-to-sample conversion;
- sample-source interface and bounded recorder;
- raw-IQ and manifest writers;
- artifact finalization and partial-outcome handling;
- SoapySDR RX selection, configuration, readback, streaming, and cleanup.

SDR Calibration-specific diagnostic components are the CLI name and help,
calibration-oriented defaults, operator wording, and later evidence-bundle
integration. The CLI contains parsing and presentation only; it does not own the
capture loop or SoapySDR lifetime.

## 10. Diagnostic CLI

Phase 4 provides the `sdrcal-capture` executable when both
`SDRCAL_BUILD_CLI` and `SDRCAL_ENABLE_SOAPYSDR` are enabled. Its required
arguments are one or more explicit `--device key=value` selectors,
`--frequency-hz`, `--sample-rate`, exactly one of `--duration` or `--samples`,
and `--output`. Optional arguments select an invocation-local enumeration index,
RX channel, hardware bandwidth, aggregate manual gain, read timeout,
strict/permissive effective-setting policy, and a non-semantic purpose note.
`--help` and `--version` are device-free operations.

Parsing is exact: unknown and positional arguments, repeated scalar options,
duplicate device keys, malformed selectors, non-finite numbers, and missing or
excessive bounds fail before SoapySDR enumeration or device construction. The
CLI labels requested and effective settings separately and reports the capture
outcome, artifact paths, and final device-state knowledge. Existing artifacts
are not overwritten. SIGINT requests the recorder's ordinary bounded
cancellation and cleanup path.

Process exit statuses are `0` for a complete capture or informational action,
`2` for command-line usage failure, `3` for device selection/preparation
failure, and `4` for a non-complete capture. These diagnostic categories are
implemented behavior, not the stable automation contract planned for the Phase
12 production CLI.

## 11. Hardware-free validation

Normal tests remain hardware-free and cover:

- missing, invalid, conflicting, excessive, and overflowing request values;
- duration conversion and its upward rounding;
- exact reads, repeated short reads, and a partial final read;
- bounded timeout and cancellation behavior;
- overflow, discontinuity, device, and generic stream errors;
- requested/effective matches, changes, unsupported values, and unavailable
  readbacks under both setting policies;
- raw and manifest write failures;
- complete, partial, cancelled, and failed artifact finalization;
- cleanup after failure at each acquisition stage;
- manifest, sample-count, and byte-count consistency.

The reusable capture recorder is tested through a fake sample source. The
SoapySDR session is tested through injected fake enumeration, device,
configuration, stream, failure, and cleanup operations. Native SoapySDR stream
codes and flags are translated by a pure hardware-free test surface. Ordinary
tests never instantiate the production API wrapper and therefore cannot
enumerate or construct a device. Installed-module and real-device tests are
opt-in integration tests. Each
real-device test requires separate authorization of exact device identity,
input arrangement, requested configuration, duration, expected evidence, abort,
and cleanup. Contract tests and mock tests do not establish RSP1B support.

## 12. Phase 2 implementation decisions

The hardware-free implementation uses these bounded policies:

- maximum duration: 120 seconds;
- maximum sample count: 268,435,456 complex samples;
- maximum raw output: 2,147,483,648 bytes (2 GiB at eight bytes per sample);
- default read timeout: 100 ms, maximum: 5 seconds, with a default terminal
  budget of 10 consecutive timeouts;
- setting comparison: the greater of 1 Hz or 1 ppm for center frequency, sample
  rate, and bandwidth, and 0.1 dB absolute for gain;
- incomplete names: `<output>.incomplete-<attempt-id>`, where production IDs
  combine a monotonic-clock value with a process-local atomic sequence and only
  ASCII letters, digits, hyphen, and underscore are accepted from callers;
- JSON: a project-owned deterministic serializer for the closed manifest model,
  with explicit escaping, classic-locale finite numbers, and stable field order;
- cancellation: a synchronous cooperative callback checked between bounded
  reads, requiring no recorder-owned thread;
- cleanup: separate attempted/succeeded/detail results for the sample source,
  raw artifact, and manifest, plus `known_safe`, `known_open`, or `unknown` final
  state.

Callers may impose stricter resource limits. Explicit sample counts and planned
byte counts must satisfy every applicable limit.

## 12. Phase 3 SoapySDR boundary

When `SDRCAL_ENABLE_SOAPYSDR=ON`, the build locates the installed SoapySDR CMake
package and builds a distinct `sdrcal_soapy` library over `sdrcal_capture`.
When disabled, neither SoapySDR headers nor libraries are required and the
generic capture tests remain available.

The adapter implements unambiguous selection, resolved construction arguments,
receive configuration and readback, CF32 stream setup, activation, bounded read
translation, and reverse-order cleanup. Its injected facade keeps production
SoapySDR types out of the generic capture interface. Explicitly requested
unsupported settings remain fatal under both setting policies; permissive mode
is limited to changed or unverifiable settings. Manual gain is not verified when
the driver exposes gain mode but cannot confirm that automatic gain is off.
The final sample and byte plan is computed from effective readback before stream
setup or activation, so an excessive duration-derived plan cannot start a stream.

The adapter and real wrapper have been compiled and linked, but production
enumeration, construction, configuration, and streaming paths have not been
executed. No CLI or real-device integration is implemented. RSP1B support and
hardware behavior remain unestablished and require separate authorization.
