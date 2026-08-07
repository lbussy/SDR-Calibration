# 0005 — Scope and claim levels

Status: Accepted

Date: 2026-08-07

## Decision

Version 1 calibrates absolute frequency indication for an identified SDR under
documented conditions. Amplitude, phase, absolute time, antennas, feed lines,
transmitters, modulation quality, and regulatory certification are outside the
initial scope.

Results use three claim levels:

- `informational_only` — useful evidence exists, but one or more calibration gates
  are incomplete.
- `calibrated` — the profile has a complete model, stated uncertainty, and a
  validated domain.
- `qualification_capable` — the calibrated result also meets the consuming
  application's uncertainty and assurance requirements.

The application is a measurement tool, not an accredited calibration service.
It must not use `certified`, `traceable`, or a fixed accuracy claim unless the
specific result satisfies the documented meaning of that term.

## Consequences

Accuracy claims attach to a particular evaluated result, not to the application
or SDR model generally. Source builds, tests, and profile creation alone do not
establish qualification-capable output.
