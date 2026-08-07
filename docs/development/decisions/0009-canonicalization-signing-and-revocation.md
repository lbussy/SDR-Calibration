# 0009 — Canonicalization, signing, and revocation

Status: Accepted

Date: 2026-08-07

## Decision

Use RFC 8785 JSON Canonicalization Scheme payloads for profile hashing and
signing. Exclude the top-level `integrity` member from that payload.

Require SHA-256 integrity for every published profile. Support detached Ed25519
signatures, key identifiers, rotation, and revocation. An unsigned profile can
be calibrated when all scientific gates pass, but signature status contributes
to assurance and a consumer may require a verified signature.

## Consequences

An invalid signature is a hard failure. A missing signature is explicit and is
never represented as verified. Canonical numeric representation and negative
zero behavior require conformance fixtures before signing is implemented.
