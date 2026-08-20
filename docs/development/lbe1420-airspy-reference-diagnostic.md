# LBE-1420/Airspy reference-path diagnostic

## Result

**Suitable for the next bounded qualification step for this exact unmeasured
path.**

On 2026-08-20, one separately authorized five-second receive-only diagnostic
completed at its exact bounds. The production carrier estimator and
`signal-quality-v1` analyzer accepted the retained sample. This establishes
bounded transport and coherent-signal suitability only for the exact run.

The operator explicitly waived the readiness package's independent
preconnection level measurement. The generic attenuator was not characterized.
Consequently this record does not establish electrical safety, analog
linearity, traceability, uncertainty, calibration accuracy, a native profile,
general support, or completion of RC-01, RC-02, or RC-03. Durable connection
readiness remains `NOT READY TO CONNECT`.

## Source and path

The source was one exact Leo Bodnar LBE-1420. Its serial is retained in the
private execution record and intentionally excluded here. The operator reported
10,000,000 Hz, Low Power, nominal +6 dBm, and a valid GNSS-lock indication.
Firmware, unit-specific calibration evidence, and traceability records were not
retained for this run.

The exact path was:

```text
LBE-1420 RF output
→ 6-inch patch cable
→ LBE-1710 10 MHz low-pass filter with DC block
→ 12-inch patch cable
→ generic nominal -10 dB, DC-1 GHz attenuator
→ Airspy HF+ Discovery RF input
```

Leo Bodnar's LBE-1420 datasheet v1.1 specifies a 50-ohm CMOS output and nominal
+6 dBm Low Power output below 400 MHz. The manufacturer's LBE-1710 page states
that it blocks DC, suppresses significant harmonics by more than 65 dB, is
bidirectional, and has 0.6 dB typical insertion loss at 10 MHz. Airspy publishes
+10 dBm as the HF+ Discovery maximum RF input; that is a damage boundary, not
an operating target.

Authoritative product documents retrieved 2026-08-20 were the
[LBE-1420 datasheet v1.1](https://leobodnar.com/files/datasheets/LBE-1420-Datasheet-v1.1.pdf),
the [LBE-1420 manufacturer page](https://www.leobodnar.com/shop/index.php?main_page=product_info&products_id=393),
the [LBE-1710 manufacturer page](https://www.leobodnar.com/shop/index.php?main_page=product_info&products_id=400),
and the [Airspy HF+ Discovery specifications](https://airspy.com/airspy-hf-discovery/).
No unit-specific calibration certificate was available.

The nominal-only calculation was:

```text
+6.0 dBm nominal source
−0.6 dB typical filter loss
−10.0 dB nominal attenuator loss
= −4.6 dBm nominal receiver input before uncharacterized cable loss
```

The corresponding nominal separation from the published maximum is 14.6 dB.
It is not a measured level or verified engineering margin. Source tolerance,
attenuator tolerance and return loss, cable loss, mismatch, and measurement
uncertainty remain unknown.

## Authorization and execution

The exact override plan had SHA-256
`2d9d24743810c56944d5c87917979282799254148daa28754098e49c34a449ba`.
It authorized one unmeasured-path diagnostic and one attempt only. The operator
confirmed the frozen topology, valid lock indication, source setting, normal
physical condition, and exclusive test interval before enumeration.

Serial-filtered enumeration returned exactly one Airspy HF+ Discovery,
`2f52ff5de72635ba`. The capture requested RX channel 0, 10,000,000 Hz center,
192,000 complex samples/s, no explicit hardware bandwidth or aggregate gain,
driver-default gain behavior, no requested frequency correction, a 100 ms read
timeout, strict setting policy, and a five-second bound.

Center frequency and sample rate were applied and verified. Bandwidth and gain
remained not requested and unverified. The run wrote exactly 960,000 CF32LE
samples and 7,680,000 bytes from `2026-08-20T12:03:57Z` through
`2026-08-20T12:04:02Z`. It reported 469 reads, 468 permitted short reads, and
zero timeouts, overflows, or discontinuities.

Acquisition deactivation, stream close, device release, and raw-output cleanup
succeeded. Final software state was `known_safe`; no capture process remained;
exact serial-bound post-run enumeration succeeded; and the operator confirmed
reverse-order physical cleanup. No retry or second receiver operation occurred.

## Private evidence and analysis

Raw IQ, source serial, private host detail, and runtime paths remain outside
Git. The retained private artifacts are content-addressed by:

| Artifact | Bytes | SHA-256 |
| --- | ---: | --- |
| CF32LE capture | 7,680,000 | `8aff217fcde4af4329a823f4ef6f638be0bf4b987d79153055b10f34997349ea` |
| Capture manifest | 2,975 | `c7692ce1044e69ffb02485eb834bc494b73c09078dd501db95175274cf4dbc95` |
| Offline-analysis record | 1,611 | `6dd85f4fef6ca8bf73b3fe6af677d8759a8bf04de447d0a204536ecceda7fa7e` |

A temporary, non-installed harness read the exact hashed CF32LE artifact once
and invoked the unchanged production APIs with default
`CarrierEstimatorOptions` and `signal-quality-v1`:

| Metric | Result |
| --- | ---: |
| Estimator status | Success |
| Fitted offset from requested center | +1.7433092626 Hz |
| Fitted drift | +0.0253180007 Hz/s |
| Model coherence | 0.9999999697 |
| Residual phase RMS | 0.0002462659 rad |
| Signal-to-noise ratio | 37.051229 dB |
| Digital-rail clipped samples | 0 |
| Frequency-instability metric | 0.064507121 Hz |
| Interference-to-carrier ratio | -68.734015 dB |
| Instability windows / FFT samples | 937 / 65,536 |

These values show one strongly coherent signal under the exact digital capture
conditions. Zero digital-rail clipping cannot exclude analog front-end overload
or AGC-masked compression. The offset and drift are observations, not a
calibration result, because reference authority, path level, uncertainty, and
the full calibration chain were not established.

## Gate status and next step

- RC-01 remains blocked: this run did not compare two receivers under a
  characterized, rotated splitter topology.
- RC-02 remains blocked: source authority, unit-specific traceability,
  measurement uncertainty, verified loaded level, characterized attenuation,
  and complete environmental/lock evidence remain incomplete.
- RC-03 remains blocked: no calibration observation set, model, uncertainty
  budget, profile, or in-domain evaluation was produced.

A later RC-02 attempt requires a new plan. Under the durable readiness contract
it must close the missing source dossier, characterize the attenuation path,
verify loaded level with suitable independent equipment, retain uncertainty and
lock/holdover evidence, and receive separate authorization. This record itself
does not authorize reconnection.
