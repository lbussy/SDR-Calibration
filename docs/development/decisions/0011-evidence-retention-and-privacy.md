# 0011 — Evidence retention and privacy

Status: Accepted

Date: 2026-08-07

## Decision

Retain bounded reduced evidence by default. Raw IQ retention is explicit,
time-limited, byte-limited, and separately identified because it can contain
unrelated communications.

Evidence records UTC timestamps, monotonic durations, source identity, byte and
time bounds, truncation, partial failures, and atomic-write status. It excludes
credentials, private keys, tokens, and unnecessary personal or device data.

## Consequences

Evidence bundles are reviewable before sharing. Deep or expanded diagnostics
are opt-in and cannot alter acquisition timing or recovery behavior.
