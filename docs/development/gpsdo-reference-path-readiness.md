# GPSDO and reference-path readiness package

## Current decision

**Reference-suitability status: COMPLETE for the exact five-second LBE-1421
OUT1/Airspy observation contract recorded below.**

The exact source is classified `ad_hoc`, with assurance ceiling 50, a
conservative Type-B standard frequency uncertainty assignment of 0.5 Hz, and
expanded uncertainty 1.0 Hz using the project's normal `k = 2` reporting
convention. This engineering assignment is not a statistical confidence claim
or unit-specific traceability. A separately authorized five-second observation retained exact OUT1,
warm-up, continuous GNSS conditions, transport, production analysis, cleanup,
and operator safe-state evidence. Its
[record](lbe1421-airspy-reference-diagnostic.md) closes RC-02 only for that
exact source, path, receiver, configuration, condition set, and five-second
duration. It does not establish traceability, arbitrary-duration suitability,
calibration accuracy, a profile, RC-01, or RC-03.

No step in this document authorizes powering equipment, connecting cables,
enumerating an SDR, starting a stream, or performing a receive test.

## Intended claim boundary

The completed physical activity was a bounded receive-only reference-path
observation. It established transport and reference suitability only for one
exact five-second contract. It did not establish reference traceability,
observation independence, calibration accuracy, a native profile, or
end-to-end qualification. No future activity is authorized by this record.

Candidate receivers currently known to the project are:

| Receiver | Current evidence | Reference-path status |
| --- | --- | --- |
| Airspy HF+ Discovery serial `2f52ff5de72635ba` | Exact recorded macOS combination is bounded-capture-qualified; exact LBE-1421 OUT1 path passed the five-second RC-02 contract | Reference-suitable only for the retained five-second conditions; not suitable for arbitrary duration |
| SDRplay RSP1B serial `2404058C60` | Exact recorded macOS combination is bounded-capture-qualified | Input topology and limits unverified; not ready |

## Completed five-second reference qualification

The accepted 2026-08-20 observation used LBE-1421 OUT1, the attenuated RF-input
topology, an LBE-1710 filter/DC block, and a generic nominal -10 dB attenuator.
Its digest-bound plan, 15-minute warm-up, read-only source status, continuous
valid GNSS fixes, transport, cleanup, private artifact hashes, production
analysis, and repeatability comparison are retained in the
[diagnostic record](lbe1421-airspy-reference-diagnostic.md).

The run used published nominal source and path values rather than an independent
preconnection level measurement. Under
[decision 0026](decisions/0026-rf-level-safety-boundary.md), that is not by
itself a reference-suitability blocker when the operator confirms a compatible
50-ohm path and a conservative level calculation. The accepted run retained
that confirmation and a 14.6 dB nominal separation from the published maximum.
The source remains non-traceable and `ad_hoc`; the assigned uncertainty and
five-second validity ceiling prevent that limitation from being hidden.

## Gate 1: exact source dossier

Complete every field from retained manufacturer documentation, calibration
records, or direct measurement. Record document titles, revisions, retrieval
dates, and page or section references.

| Field | Required evidence | Current value |
| --- | --- | --- |
| Manufacturer, exact model, serial | Nameplate plus authoritative manual | Leo Bodnar LBE-1421; exact serial retained privately; exact USB identity observed 2026-08-20 |
| Output port and connector | Labeled port and manual | Accepted run used OUT1 SMA female; OUT2 remained disconnected; output 1 supports 1 Hz-800 MHz or 1 PPS |
| Nominal frequency and available modes | Manual; exact selected mode later verified | Accepted run verified OUT1 enabled at 10 MHz, Low Power, with 1 PPS disabled |
| Waveform and harmonic specification | Manual or measured record | LBE-1421 datasheet: CMOS square wave; LBE-1710 page: greater than 65 dB significant-harmonic suppression; not independently measured |
| Output level | Minimum, nominal, and maximum with units | +6 dBm nominal Low Power below 400 MHz; tolerance and received-unit extrema unverified |
| Level convention | dBm, dBV, Vrms, Vpp, open-circuit, or specified load | Datasheet specifies 1.65 V into 50 ohms and +6 dBm Low Power; exact power convention/tolerance not fully stated |
| Source impedance and required load | Manual | 50-ohm source; assembled loaded output not measured |
| DC content or bias | Manual and measurement method | LBE-1710 manufacturer page states DC removal; assembled-path DC not independently measured |
| Warm-up requirement | Manual and planned monotonic timer | Accepted plan assigned and retained at least 15 minutes; actual interval was 15 minutes 4 seconds |
| Lock indication and lock-loss behavior | Manual plus observable indication | Application reported `Device Ok`; all 151 passive NMEA sentences had valid checksums, last GGA fix quality 2 and GSA fix type 3; no distinct PLL-lock field was available |
| Holdover behavior | Manual; state whether holdover is permitted | Holdover was prohibited; continuous valid GNSS fixes and no status ambiguity were required, but no distinct internal holdover field was available |
| Environmental limits | Manual and planned observation method | Exact run was stationary under operator-observed normal indoor conditions; broader environmental validity is not claimed |
| Power supply and grounding | Manual and bench arrangement | Datasheet specifies USB-C, 5 V ±10%, 250 mA ±10%; USB reported 250 mA; exact bench supply/grounding record incomplete |
| Traceability chain | Current calibration/specification evidence | None independently established; classified `ad_hoc`, assurance ceiling 50 |
| Frequency uncertainty | Value, coverage, conditions, validity interval | Conservative Type-B engineering assignment: standard 0.5 Hz; expanded 1.0 Hz using `k = 2`; no statistical confidence or traceability claim; valid only for the retained exact five-second conditions |

