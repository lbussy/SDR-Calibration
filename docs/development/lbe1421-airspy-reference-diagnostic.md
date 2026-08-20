# LBE-1421/Airspy reference-path diagnostic

## Result

**RC-02 complete only for the exact five-second LBE-1421 OUT1/Airspy
observation contract recorded here.**

RC-02 completion is based on the later accepted five-second OUT1 observation
described below. An earlier separately authorized five-second receive-only
diagnostic also completed at its exact bounds and passed the production carrier
estimator and `signal-quality-v1` analyzer, but it did not retain the exact
source output or complete reference conditions and did not close RC-02.

The run did not independently measure RF power or characterize the generic
attenuator. Under [decision 0026](decisions/0026-rf-level-safety-boundary.md),
those are not calibration requirements and do not by themselves block a
reference observation with a conservative operator-confirmed safety margin.
The source is classified `ad_hoc`, with assurance ceiling 50, a conservative
Type-B standard frequency uncertainty assignment of 0.5 Hz, and expanded
uncertainty 1.0 Hz using the project's normal `k = 2` reporting convention.
The assignment is not a statistical confidence claim and is not derived by
treating the manufacturer's stability specification as absolute uncertainty.
This record does not establish traceability, arbitrary-duration reference
suitability, analog linearity, calibration accuracy, a native profile, general
support, RC-01, or RC-03.

## Source and path

The source was one exact Leo Bodnar LBE-1421. Its serial is retained in private
evidence and intentionally excluded here. The RF diagnostic authorization
called it an LBE-1420 because of an operator model-number typo; a later bounded
USB identity observation established LBE-1421 for that same serialized unit.
The operator reported 10,000,000 Hz, Low Power, nominal +6 dBm, and a valid
GNSS-lock indication for the RF run. The exact one of the unit's two RF outputs
used in that run was not retained and remains unknown.

The exact path was:

```text
LBE-1421 RF output (exact output port not retained)
→ 6-inch patch cable
→ LBE-1710 10 MHz low-pass filter with DC block
→ 12-inch patch cable
→ generic nominal -10 dB, DC-1 GHz attenuator
→ Airspy HF+ Discovery RF input
```

Leo Bodnar's LBE-1421 datasheet v1.0 specifies two 50-ohm CMOS outputs and
nominal +6 dBm Low Power output below 400 MHz. The manufacturer's LBE-1710 page states
that it blocks DC, suppresses significant harmonics by more than 65 dB, is
bidirectional, and has 0.6 dB typical insertion loss at 10 MHz. Airspy publishes
+10 dBm as the HF+ Discovery maximum RF input; that is a damage boundary, not
an operating target.

