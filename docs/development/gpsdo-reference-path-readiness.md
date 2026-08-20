# GPSDO and reference-path readiness package

## Current decision

**Readiness status: NOT READY TO CONNECT.**

This package is complete as an offline planning artifact. The physical path is
not ready because loaded source/path level, attenuation tolerance, traceability,
uncertainty, and complete source conditions have not been verified. One exact
unmeasured-path diagnostic has now completed under an explicit operator waiver;
its [record](lbe1421-airspy-reference-diagnostic.md) establishes only bounded
transport and coherent-signal suitability for that run. Possession, delivery,
lock indication, a nominal 10 MHz label, or a clean digital sample does not
close the remaining gates.

No step in this document authorizes powering equipment, connecting cables,
enumerating an SDR, starting a stream, or performing a receive test.

## Intended claim boundary

The first authorized physical activity, when separately approved, will be a
bounded receive-only reference-path diagnostic. It may establish safe transport
and signal suitability for one exact path. It will not by itself establish
reference traceability, observation independence, calibration accuracy, a
native profile, or end-to-end qualification.

Candidate receivers currently known to the project are:

| Receiver | Current evidence | Reference-path status |
| --- | --- | --- |
| Airspy HF+ Discovery serial `2f52ff5de72635ba` | Exact recorded macOS combination is bounded-capture-qualified; one waived-measurement LBE-1421 path produced an accepted coherent signal | Transport suitable for the exact diagnostic; electrical path and reference suitability not ready |
| SDRplay RSP1B serial `2404058C60` | Exact recorded macOS combination is bounded-capture-qualified | Input topology and limits unverified; not ready |

## Completed diagnostic with open gates

The 2026-08-20 LBE-1421/Airspy diagnostic used the attenuated RF-input topology,
an LBE-1710 filter/DC block, and a generic nominal -10 dB attenuator. Its exact
digest-bound override, transport record, cleanup evidence, private artifact
hashes, and production analysis are retained in the
[diagnostic record](lbe1421-airspy-reference-diagnostic.md).

The operator waived the independent preconnection level measurement. The
attenuator's actual loss and tolerance remain unknown, and no unit-specific
traceability or uncertainty record was established. Gates 4 through 6 therefore
remain incomplete and the durable decision remains `NOT READY TO CONNECT`.

## Gate 1: exact source dossier

Complete every field from retained manufacturer documentation, calibration
records, or direct measurement. Record document titles, revisions, retrieval
dates, and page or section references.

| Field | Required evidence | Current value |
| --- | --- | --- |
| Manufacturer, exact model, serial | Nameplate plus authoritative manual | Leo Bodnar LBE-1421; exact serial retained privately; exact USB identity observed 2026-08-20 |
| Output port and connector | Labeled port and manual | Two SMA-female outputs; output 1 supports 1 Hz-800 MHz or 1 PPS and output 2 supports 1 Hz-1.4 GHz; exact output used in the RF diagnostic was not retained |
| Nominal frequency and available modes | Manual; exact selected mode later verified | USB-only observation displayed both outputs enabled at 10 MHz and Low Power; earlier RF-run output-port identity remains unknown |
| Waveform and harmonic specification | Manual or measured record | LBE-1421 datasheet: CMOS square wave; LBE-1710 page: greater than 65 dB significant-harmonic suppression; not independently measured |
| Output level | Minimum, nominal, and maximum with units | +6 dBm nominal Low Power below 400 MHz; tolerance and received-unit extrema unverified |
| Level convention | dBm, dBV, Vrms, Vpp, open-circuit, or specified load | Datasheet specifies 1.65 V into 50 ohms and +6 dBm Low Power; exact power convention/tolerance not fully stated |
| Source impedance and required load | Manual | 50-ohm source; assembled loaded output not measured |
| DC content or bias | Manual and measurement method | LBE-1710 manufacturer page states DC removal; assembled-path DC not independently measured |
| Warm-up requirement | Manual and planned monotonic timer | Unverified |
| Lock indication and lock-loss behavior | Manual plus observable indication | Operator confirmed a valid lock indication for the RF diagnostic; later application reported `Device Ok` but exposed no distinct GPS-lock or PLL-lock field; semantics remain incomplete |
| Holdover behavior | Manual; state whether holdover is permitted | Manufacturer describes seamless temporary-loss holdover/reacquisition; application and passive NMEA observation did not expose or distinguish holdover |
| Environmental limits | Manual and planned observation method | Unverified |
| Power supply and grounding | Manual and bench arrangement | Datasheet specifies USB-C, 5 V ±10%, 250 mA ±10%; USB reported 250 mA; exact bench supply/grounding record incomplete |
| Traceability chain | Current calibration/specification evidence | Unverified |
| Frequency uncertainty | Value, coverage, conditions, validity interval | Unverified |

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