An order page or nominal product specification may identify a candidate. It
does not prove the received unit's identity, condition, configured output,
traceability, or uncertainty.

## Gate 2: choose the electrical topology

Exactly one topology must be selected and justified before calculating the
path. The selection is part of the later approval digest.

### Receiver reference-clock input

Use only when the exact receiver and driver expose a documented compatible
reference-clock input and the software can preserve requested and effective
clock state. Verify frequency, waveform, level, impedance, connector, lock or
clock-detection behavior, fallback behavior, and whether switching clocks while
the device is active is prohibited.

### Attenuated RF calibration-signal input

Use only through the exact receiver RF input with a path that reduces the
source to a documented safe and useful level. Verify absolute maximum input,
recommended operating range, impedance, DC tolerance, preselector/bandwidth,
overload behavior, and whether AGC or gain settings can obscure clipping.

These topologies are not interchangeable. A reference-clock output must never
be connected to an RF or clock port merely because its connector fits.

## Gate 3: receiver input dossier

Complete one copy for each proposed receiver and input port.

| Field | Required value |
| --- | --- |
| Exact receiver, serial, driver/module/API | Airspy HF+ Discovery `2f52ff5de72635ba`; `AirspyHF`; SoapySDR 0.8.1/API 0.8.0/ABI 0.8; exact module version not retained in this run |
| Exact physical port and intended function | RX channel 0, antenna `RX`, RF input |
| Input impedance across the test frequency | Unverified |
| Absolute maximum input, including duration conditions | Manufacturer publishes +10 dBm maximum RF input; duration conditions unverified |
| Recommended continuous diagnostic level | Unverified |
| DC tolerance and required DC block | Receiver DC tolerance unverified; LBE-1710 provided DC blocking in the exact diagnostic path |
| ESD or overload protection limitations | Unverified |
| Supported external-clock contract, if applicable | Unverified |
| Requested sample rate, bandwidth, gain, AGC, antenna, clock, correction | Diagnostic used 192,000 samples/s; no bandwidth/gain request; RX; driver-default clock; unsupported correction normalized to zero |
| Required effective readbacks and normalization provenance | Center and sample rate verified; bandwidth/gain unrequested and unverified; clock/correction used reviewed Airspy normalization |

Treat an absolute maximum as a damage boundary, not a target. The later plan
must choose a lower operating level with a documented engineering margin.

## Gate 4: level and attenuation worksheet

Do not mix voltage conventions. Convert only after establishing waveform,
source impedance, load impedance, and whether the stated voltage is loaded or
open circuit.

For a sinusoid measured across the intended resistive load:

```text
Vrms = Vpp / (2 * sqrt(2))
P_watts = Vrms^2 / R_ohms
P_dBm = 10 * log10(P_watts / 0.001)
```

For a matched path expressed in dB:

```text
receiver_level_dBm = source_level_dBm
                     - fixed_attenuation_dB
                     - splitter_loss_dB
                     - cable_and_adapter_loss_dB
```

The safety calculation uses worst cases, not nominal values:

```text
highest_receiver_level = highest_source_level
                         - lowest_total_path_loss

required_attenuation >= highest_source_level
                        - chosen_receiver_operating_level
                        + engineering_margin
```

Use a published source level and path loss or justified conservative bounds,
and retain the engineering margin separately. The operator must confirm that
the calculated receiver level remains below the documented absolute maximum
with a conservative margin. Independent measurement is required only when the
available information cannot establish safety, the margin is small, or
overload or compression is suspected. Power-meter uncertainty is a path-safety
input, not a frequency-calibration uncertainty component.

