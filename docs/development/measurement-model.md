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

## Phase 7 implementation

The hardware-free core implements `frequency-error-model-v1` with a local
constant correction from one accepted observation and an ordinary least-squares
linear model from at least two accepted, independently identified observations
with nonzero indicated-frequency span. The linear form is:

```text
indicated_error_hz
    = intercept_error_hz
    + slope_ppm * (indicated_frequency_hz - reference_frequency_hz) / 1,000,000
```

The linear model uses the observed range midpoint as its reference. Its validity
domain is the inclusive minimum-to-maximum indicated-frequency range of the
fitted observations; a local correction is valid only at its observation's
frequency. The core has no extrapolation option. Inputs must explicitly carry
successful observation-acceptance provenance and an independence identifier.
The API does not establish that those assertions are truthful; application
services and retained evidence remain planned.

For more than two observations the model records residual standard uncertainty
and its residual degrees of freedom. A two-point line marks residual uncertainty
unavailable rather than presenting zero as an estimate. A present residual
value is only one input to a full uncertainty budget.

`reliability-quotient-v1` requires exactly these named assurance components:
reference provenance, received-signal suitability, device binding,
environmental validity, observation quality, model quality, artifact integrity,
and evidence completeness. Each integer score is from 0 through 100. The
quotient is their minimum further limited by the reference-class ceiling in
decision 0006. Incomplete or unknown-version inputs are rejected rather than
scored.

## Time semantics

Use UTC for evidence timestamps and a monotonic clock for durations, warm-up,
timeouts, and sample continuity. Wall-clock synchronization does not calibrate
the SDR sample oscillator.
