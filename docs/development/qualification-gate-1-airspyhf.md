# Qualification Gate 1: Airspy HF+ Discovery

## Result

Qualification Gate 1 passed on 2026-08-15 for the exact receive-only
combination below. The retained capture completed at the exact planned bound,
reported no stream errors, completed reverse-order cleanup, and left the device
available for enumeration.

This result qualifies only bounded capture-stack operation under these recorded
conditions. It does not qualify WWV reception, signal quality, frequency
estimation, observation acceptance, model fitting, uncertainty, profile
generation, calibration accuracy, other Airspy devices, other settings, other
drivers, or other platforms.

## Tested combination

| Item | Recorded value |
| --- | --- |
| Source revision | `dc3b3d83e328acfb2b7ab1f0f2a9a8700afba3fe` |
| Platform | macOS 26.5.2 build 25F84, arm64 |
| SoapySDR | library 0.8.1-release, API 0.8.0, ABI 0.8 |
| Soapy module | SoapyAirspyHF 0.2.0-7457d69 |
| Device | Airspy HF+ Discovery, serial `2f52ff5de72635ba` |
| Driver and hardware keys | `AirspyHF`, `AirspyHF` |
| Input arrangement | RX channel 0 `RX` port connected to the operator's existing antenna in an RF-hostile location |
| Requested center | 10,000,000 Hz |
| Requested sample rate | 192,000 complex samples/s |
| Requested hardware bandwidth | None |
| Requested aggregate gain | None |
| Bound | 5 seconds |
| Read timeout and policy | 100 ms, strict |
| Abort | SIGINT through the application's bounded cancellation path |

The 10 MHz setting placed WWV within the nominal receive passband, but WWV
reception was neither required nor evaluated. No test signal was injected and
no RF transmission was performed.

## Execution and retained evidence

The freshly built executable was invoked as follows:

```shell
build/default/src/cli/sdrcal-capture \
  --device driver=airspyhf \
  --device serial=2f52ff5de72635ba \
  --channel 0 \
  --frequency-hz 10000000 \
  --sample-rate 192000 \
  --duration 5 \
  --output /private/tmp/sdrcal-qualification-gate-1/20260815-airspyhf-2f52ff5de72635ba/capture \
  --timeout-ms 100 \
  --policy strict \
  --purpose "Qualification Gate 1 bounded Airspy HF+ Discovery capture"
```

The compact, privacy-reviewed manifest is retained under
`evidence/qualification-gate-1/2026-08-15-airspyhf-2f52ff5de72635ba/`.
The 7,680,000-byte raw CF32LE artifact is retained at the same location in the
qualification checkout but is ignored by Git to avoid distributing recorded
spectrum data. Its digest makes that local artifact independently identifiable.

| Artifact | Bytes | SHA-256 |
| --- | ---: | --- |
| `capture.cf32` | 7,680,000 | `91e95702065e7ad6abe911ce8d1a33bc5282ded9ac5afd4f1993a54842bbd2ec` |
| `capture.capture.json` | 2,880 | `4bff876f2138419ec25260bf56c675b206cfa56f90ea0fa3cff9d7db91c5921d` |

## Acceptance evidence

- Device selection resolved uniquely to the requested driver and serial.
- Center-frequency and sample-rate readbacks exactly matched their requests and
  were classified `applied_verified`.
- The computed and written totals were 960,000 samples and 7,680,000 bytes.
- The manifest reported 469 reads, 468 short reads, zero timeouts, zero
  overflows, and zero discontinuities. Short reads are permitted because the
  recorder continued until the exact target.
- The raw artifact's independently measured size matched both the plan and the
  manifest, and the JSON parsed successfully.
- The application reported deactivation, stream close, and device release as
  successful, with final state `known_safe`.
- No `.part` or incomplete artifact and no lingering SDR capture process
  remained. Exact serial-bound enumeration succeeded after cleanup.

## Adversarial review

The review independently recalculated `192000 * 5 = 960000` samples and
`960000 * 8 = 7680000` bytes, parsed the manifest, measured the raw file, hashed
both retained artifacts, checked stream counters, inspected cleanup details,
searched for residual files and processes, and re-enumerated the exact device.

The first pass found documentation drift: the roadmap and status page still
classified all real-device capture as unqualified, while the capture contract
still named the previously anticipated RSP1B. Those statements were narrowed to
this exact evidence-backed Airspy combination. It also clarified that the raw
capture is retained locally but intentionally excluded from Git. The next pass
found that the new report was missing from the Sphinx toctree. After that was
corrected, checksum verification exposed path entries that depended on the
caller's current directory; the ledger was made directly verifiable from the
repository root. The final repeated pass found no remaining actionable evidence,
safety, scope, privacy, reproducibility, documentation, or claim-boundary issue.
