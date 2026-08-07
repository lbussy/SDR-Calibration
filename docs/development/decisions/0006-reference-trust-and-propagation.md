# 0006 — Reference trust, local references, and propagation

Status: Accepted

Date: 2026-08-07

## Decision

Allow authoritative broadcasts, independently traceable local references,
locally characterized references, and ad-hoc references. Every observation must
identify its reference class and evidence.

Reference classes impose the following maximum reference-assurance score:

| Class | Reference evidence | Score ceiling |
| --- | --- | ---: |
| `authority_confirmed` | Current national-authority or equivalent evidence | 100 |
| `derived_traceable` | Documented local chain with current independent verification | 90 |
| `locally_characterized` | Local source characterized against an accepted reference or profile | 75 |
| `ad_hoc` | Nominal or repeatable source without an independently established chain | 50 |
| `unknown` | Missing or contradictory provenance | 0 |

The overall reliability quotient cannot exceed the weakest applicable assurance
component. It therefore cannot exceed the reference score ceiling. The quotient
is an assurance index from 0 through 100, not a probability, confidence level,
or replacement for uncertainty.

Transmitted-frequency uncertainty and received-frequency uncertainty are
separate. Propagation, multipath, modulation, interference, and the observation
method contribute independently to the uncertainty and assurance assessment.

## Consequences

Ad-hoc and locally generated references remain useful for comparison,
development, repeatability checks, and bounded calibration. Their results are
not presented with the same reliability as a result supported by stronger
provenance unless an independent chain elevates their class.

Consumer policy determines its minimum acceptable quotient. The default
qualification-capable threshold is 90; consumers may require a higher value.