## Gate 5: path inventory

| Item | Required retained information |
| --- | --- |
| Fixed or step attenuator | Asset ID, nominal attenuation, tolerance, frequency range, impedance, power rating |
| Splitter or divider | Asset ID, topology, port isolation, insertion loss, impedance, frequency and power ratings |
| DC block | Asset ID, voltage rating, frequency range, insertion loss, impedance |
| Terminators | Asset ID, impedance, power rating, frequency range |
| Cables and adapters | Endpoints, impedance, length, loss or bound, frequency and power ratings |
| Switch or relay, if any | Isolation, unterminated-state behavior, sequencing, ratings |
| Measurement instrument, when required or used | Model, serial, stated accuracy or assigned limitation, probe/load configuration |

Every unused splitter port must have a reviewed disposition. Do not combine an
antenna and local source without a separately reviewed combiner/isolation plan.
Prevent receiver-to-receiver and source-to-antenna back-feed. Connector adapters
do not correct an impedance or level mismatch.

## Gate 6: pre-connection review

The later reviewer must answer every item positively:

- Exact source, receiver, port, and component identities are recorded.
- The selected topology is supported by authoritative documentation.
- Units, load convention, impedances, and DC behavior are unambiguous.
- The conservative level calculation uses published specifications or justified
  conservative bounds and an explicit margin.
- Independent level measurement is retained when the available information
  cannot establish safety, the margin is small, or overload or compression is
  suspected.
- GPS lock, warm-up, environment, and holdover policy are observable.
- The operator owns the hardware and test interval; conflicting users and
  processes are excluded.
- Requested and required effective SDR settings are frozen.
- Duration, sample, memory, and artifact bounds are explicit.
- Abort, cleanup, post-run enumeration, and evidence paths are explicit.
- The exact plan text has an immutable SHA-256 and separate authorization.

Any unresolved safety item keeps the path not ready. Missing optional power
measurement alone does not block the path.

## Future bounded diagnostic plan template

Complete this template only after Gates 1 through 6 pass. Its completion still
does not authorize execution.

```text
Plan identifier and SHA-256:
Source identity and output:
Reference class and evidence ceiling:
Receiver identity, driver, platform, and input port:
Selected topology:
Complete connection order:
Published source level or conservative bound and load convention:
Path components, loss bounds, and calculated receiver-level bound:
Receiver safe limit, chosen operating level, and margin:
Independent level measurement, if required or used, with its limitation:
Warm-up start, minimum duration, and lock acceptance:
Requested center frequency, sample rate, bandwidth, gain/AGC, antenna,
clock source, and driver correction:
Required effective readbacks:
Capture duration/sample/memory/byte bounds:
Estimator and acceptance policy versions:
Reference-condition evidence:
Artifact and privacy disposition:
Abort method and triggers:
Reverse-order cleanup and safe-state checks:
Post-run exact serial-bound enumeration:
Independent hashes and evidence-review procedure:
Stop criteria and prohibited retries:
Authorization scope, operator, and time window:
```

## Abort and cleanup contract

Abort before or during a future diagnostic upon lock loss or ambiguous lock,
unexpected level or DC, incorrect load, heating, odor, unstable power,
unplanned clock fallback, identity/configuration mismatch, overload or clipping,
timeouts beyond the bound, discontinuity, non-finite samples, artifact-bound
violation, cancellation failure, or any condition outside the approved plan.

After construction, cleanup proceeds in reverse order: stop acquisition,
deactivate and close the stream, release the device, disable or isolate the
source as specified, disconnect in the approved safe order, terminate required
ports, and verify the final device and bench state. Unknown cleanup state fails
the diagnostic regardless of otherwise useful samples.

## Evidence and stop criteria

Retain the approved plan digest, source and receiver dossiers, manuals and
calibration-record identifiers, calculations, inventory, any required or
optional pre-connection measurements, photographs or diagrams as privacy
permits, UTC and monotonic
timing, source lock evidence, requested/effective SDR settings, stream and
cleanup results, artifact hashes, estimator/acceptance results, deviations, and
independent review.

Stop after the first bounded diagnostic and offline suitability analysis. Do
not proceed automatically to a second receiver, another frequency, profile
generation, or end-to-end calibration. Each expansion requires a new reviewed
plan and authorization.

## Conditions for changing readiness

This package is reference-suitable only for the retained five-second contract.
Any longer duration, different output, topology, receiver, setting, reference
condition, or uncertainty claim requires a new reviewed plan and evidence.
Electrical readiness may rely on a conservative specification-based calculation
under decision 0026. RC-01, RC-03, profile generation, and every broader support
claim remain separately gated.
