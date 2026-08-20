# LBE-1420/Airspy reference diagnostic evidence prompt

## Objective

Preserve a privacy-reviewed, documentation-only record of the one completed
LBE-1420-to-Airspy HF+ Discovery diagnostic. Reconcile its transport and signal
evidence with the GPSDO readiness package without converting an explicitly
waived level measurement into an electrical-safety, traceability, uncertainty,
calibration, supported-device, or release claim.

## Verified input

- Execution date: 2026-08-20.
- Authorized override-plan SHA-256:
  `2d9d24743810c56944d5c87917979282799254148daa28754098e49c34a449ba`.
- Source: one exact Leo Bodnar LBE-1420, with its serial retained privately.
- Operator-reported setting: 10 MHz, Low Power, nominal +6 dBm, valid GNSS
  lock.
- Path: 6-inch patch, LBE-1710 10 MHz low-pass filter/DC block, 12-inch
  patch, generic nominal -10 dB DC-1 GHz attenuator, Airspy HF+ Discovery RF
  input.
- Receiver: Airspy HF+ Discovery serial `2f52ff5de72635ba`.
- The operator explicitly waived independent preconnection level measurement.
- The generic attenuator's actual loss, tolerance, return loss, and calibration
  remain unknown.
- Capture and offline-analysis artifact digests are retained; raw IQ and the
  source serial remain outside Git.

## Required work

1. Add a concise diagnostic record containing the exact frozen topology,
   nominal-only level calculation, requested/effective settings, transport,
   cleanup, hashes, production-estimator result, `signal-quality-v1` result,
   and final suitability classification.
2. Record manufacturer-document facts separately from operator observations
   and measured facts. Do not report the nominal level calculation as a
   measurement or uncertainty result.
3. Preserve the source serial, raw IQ, private host details, and private
   artifact paths outside Git. Retain content hashes as independently
   identifiable private-evidence locators.
4. Reconcile the GPSDO readiness package and project status. Keep the path
   `NOT READY TO CONNECT` under the durable readiness contract because the
   waived level measurement and uncharacterized attenuator leave Gates 4-6
   incomplete.
5. Keep RC-01, RC-02, and RC-03 separate. The single diagnostic does not
   compare receivers, complete reference suitability, or perform calibration.
6. Identify the next unblock conditions without authorizing another physical
   operation.

## Claim boundary

The record may state only that the exact run completed bounded receive
transport and that the immutable production estimator and analyzer accepted
one coherent 10 MHz signal under the recorded settings. Zero digital-rail
clipping does not prove analog linearity or absence of front-end overload.

Do not claim electrical safety, verified input level, reference traceability,
measurement uncertainty, GNSS authority beyond the retained documentation,
calibration accuracy, a native profile, Airspy support, RC-01, RC-02, RC-03, or
release readiness.

## Validation and review

Run `git diff --check` and Sphinx with warnings as errors. Adversarially review
for private-data leakage, nominal-versus-measured confusion, maximum-input
misuse, hidden analog overload, unverified lock/holdover/traceability,
attenuator uncertainty, missing cleanup evidence, and qualification overclaim.
Correct every actionable finding and repeat affected validation.

## Exit criteria

The public repository contains a truthful, privacy-reviewed summary bound to
the exact plan and artifact hashes; readiness and RC gates remain fail closed;
no raw IQ or source serial is committed; no hardware is accessed; and complete
attributable changes are committed and pushed only after staged-diff review.
