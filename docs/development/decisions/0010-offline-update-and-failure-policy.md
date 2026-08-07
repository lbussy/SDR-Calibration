# 0010 — Offline update and failure policy

Status: Accepted

Date: 2026-08-07

## Decision

The application has no telemetry or account requirement. It remains usable
offline with a valid cached reference registry and profile set.

Updates require TLS transport, signature, digest, schema, version, and semantic
validation before atomic activation. A failed update retains the last-known-good
artifact. Expired evidence cannot silently remain qualification-capable.

Cancellation, timeout, driver exception, device removal, or interrupted writes
must release resources, preserve the previous known-good artifact, and report
whether device state is known or unknown.

## Consequences

Network availability is not a prerequisite for ordinary work, but freshness is
visible and enforced. Partial downloads and invalid updates never replace valid
local state.
