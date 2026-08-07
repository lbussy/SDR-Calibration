# Measurement model

## Scope

The initial measurement is absolute frequency indication:

```text
indicated_error_hz = indicated_frequency_hz - estimated_true_frequency_hz
```

The profile models indicated error within one or more validated interpolation
domains. A consumer subtracts the evaluated indicated error from an indicated
frequency to estimate true frequency.

## Claim levels

The application produces `informational_only`, `calibrated`, or
`qualification_capable` results as defined by decision 0005. It never promotes a
result solely because a model fit completed.

## Reference assurance

Each observation records one class from decision 0006. Local and ad-hoc
references are accepted, but their score ceiling limits the reliability quotient.

The quotient is calculated conservatively:

```text
reliability_quotient = minimum(applicable_assurance_component_scores)
```

Initial components are reference provenance, received-signal suitability,
device binding, environmental validity, observation quality, model quality,
artifact integrity, and evidence completeness. Component scoring rules must be
versioned and tested before the quotient is emitted by production software.

The quotient is not statistical confidence and does not alter the numerical
uncertainty. Both values are reported.

## Frequency domains

No default extrapolation is permitted. A single accepted reference supports a
local correction; a slope or more complex model requires sufficient accepted
frequency span and independent observations. Tuner or clock-path discontinuities
create separate segments when evidence shows they affect behavior.

## Time semantics

Use UTC for evidence timestamps and a monotonic clock for durations, warm-up,
timeouts, and sample continuity. Wall-clock synchronization does not calibrate
the SDR sample oscillator.
