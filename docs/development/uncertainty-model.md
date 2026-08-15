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

## Phase 7 implementation

`uncertainty-budget-v1` accepts named standard-uncertainty components in hertz,
finite sensitivity coefficients, and optional pairwise correlations. It sums
the component variances and covariance terms, takes the square root for combined
standard uncertainty, and multiplies by the explicit positive coverage factor
for expanded uncertainty.

Correlation pairs must be unique, refer to distinct present components, have
coefficients from -1 through 1, and together form a positive-semidefinite
correlation matrix. Invalid or non-finite budgets fail closed. The result also
states whether the caller listed excluded or unknown material components. A
calculated budget with such a listing is incomplete and cannot, by itself,
support a qualification-capable result.

The calculator does not infer components, distributions, confidence levels,
materiality, correlation, or fitness for a consumer's use. Reliability quotient
is calculated separately and never changes the numerical uncertainty.
