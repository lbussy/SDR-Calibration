# 0008 — Profile binding and validity

Status: Accepted

Date: 2026-08-07

## Decision

Bind a profile to the strongest available physical device identity and all
effective settings shown to affect frequency interpretation. Preserve the full
discovery and configuration snapshot as evidence while distinguishing binding
from informational fields.

Profile validity ends at the earliest applicable calendar expiration,
configuration mismatch, environmental violation, clock-source change,
dependency or firmware invalidation, failed verification measurement, or
explicit revocation.

Profile segments are interpolation domains bounded by accepted observations.
Extrapolation is rejected by default. A single reference can support only a
local correction unless a stronger model is independently justified.

## Consequences

A quick verification can confirm continued applicability but is not silently
treated as a full recalibration. Weak identity, unverified effective settings,
or use outside the domain lowers the claim level or fails closed.