Authoritative product documents retrieved 2026-08-20 were the
[LBE-1421 datasheet v1.0](https://leobodnar.com/files/datasheets/LBE-1421-Datasheet-V1.0_Initial_Release-15-07-2025.pdf),
the [LBE-1421 manufacturer page](https://www.leobodnar.com/shop/index.php?main_page=product_info&products_id=399),
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
This is a specification-based safety calculation, not a measured level or an
accuracy-bearing calibration input. The operator remains responsible for
confirming that the selected attenuation is safe for the exact SDR and source.

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
| USB-only source dossier | 3,246 | `c87bec4ed024ea8aa772e98d5706058ecbe7d2bfa4f7a9b0f254c579385f8b8f` |

## Later USB-only source observation

A separately authorized USB-only observation used plan SHA-256
`3ba0d279179650a26e3b9ddb8f13fe012f92b0bbc7ce7f897e4d22033cbf3c31`.
Both RF outputs were disconnected; no SDR was opened or streamed. USB identity
reported LBE-1421, firmware version 1.09, and the manufacturer application
reported `Device Ok`. It displayed both outputs enabled at 10,000,000 Hz and
Low Power, with 1 PPS on output 1 disabled. These later observations do not
establish which output was used in the earlier RF run.

A passive, read-only 30.001800-second virtual-serial observation transmitted no
bytes and summarized 204 NMEA sentences, all with valid checksums. The last
reported fix quality was 1, fix type was 3, and satellites used was 8. Raw NMEA,
precise location, altitude, source serial, and host device paths were not
retained publicly. The application exposed no distinct GPS-lock, PLL-lock,
antenna-health, or holdover-status field; `FLL Mode` being off is not treated as
proof of PLL lock. The application was quit, no evidence process or serial
descriptor remained, and the operator confirmed the hardware safe afterward.

The content-addressed private USB dossier is recorded alongside the RF
artifacts. It verifies bounded USB identity and observed configuration/status,
not loaded RF level, lock semantics, traceability, uncertainty, or calibration.

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
calibration result, because reference authority, frequency uncertainty, and the
full calibration chain were not established.

## RC-02 attempts and accepted five-second observation

The first RC-02 plan, SHA-256
`8c8eab5c19f820551f2d1b3ebc84db0b4a1261d8f2b87fac93dadbf6877392d5`,
failed closed on one SoapySDR overflow near the end of its only authorized
60-second capture. The next plan, SHA-256
`08fb5966b63ee28f771adef314da2ab7573c19cc73671ef80082809ae5f27d1d`,
completed a quiet-host 30-second capture. Its six five-second windows passed,
with fitted-offset range 0.0833093609 Hz, but the complete buffer failed the
10 dB SNR requirement at 5.211054 dB. Hardware-free diagnosis found slowly
varying behavior outside one linear-drift model rather than an analyzer defect;
short-window success did not override the complete-buffer failure. Neither run
closed RC-02.

The accepted plan had SHA-256
`d16633ab22989b1e1e1c4013403e4f67092ffd32198d211d254c7cfeee231eeb`.
It fixed the source output as OUT1, left OUT2 disconnected, retained the
conducted component order documented above, and limited reference validity to one five-second
observation. After 15 minutes 4 seconds of uninterrupted stationary warm-up,
the manufacturer application reported the exact LBE-1421, firmware 1.09,
`Device Ok`, OUT1 enabled at 10,000,000 Hz and Low Power, with 1 PPS disabled.
The application was used read-only and closed before acquisition.

Passive telemetry spanned the capture. All 151 NMEA sentences had valid
checksums; the last GGA fix quality was 2, GSA fix type was 3, and satellites
used was 8. No raw NMEA, location, altitude, source serial, or host device path
is retained publicly. Continuous fixes and unambiguous status were required;
the application exposed no distinct PLL-lock or internal holdover field, so the
record makes no broader lock-semantics claim.

The strict capture wrote exactly 960,000 CF32LE samples and 7,680,000 bytes.
Center frequency and sample rate were applied and verified; bandwidth and gain
were unrequested and unverified. It reported zero timeouts, overflows, and
discontinuities. Deactivation, stream close, device release, post-run exact
enumeration, and operator-confirmed reverse-order physical cleanup succeeded;
final software state was `known_safe`.

| Accepted artifact | Bytes | SHA-256 |
| --- | ---: | --- |
| Five-second CF32LE capture | 7,680,000 | `447091e5ade3f982f3f68656faee42d48fa1ae41bc30ba89ccae4f302e56f53f` |
| Capture manifest | 2,977 | `39c567b2746b21d0ba10e994b0f40bef0b94b1727fc17de80deaaf2d5a1122ab` |
| Privacy-redacted NMEA summary | 195 | `c3a19f2c28633e8bf5a3edc3fa7c88eba21c4581c766c9f8389a95a7d8df19d6` |
| Offline-analysis record | 2,952 | `1f0f800bcdd08dd29a1e204c227e1ede09baa02b3b3261e41d0e6606ef2a13cb` |

The unchanged production estimator and `signal-quality-v1` analyzer passed:

| Metric | Result |
| --- | ---: |
| Fitted offset from requested center | +1.8046368506 Hz |
| Fitted drift | +0.0247365805 Hz/s |
| Model coherence | 0.9999999698 |
| Residual phase RMS | 0.0002456560 rad |
| Signal-to-noise ratio | 39.215317 dB |
| Digital-rail clipped samples | 0 |
| Frequency-instability metric | 0.0290189284 Hz |
| Interference-to-carrier ratio | -86.417357 dB |
| Instability windows / FFT samples | 937 / 65,536 |

The new offset was 0.1286135430 Hz above the prior six-window range, within the
assigned expanded limitation of 1.0 Hz. This supports bounded repeatability; it
does not establish absolute traceability or reduce the assigned uncertainty.
`observation-acceptance-v1` therefore passed only for the exact five-second
conditions.

## Gate status and next step

- RC-01 remains blocked: this run did not compare two receivers under a
  characterized, rotated splitter topology.
- RC-02 is complete only for the exact LBE-1421 OUT1, path, Airspy, settings,
  assigned uncertainty, GNSS/warm-up conditions, and five-second observation
  duration retained above. The source remains `ad_hoc`, ceiling 50.
- RC-03 remains blocked: no calibration observation set, model, uncertainty
  budget, profile, or in-domain evaluation was produced.

Any longer duration, changed output, path, receiver, setting, condition, or
stronger reference-authority claim requires a new plan and evidence. This
record itself does not authorize reconnection or RC-03.

## Rejected 20 MHz different-frequency attempt

A separately authorized execution of the immutable
[20 MHz reference plan](lbe1421-airspy-20mhz-reference-execution-plan.md),
SHA-256 `e15d46611d7280aefd2d9b09f04427bc02632bce43ac5941b828ebdb4f70a1a9`,
used the same private-serial LBE-1421 OUT1 and Airspy serial, two generic nominal
-10 dB attenuators, and no LBE-1710 or external DC block. The manufacturer
application reported firmware 1.09, OUT1 at 20 MHz Low Power, 1 PPS disabled,
Stationary, and `Device Ok`. The application was closed before Airspy access.

The only authorized five-second capture completed with exact 20,000,000 Hz and
192,000 samples/s readback, 960,000 CF32LE samples, 7,680,000 bytes, zero
timeouts, overflows, or discontinuities, final state `known_safe`, and exact
pre-run and post-run serial-filtered Airspy enumeration. No retry occurred. The
operator subsequently confirmed reverse-order physical cleanup and `Hardware
safe`.

The unchanged production estimator and `signal-quality-v1` analyzer produced:

| Metric | Result |
| --- | ---: |
| Fitted offset from requested center | +3.1951333656 Hz |
| Fitted drift | +0.0602400846 Hz/s |
| Model coherence | 0.9999998560 |
| Residual phase RMS | 0.0005366622 rad |
| Signal-to-noise ratio | 19.064248 dB |
| Digital-rail clipped samples | 0 |
| Frequency-instability metric | 5.514993 Hz |
| Interference/carrier | -78.237621 dB |
| Instability windows / FFT samples | 937 / 65,536 |

`observation-acceptance-v1` rejected the observation because frequency
instability exceeded the 0.5 Hz maximum. The successful transport, estimator,
SNR, clipping, drift, and interference checks do not override that rejection.
This run does not qualify the 20 MHz reference path and supplies no accepted
second-frequency observation for RC-03.

Private artifacts remain outside Git and are content-addressed by:

| Artifact | Bytes | SHA-256 |
| --- | ---: | --- |
| Five-second CF32LE capture | 7,680,000 | `38408580ec22971ee27ffac0ae6aea1ea58a669ee311e41aa6d516fa4a2d58ee` |
| Capture manifest | 2,983 | `fbd8bcdda5e0052da0ec09bda6e8a81cf04d0da305675c283626535099c66d69` |
| Offline-analysis record | 2,425 | `bfbc8e0a6fe7f2c0c101882fe3e0c89cb1d358c57525776d2ec650ec9e16bb70` |
| Cleanup record | 722 | `5cf004e18b552c19f779db9f292e4ae42c0a8fae2bb71ddc8aa9308fa8d206c7` |

The accepted 10 MHz RC-02 scope remains unchanged. Any diagnosis, repeat,
changed attenuation, changed duration, or alternative frequency requires a new
plan and authorization.
