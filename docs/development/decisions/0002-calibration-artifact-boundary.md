# 0002 — Calibration artifact boundary

Status: Accepted

Date: 2026-08-07

## Decision

The native SDR Calibration Profile is the authoritative, semi-durable artifact
for one identified SDR configuration. It contains the model, validated domain,
uncertainty, validity, identity, and provenance.

`target_frequency_hz` belongs to a record that applies the profile, not to the
profile itself. WSJT-X settings and `fmt.all` are lossy interoperability
adapters and cannot replace the native artifact.

## Consequences

Consumers can evaluate one profile at multiple target frequencies. Imports that
lack identity, validity, uncertainty, or provenance remain informational until
those requirements are independently established.
