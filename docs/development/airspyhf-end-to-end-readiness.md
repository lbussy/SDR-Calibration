# Airspy HF+ end-to-end qualification readiness

## Result

**Status: Exact enumeration and bounded 10 MHz transport passed with known-safe
cleanup; WWV reference suitability was not established, so the end-to-end gate
stopped before 15 MHz or production calibration.**

The hardware-free Stage A review of the
[execution prompt](airspyhf-end-to-end-qualification-prompt.md) found that the
exact retained Airspy HF+ Discovery capture does not provide all identity and
configuration readbacks required by the production live workflow. The initial
requested receive-only enumeration was rejected at the authorization boundary.
A later exact authorization permitted the bounded enumeration recorded below;
it did not authorize device construction or streaming.

## Blocking evidence

The retained Qualification Gate 1 manifest for serial
`2f52ff5de72635ba` records:

- `hardware_info` containing only the serial, without manufacturer or model;
- an empty clock-source value;
- no effective frequency-correction value.

`SoapyWorkflowBoundary` currently requires a manufacturer, model, hardware
serial, nonempty clock source, positive effective sample rate, and finite
effective frequency correction before it can construct the exact device and
configuration identity used by calibration. It then requires that identity and
configuration to match the request. Inventing defaults or weakening this check
would violate the fail-closed contract and is not part of qualification.

The production workflow also requires at least two independent accepted
observations spanning different frequencies. [NIST's WWV station
page](https://www.nist.gov/pml/time-and-frequency-division/time-distribution/radio-station-wwv)
identifies continuous 10 MHz and 15 MHz broadcasts, while [NIST SP
250-67](https://doi.org/10.6028/NIST.SP.250-67) identifies the carriers as
standard frequencies suitable for calibration within receiving-equipment and
propagation limitations. This establishes a candidate reference plan, not that
either carrier is currently receivable or suitable at this antenna.
WWV and WWVH share nominal frequencies, and WWV's 10 MHz and 15 MHz transmitters
derive from a common station clock. Station attribution, propagation effects,
and scientifically honest independence/correlation evidence therefore remain
unresolved later gates even after device readback is addressed.

## Source resolution

The subsequent hardware-free normalization slice added explicit provenance-
bearing rules rather than assumed readbacks. Exact `AirspyHF` driver/hardware
keys may supply the Airspy HF+ family identity policy; an empty current clock is
normalized only when Soapy reports no selectable sources; and effective
correction becomes zero only when Soapy explicitly reports correction as
unsupported. Unknown identities, selectable-but-unreported clocks, and
supported-but-unreadable correction still fail closed. Fake-API tests cover the
positive and negative boundaries.

## Diagnostic authorization gate

The operator separately authorized one bounded 10 MHz receive operation from
the execution prompt. Current post-construction metadata was required to match
the normalized request before streaming. That authorization did not include a
15 MHz capture or production calibration. The device matrix remains capture-
qualified only and end-to-end calibration remains not qualified.

## Receive-only enumeration record

At `2026-08-16T20:36:06Z`, source revision
`43ec9d44dda894f5228912a8e3e38479c7da4ccd` performed one serial-bound Soapy
enumeration on macOS 26.5.2 build 25F84, arm64. The installed stack reported
SoapySDR library 0.8.1-release, API 0.8.0, ABI 0.8, and SoapyAirspyHF
0.2.0-7457d69.

The exact enumeration filter was `driver=airspyhf` and
`serial=2f52ff5de72635ba`. It returned exactly one match:

```text
driver = airspyhf
label = AirSpy HF+ [2f52ff5de72635ba]
serial = 2f52ff5de72635ba
```

This passes only deterministic selection at the enumeration boundary. It does
not expose or verify the post-construction driver key, hardware key,
clock-source capabilities, frequency-correction capability, effective settings,
signal quality, cleanup, or calibration behavior. No device was constructed or
configured, no stream was created, and no samples or RF were transmitted. The
next physical step requires separate authorization for an exact bounded
diagnostic receive capture.

## Bounded 10 MHz diagnostic record

At `2026-08-16T20:38:06Z`, source revision
`fd1b183838e3aa2414aa6686d1c73bfaa158080b` performed the separately authorized
single receive-only diagnostic capture. The exact request used Airspy serial
`2f52ff5de72635ba`, RX channel 0, 10,000,000 Hz center, 192,000 complex
samples/s, no requested bandwidth or gain, a five-second bound, 100 ms timeout,
and strict setting policy. Output remained private under
`/private/tmp/sdrcal-airspy-10mhz.OLD12o`.

The capture transport and normalization checks passed:

- driver and hardware keys were both `AirspyHF`;
- the exact hardware serial and RX antenna matched;
- clock normalized to `soapy-driver-default` only after no selectable source
  was reported;
- frequency correction was explicitly unsupported and recorded as effective
  zero;
- center frequency and sample rate readbacks exactly matched their requests;
- 960,000 samples and 7,680,000 bytes were atomically published;
- zero timeouts, overflows, discontinuities, missing samples, and non-finite
  samples were observed; and
- deactivation, stream close, and device release succeeded, final state was
  `known_safe`, and no capture or Soapy utility process remained.

Artifact integrity at review time:

| Private artifact | Bytes | SHA-256 |
| --- | ---: | --- |
| `capture.cf32` | 7,680,000 | `c98a446ed4f6e45456a231ede56ed55daf6fdababdf1308f30f1b03f0617bac0` |
| `capture.capture.json` | 2,975 | `ef582247c4778122dbacfbc2e14a8db5652ef207bc0231e6bcc171cc1a293e3e` |

The signal-suitability preflight did not establish the intended WWV carrier.
An offline 65,536-point Hann-windowed, block-averaged spectrum found its
strongest component near -70,013.672 Hz at 22.13 dB above the spectral median.
The bins at 0 and +2.930 Hz were only 9.49 dB and 10.31 dB above the median,
respectively. This is not a production analyzer acceptance result, but it shows
that the intended near-zero-offset carrier was not dominant and does not supply
positive WWV suitability evidence under the unchanged policy.

**Diagnostic outcome: transport passed; 10 MHz reference suitability not
established.** In accordance with the no-retry plan, no 15 MHz capture or
production calibration was attempted. End-to-end calibration remains not
qualified.

## Production offline-analysis record

The separately reviewed hardware-free analysis used source revision
`c0b6b57242cbe0e249b5c49c3d1169c034e4effb` and reverified the private raw
artifact's 7,680,000-byte size and SHA-256 before reading it. A temporary,
non-installed harness outside the repository loaded exactly 960,000 CF32LE
samples and called the unmodified default `estimateCarrier` API at 192,000
samples/s.

The production estimator returned:

| Field | Result |
| --- | --- |
| Status | `ambiguous_signal` |
| Reason | Phase evolution is not coherent with one drifting carrier |
| Sample count | 960,000 |
| Mean power | `1.304926891240126e-08` |
| Model coherence | `0.25964243826014699` |
| Residual phase RMS | `1.5177842650445421` radians |

The default minimum model coherence is 0.75. Because carrier estimation failed,
the production `signal-quality-v1` analyzer was not invoked; its contract
requires a successful, sample-bound estimate. This typed rejection agrees with
the exploratory spectrum's competing off-center components and is stronger
evidence than the earlier spectral heuristic: the captured input does not
behave as one coherent drifting carrier under the immutable production model.

**Offline-analysis outcome: production estimator rejected the sample; intended
WWV suitability remains unestablished.** No estimator, analyzer, or acceptance
threshold was changed. No SDR operation, second capture, 15 MHz analysis, or
production calibration occurred.
