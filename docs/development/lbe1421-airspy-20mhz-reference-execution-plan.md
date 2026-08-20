# LBE-1421/Airspy 20 MHz reference execution plan

## Purpose and claim boundary

Qualify one and only one five-second 20 MHz RC-02 observation using the exact
Airspy HF+ Discovery already qualified at 10 MHz. This plan does not execute
RC-03, generate a profile, complete RC-01, qualify another receiver, establish
traceability, or establish arbitrary-duration or general support.

The source remains `ad_hoc`, with assurance ceiling 50, conservative Type-B
standard frequency uncertainty 0.5 Hz, and expanded uncertainty 1.0 Hz at
`k = 2`. Passing this plan provides the different-frequency observation needed
for a later separately authorized Airspy-only linear-model RC-03 attempt.

## Frozen hardware and path

- Source: the exact private-serial Leo Bodnar LBE-1421 used for the accepted
  10 MHz observation; firmware 1.09; OUT1 only; OUT2 disconnected.
- Source setting: OUT1 enabled at exactly 20,000,000 Hz, Low Power; 1 PPS on
  OUT1 disabled.
- Receiver: Airspy HF+ Discovery serial `2f52ff5de72635ba`, RF input, RX channel
  0, Soapy selection `driver=airspyhf,serial=2f52ff5de72635ba`.
- Physical order: LBE-1421 OUT1 → existing 6-inch 50-ohm patch cable → first
  generic nominal -10 dB, DC-1 GHz, 50-ohm attenuator → second generic nominal
  -10 dB, DC-1 GHz, 50-ohm attenuator → existing 12-inch 50-ohm patch cable →
  Airspy HF+ Discovery RF input.
