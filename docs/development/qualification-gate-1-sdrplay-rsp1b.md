# Qualification Gate 1: SDRplay RSP1B

## Result

Qualification Gate 1 passed on 2026-08-17 for the exact receive-only
combination below. The retained capture completed at the exact planned bound,
reported no overflow or discontinuity, completed reverse-order cleanup, and
left the device available for exact serial-bound enumeration.

This result qualifies only bounded capture-stack operation under these recorded
conditions. It does not qualify WWV reception, signal quality, frequency
estimation, observation acceptance, model fitting, uncertainty, profile
generation, calibration accuracy, other SDRplay devices, other settings, other
drivers, or other platforms. The separate production estimator rejected this
antenna sample as ambiguous, so it is not suitable calibration evidence.

## Tested combination

| Item | Recorded value |
| --- | --- |
| Source revision | `0bc98ecb7c4b49fc3c24a827ede3f2714164b638` |
| Capture executable SHA-256 | `f845db0408f2862a1d93fcfa9f011b1733d3acbe48cdd976aabf6d4723ab1501` |
| Platform | macOS 26.5.2 build 25F84, arm64 |
| SoapySDR | library 0.8.1-release, API 0.8.0, ABI 0.8 |
| Soapy module | SoapySDRPlay3 0.5.2-6cc3131 |
| SDRplay API | 3.15 |
| Device | SDRplay RSP1B, serial `2404058C60`, hardware version 6 |
| Driver and hardware keys | `SDRplay`, `RSP1B` |
| Input arrangement | RX channel 0 `RX` port connected to the same small antenna previously used for the Airspy HF+ Discovery diagnostic |
| Requested center | 10,000,000 Hz |
| Requested sample rate | 192,000 complex samples/s |
| Effective hardware bandwidth | 200,000 Hz |
| Effective aggregate gain | 30 dB, automatic gain enabled |
| Effective correction | 0 ppm |
| Bound | 5 seconds |
| Read timeout and policy | 100 ms, strict |

The executable used for the run was hashed before the evidence was recorded.
An immediate `cmake --build --preset default --target sdrcal-capture` from the
clean source revision reported no work to do and reproduced that exact hash.

## Execution and retained evidence

The executable was invoked as follows:

```shell
build/default/src/cli/sdrcal-capture \
  --device driver=sdrplay \
  --device serial=2404058C60 \
  --channel 0 \
  --frequency-hz 10000000 \
  --sample-rate 192000 \
  --duration 5 \
  --output /private/tmp/sdrcal-rsp1b-10mhz.5B33ht/capture \
  --timeout-ms 100 \
  --policy strict \
  --purpose "Bounded pre-calibration SDRplay RSP1B antenna test"
```

The compact manifest is retained under
`evidence/qualification-gate-1/2026-08-17-sdrplay-rsp1b-2404058C60/`.
The raw CF32LE artifact is retained locally at the same path but ignored by Git
to avoid distributing recorded spectrum data. Its digest makes the local
artifact independently identifiable.

| Artifact | Bytes | SHA-256 |
| --- | ---: | --- |
| `capture.cf32` | 7,680,000 | `36c49b36a8cea52d7233c01ec04bd5c77bf22527571e31f11735054e56dbdee7` |
| `capture.capture.json` | 2,988 | `19a7e2b6899ef8d37455edd8a42f3374e72cb26a8801959b8def8e0d6888ec38` |

## Acceptance evidence

- SDRplay-only enumeration resolved uniquely to the requested RSP1B serial.
- Center-frequency and sample-rate readbacks exactly matched their requests and
  were classified `applied_verified`.
- The computed and written totals were 960,000 samples and 7,680,000 bytes.
- The manifest reported 236 reads, 234 permitted short reads, one recovered
  timeout, zero overflows, and zero discontinuities.
- The raw artifact's independently measured size matched both the plan and the
  manifest, and its retained digest was reverified after copying.
- The application reported deactivation, stream close, and device release as
  successful, with final state `known_safe`.
- No capture process remained, and exact serial-bound enumeration succeeded
  after cleanup.

## Separate signal-suitability result

A temporary non-installed harness read the exact hashed CF32LE artifact and
called the unchanged production carrier estimator at 192,000 samples/s. The
estimator rejected the sample with status `ambiguous_signal`: model coherence
was 0.085582808910928107 against the default minimum 0.75, residual phase RMS
was 1.719478397040576 radians, and mean power was
0.000049962031532777475. Because estimation failed, the production signal-
quality analyzer was not invoked.

This rejection does not invalidate the bounded transport result. It establishes
that the captured antenna input is not suitable evidence for WWV reception or
calibration under the unchanged production policy. No 15 MHz capture or
production calibration was attempted.
