# Failure-state model

## Principle

The application fails closed when it cannot establish identity, configuration,
reference evidence, observation suitability, model domain, uncertainty,
integrity, or validity.

## State categories

- `usable` — all requirements for the requested claim level pass.
- `reduced_assurance` — calculation is usable with a lower reliability quotient.
- `informational_only` — evidence is useful but calibration gates are incomplete.
- `rejected` — the calculation would be misleading or outside contract.
- `state_unknown` — interruption prevents confirmation of device or artifact state.

## Required behavior

- Never replace the last-known-good profile until validation and atomic write
  complete.
- Never substitute another reference without recording the selection and its
  assurance effect.
- Never extrapolate or downgrade a claim silently.
- Preserve bounded evidence and the first causal failure separately from
  recovery activity.
- Release streams and device ownership on every normal or exceptional exit.
- Treat cancellation as an explicit result rather than success or generic error.
