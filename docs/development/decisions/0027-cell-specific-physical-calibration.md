# 0027 — Cell-specific physical calibration qualification

Status: Superseded for release gating by
[decision 0028](0028-user-directed-calibration-workflow.md); retained for
optional positive physical-cell claims

Date: 2026-08-20

## Decision

This decision defines how an optional positive physical-device claim is
bounded. It does not require the project owner's hardware to obtain such a
claim before the product can enable user-directed calibration. Decision 0028
now governs the release-enabling workflow evidence.

Physical end-to-end calibration qualification is recorded as a matrix of exact
device, effective-configuration, reference-path, platform, and observation-domain
cells. One accepted cell does not qualify another cell or the product generally.

RC-01 remains a separate matched-receiver comparison gate. It is required before
claiming receiver equivalence, cross-device support, SDRplay RSP1B support based
on Airspy evidence, or general receiver support. An exact Airspy-only RC-03 cell
may be attempted while RC-01 is deferred, provided every other RC-03 contract is
satisfied. That attempt does not complete RC-01 or any other optional RC-03
matrix cell.

The production workflow continues to require its implemented linear frequency
model: at least two accepted, independently identified observations with nonzero
indicated-frequency span. Repeated observations at one nominal frequency may
support repeatability evidence but do not create a linear-model domain. Adopting
the core's local-constant model in the production workflow would require a
separate implementation and profile-semantics decision with deterministic tests.

Physical reference conditions that the live workflow cannot establish must not
be rewritten as `none`. A recorded-input RC-03 attempt may use retained physical
captures only when each observation is bound to separately reviewed condition
evidence, exact artifact hashes, and an authenticated, unexpired reference
registry. The run request's condition assertion is not sufficient evidence by
itself.

An `ad_hoc` reference remains limited to reliability quotient 50. A resulting
native profile is therefore `informational_only` under the current
qualification-capable threshold of 90. It may be retained as physical
calibration evidence for its exact cell, but it cannot be described as
traceable, qualification-capable, generally supported, or suitable outside its
observation-bounded domain. WSJT-X export is excluded from this cell because it
would discard material native-profile limitations.

## Consequences

- The existing accepted five-second 10 MHz LBE-1421/Airspy artifact may supply
  one recorded observation only after its private bytes, size, digest, privacy,
  and condition evidence are reverified.
- A positive Airspy linear-model cell needs at least one additional independently
  qualified observation at a different reference frequency. That frequency is
  a new RC-02 scope and requires its own immutable hardware plan.
- A different SDR is not required for the Airspy-only cell. The RSP1B and
  splitter remain required for RC-01 and broader receiver-comparison claims.
- A valid signed registry and independent local trust pin must exist before a
  production attempt. Signatures, conditions, span, uncertainty components, or
  assurance scores must not be fabricated to make the workflow pass.
- An RC-03 physical cell remains `Not qualified` until it completes acquisition or
  verified recorded ingestion through profile validation, in-domain evaluation,
  evidence publication, privacy and integrity review, and cleanup evidence.
