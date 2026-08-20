# GPSDO and reference-path readiness execution prompt

```{note}
This is a historical execution prompt. Decision
[0026](decisions/0026-rf-level-safety-boundary.md) supersedes any implication
that independent RF power measurement or laboratory power metrology is a
general reference-suitability requirement.
```

## Objective

Prepare a complete, reviewable, documentation-only readiness package for a
future GPS-disciplined reference connection. Define the information, safety
calculations, topology decision, evidence, bounded execution-plan fields, abort
conditions, cleanup, and approval gates required before connecting the source
to an SDR.

## Verified starting point

- Airspy HF+ Discovery serial `2f52ff5de72635ba` and SDRplay RSP1B serial
  `2404058C60` are qualified only for their retained bounded captures.
- Their 10 MHz antenna samples were rejected by the unchanged production
  estimator and do not establish WWV suitability or calibration.
- A GPSDO has been ordered, but its exact model, received condition, outputs,
  level, impedance, lock behavior, traceability, and suitability have not been
  established in this repository.
- No approved physical connection plan exists. End-to-end calibration remains
  paused.

## Required work

1. Define the exact source-identification, output, traceability, environmental,
   warm-up, lock, and uncertainty evidence required after receipt.
2. Require an explicit choice between using a supported receiver reference-
   clock input and injecting an attenuated signal into an RF input. Never treat
   those ports or electrical contracts as interchangeable.
3. Define receiver-specific evidence for the exact input port, impedance,
   absolute maximum level, recommended test level, DC tolerance, protection,
   bandwidth, and any driver-visible clock/configuration state.
4. Provide reviewable level-conversion and worst-case attenuation calculations
   without supplying values that have not been verified.
5. Inventory every splitter, attenuator, DC block, adapter, cable, terminator,
   and measurement instrument, including ratings, losses, uncertainties, and
   connector/impedance compatibility.
6. Define pre-connection, connection, bounded receive, abort, cleanup, and
   independent evidence-review gates. Leave exact commands and settings blank
   until the equipment and one bounded test are separately approved.
7. State the current readiness decision and the evidence needed to change it.

## Non-goals and safety boundary

- Do not connect, power, configure, enumerate, or stream from any hardware.
- Do not infer electrical suitability from an order listing, connector shape,
  nominal frequency, product family, or unverified web description.
- Do not prescribe attenuation, signal level, splitter topology, clock mode,
  capture settings, or commands before exact source and receiver evidence is
  available.
- Do not claim traceability, reference class, uncertainty, calibration
  accuracy, device support, or end-to-end qualification.
- Do not change code, schemas, dependencies, estimator thresholds, or existing
  capture and calibration claims.

## Validation and adversarial review

Build the Sphinx documentation with warnings as errors and run
`git diff --check`. Review the package for clock/RF-port confusion, peak/RMS or
loaded/unloaded voltage confusion, impedance mismatch, DC hazards, unsafe
maximum-level interpretation, missing tolerances, unverified traceability,
splitter back-feed, absent abort/cleanup rules, premature commands, and
qualification overclaim. Correct every actionable finding and repeat affected
checks.

## Exit criteria

The repository contains a complete readiness worksheet and bounded future-plan
template; every unknown remains visibly blocking; connection readiness remains
fail closed until exact evidence is supplied and reviewed; and no hardware or
external service has been accessed.
