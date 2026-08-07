# 0007 — Observation and uncertainty policy

Status: Accepted

Date: 2026-08-07

## Decision

Observation acceptance is deterministic and records duration, estimator
confidence, clipping, missing samples, discontinuities, drift, interference,
repeatability, and rejection reasons.

Internally retain standard uncertainty components. Report expanded uncertainty
with an explicit coverage factor, normally `k = 2`. Describe an approximate
confidence level only when the statistical assumptions justify it.

Uncertainty includes applicable reference, propagation, estimator, sample-clock,
fit, temperature, warm-up, repeatability, interpolation, and numerical
components. Correlated observations must not be treated as independent merely
because they occur in separate captures.

## Consequences

Fit residual and displayed precision cannot stand in for uncertainty. Operator
overrides remain visible and can reduce the reliability quotient or prevent a
qualification-capable result.
