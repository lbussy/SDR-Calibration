# Uncertainty model

## Reporting

Store standard uncertainty components and their correlation assumptions. Report
expanded uncertainty with its coverage factor. `k = 2` is the normal default,
but an approximate confidence level is stated only when justified.

## Candidate components

- transmitted reference uncertainty;
- propagation and received-signal uncertainty;
- carrier-estimator uncertainty;
- sample-clock short-term stability;
- accepted-observation repeatability;
- model-fit and interpolation uncertainty;
- temperature and warm-up variation;
- tuner and clock-path repeatability;
- numerical and serialization effects.

Excluded or unknown components remain explicit. Unknown material components
prevent a qualification-capable result.

## Acceptance boundary

A consumer supplies its maximum acceptable expanded uncertainty and minimum
reliability quotient. A result must satisfy both independently.

No application-wide accuracy is inferred from one profile, one device, one
frequency, or one environmental condition.