- The LBE-1710 filter/DC block is absent. No external DC block is used.
- The [Airspy designer's published design statement](https://groups.io/g/airspy/topic/hf_discovery_specs/31743220)
  says the HF+ Discovery RF input is coupled through a 1 µF capacitor under the
  RF shield. This supports the frozen no-external-block topology but is not a
  general DC-withstand claim.
- Before connection, the operator must confirm that both attenuators are the
  stated 50-ohm DC-1 GHz nominal -10 dB type, undamaged, and suitable for the
  connectors and source level. A mismatch or unknown component aborts the plan.

Nominal level calculation, before uncharacterized cable loss:

```text
+6 dBm nominal LBE-1421 Low Power output below 400 MHz
-10 dB nominal first attenuator
-10 dB nominal second attenuator
= -14 dBm nominal Airspy input
```

This is nominal specification evidence, not a measurement. It provides 24 dB
nominal separation from the published Airspy +10 dBm maximum RF input. The
operator must confirm that this is a conservative safe margin for the exact
components. Unexpected heating, odor, instability, overload, or compression
aborts immediately.

## Immutable prerequisites

Before any enumeration or connection, the operator must confirm all of the
following in one message:

- `Hardware ready`.
- The exact source and Airspy identities above.
- The exact physical order above, including two attenuators and no filter or
  external DC block.
- OUT2 is disconnected.
- Both attenuators meet the stated type, impedance, range, connector, condition,
  and level requirements.
- The source and Airspy are stationary indoors and physically normal.
- The host is quiet: no SDR, capture, calibration, vendor-control, audio, or
  competing high-load process will use the device or interval.
- At least 15 uninterrupted minutes of warm-up will be timed after the final
  20 MHz configuration.
- The manufacturer application will report the exact LBE-1421, firmware 1.09,
  `Device Ok`, OUT1 20 MHz Low Power, and 1 PPS disabled.
- Continuous checksum-valid GNSS/NMEA evidence will show a valid fix throughout
  the five-second observation, with no status ambiguity or suspected holdover.
- The private evidence directory below does not exist and has sufficient space.

Any missing or qualified confirmation stops before hardware access.

## Execution bounds and command

Use exactly one attempt. Private output basename:

```text
/private/tmp/sdrcal-rc02-20mhz-20260820-attempt-1/capture
```

After the prerequisites and separate digest-bound authorization are received:

1. Exclude competing processes without killing unrelated work. If exclusion
   cannot be established, stop.
2. Configure and observe the LBE-1421 prerequisites, then close the manufacturer
   application before SDR enumeration.
3. Connect the frozen path in source-to-receiver order while the source output
   is disabled or isolated; enable only OUT1 after the complete path is checked.
4. Enumerate with the exact driver and serial filter. Exactly one matching
   Airspy must be returned; zero, multiple, or changed identity aborts. Use:

```shell
SoapySDRUtil --find="driver=airspyhf,serial=2f52ff5de72635ba"
```

5. Run only this production capture command:

```shell
build/default/src/cli/sdrcal-capture \
  --device driver=airspyhf \
  --device serial=2f52ff5de72635ba \
  --channel 0 \
  --frequency-hz 20000000 \
  --sample-rate 192000 \
  --duration 5 \
  --timeout-ms 100 \
  --policy strict \
  --purpose "RC-02 LBE-1421 OUT1 20 MHz five-second reference qualification" \
  --output /private/tmp/sdrcal-rc02-20mhz-20260820-attempt-1/capture
```

No bandwidth, gain, clock-source, antenna, or frequency-correction override is
permitted. Center frequency and sample rate must read back exactly. Bandwidth
and gain remain unrequested; driver-default gain behavior and the reviewed
Airspy clock/correction normalization remain unchanged.

The acquisition must terminate within 15 seconds of command start. SIGINT is
the only permitted cooperative abort. If SIGINT does not complete cleanup
within five additional seconds, stop further device access and classify final
software state as unknown.

## Acceptance and fail-closed conditions

The capture must contain exactly 960,000 finite CF32LE samples and 7,680,000
bytes. The manifest must report complete atomic publication, exact requested
and effective center/sample rate, zero timeouts, overflows, missing samples,
and discontinuities, successful deactivation/stream close/device release/raw
cleanup, and final state `known_safe`.

Analyze the exact hashed CF32LE bytes once through unchanged production
`CarrierEstimatorOptions`, `signal-quality-v1`, and
`observation-acceptance-v1`. Acceptance requires:

- estimator success;
- model coherence at least 0.75;
- duration at least 1.0 second and exactly five seconds by the plan;
- SNR at least 10 dB;
- clipped-sample fraction exactly zero;
- absolute fitted drift no more than 1.0 Hz/s;
- frequency-instability metric no more than 0.5 Hz;
- interference-to-carrier ratio no greater than -10 dB;
- effective configuration verified;
- reference suitable and every frozen reference condition evidenced.

Any command, transport, identity, setting, condition, artifact, estimator,
quality, acceptance, timeout, or cleanup discrepancy is final failure. Do not
retry, extend duration, change gain, change attenuation, reconnect, or substitute
hardware under this plan.

## Abort and reverse-order cleanup

Abort on GNSS/fix loss or ambiguity, suspected holdover, source-status change,
identity or readback mismatch, unexpected DC or level behavior, overload,
clipping, heating, odor, unstable power, timeout, overflow, discontinuity,
non-finite sample, artifact-bound failure, or any unplanned condition.

Cleanup in reverse order: stop acquisition; deactivate and close the stream;
release the Airspy; disable or isolate LBE-1421 OUT1; disconnect the Airspy end,
12-inch cable, second attenuator, first attenuator, 6-inch cable, and source end;
leave OUT2 disconnected; close evidence processes and the manufacturer
application. Perform exact serial-filtered post-run Airspy enumeration only
after software cleanup using:

```shell
SoapySDRUtil --find="driver=airspyhf,serial=2f52ff5de72635ba"
```

The operator must then state `Hardware safe`. Unknown software or physical
state fails the run regardless of useful samples.

## Evidence, privacy, and post-run review

Keep raw IQ, source serial, NMEA/location data, host paths, and photographs
private and outside Git. Retain SHA-256 and byte count for the plan, CF32LE,
capture manifest, privacy-redacted GNSS summary, offline-analysis record, and
cleanup/post-enumeration record. Public documentation may contain only redacted
conditions, exact Airspy serial already public, metrics, byte counts, hashes,
and an honest RC-02 disposition.

After `Hardware safe`, independently verify hashes, privacy, identity and
configuration binding, condition evidence, acceptance calculations, cleanup,
and claim boundaries. A pass qualifies only this exact five-second 20 MHz
LBE-1421/two-attenuator/Airspy path. It does not automatically execute RC-03.

## Forbidden claims and actions

Do not claim traceability, measured RF power, analog linearity from zero digital
clipping, arbitrary-duration suitability, general Airspy support, RSP1B or
receiver equivalence, RC-01 completion, RC-03 completion, or a
qualification-capable profile. Do not access another SDR, use OUT2, radiate,
transmit intentionally, repeat the run, generate a profile, or alter production
code or thresholds under this plan.

## Required separate authorization

Execution requires this exact text with the published plan SHA-256 substituted:

```text
I authorize exactly one execution of the LBE-1421/Airspy 20 MHz reference plan
with SHA-256 <PLAN_SHA256>. I confirm every immutable prerequisite and Hardware
ready. I authorize only the exact source, two-attenuator path, Airspy serial,
settings, five-second capture, evidence collection, abort, enumeration, and
cleanup stated in that plan. No retry, substitution, RC-03 execution, profile
generation, or other hardware access is authorized.
```

After authorization, any changed plan text or prerequisite requires a new
digest and authorization.