Retain the source-level tolerance, component-loss tolerance, instrument
uncertainty, mismatch effects, and engineering margin separately. Independently
show that the highest possible receiver level remains below the documented
absolute maximum. A calculator result cannot replace a measured pre-connection
level at the intended load.

## Gate 5: path inventory

| Item | Required retained information |
| --- | --- |
| Fixed or step attenuator | Asset ID, nominal attenuation, tolerance, frequency range, impedance, power rating |
| Splitter or divider | Asset ID, topology, port isolation, insertion loss, impedance, frequency and power ratings |
| DC block | Asset ID, voltage rating, frequency range, insertion loss, impedance |
| Terminators | Asset ID, impedance, power rating, frequency range |
| Cables and adapters | Endpoints, impedance, length, loss or bound, frequency and power ratings |
| Switch or relay, if any | Isolation, unterminated-state behavior, sequencing, ratings |
| Measurement instrument | Model, serial, calibration status, uncertainty, probe/load configuration |

Every unused splitter port must have a reviewed disposition. Do not combine an
antenna and local source without a separately reviewed combiner/isolation plan.
Prevent receiver-to-receiver and source-to-antenna back-feed. Connector adapters
do not correct an impedance or level mismatch.

## Gate 6: pre-connection review

The later reviewer must answer every item positively:

- Exact source, receiver, port, and component identities are recorded.
- The selected topology is supported by authoritative documentation.
- Units, load convention, impedances, and DC behavior are unambiguous.
- Worst-case calculations include tolerances, uncertainty, and margin.
- The assembled path has been checked into the intended load before an SDR is
  connected.
- GPS lock, warm-up, environment, and holdover policy are observable.
- The operator owns the hardware and test interval; conflicting users and
  processes are excluded.
- Requested and required effective SDR settings are frozen.
- Duration, sample, memory, and artifact bounds are explicit.
- Abort, cleanup, post-run enumeration, and evidence paths are explicit.
- The exact plan text has an immutable SHA-256 and separate authorization.

Any negative or unknown answer keeps the path `NOT READY TO CONNECT`.

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
Verified source level and uncertainty at intended load:
Path components, losses, tolerances, and calculated receiver level:
Receiver safe limit, chosen operating level, and margin:
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
calibration-record identifiers, calculations, inventory, pre-connection
measurements, photographs or diagrams as privacy permits, UTC and monotonic
timing, source lock evidence, requested/effective SDR settings, stream and
cleanup results, artifact hashes, estimator/acceptance results, deviations, and
independent review.

Stop after the first bounded diagnostic and offline suitability analysis. Do
not proceed automatically to a second receiver, another frequency, profile
generation, or end-to-end calibration. Each expansion requires a new reviewed
plan and authorization.

## Conditions for changing readiness

Change this package from `NOT READY TO CONNECT` only after every gate is
completed with retained evidence, no contradiction remains, and the exact
bounded plan receives separate approval. A ready connection plan would still
leave physical execution and every resulting qualification claim separately
gated.
