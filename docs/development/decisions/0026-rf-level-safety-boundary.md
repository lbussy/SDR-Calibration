# 0026 — RF level is an operator safety prerequisite

Status: Accepted

Date: 2026-08-20

Supersedes in part the mandatory power-measurement requirements in the
[GPSDO/reference-path readiness package](../gpsdo-reference-path-readiness.md).

## Decision

RF input power is a hardware-safety and observation-quality prerequisite, not
an accuracy-bearing calibration reference. The frequency reference's stated
frequency, lock state, stability, provenance, validity, and uncertainty inform
the calibration result. RF power-meter accuracy does not enter the frequency-
calibration uncertainty budget.

Before a conducted reference observation, the operator must establish that the
source, path, and receiver are impedance- and connector-compatible and that the
calculated maximum receiver input remains below the receiver manufacturer's
published limit with a conservative engineering margin. The calculation uses
the source's published output specification or a justified conservative bound
and the path's specified or conservatively bounded loss. The operator remains
responsible for selecting attenuation appropriate for the exact SDR and source.

An independent RF power measurement is optional when the source and path have
adequate published specifications or conservative bounds and the calculation
retains a conservative margin. Measurement becomes required only when the
available information cannot establish safety, the calculated margin is small,
damage or heating is plausible, or overload or compression is suspected. A
laboratory-grade or traceably calibrated power meter is not a general
application requirement.

Observation acceptance must still reject digital clipping and other existing
signal-quality failures. Absence of digital clipping does not prove absence of
analog compression. When compression is suspected, the operator should add
attenuation and repeat the observation; a materially changing frequency result
fails suitability until resolved.

## Consequences

Reference-path evidence retains the source power specification, receiver limit,
nominal and worst-case path calculation, impedance and connector compatibility,
selected attenuation, and operator confirmation. It need not contain measured
RF power, a characterized attenuator, power-meter calibration, or RF-power
uncertainty unless the conditions above make measurement necessary.

RF power evidence must not be used to raise reference authority or reduce the
frequency-reference uncertainty. Conversely, lack of laboratory RF-power
metrology must not block an otherwise safe and suitable reference observation.
