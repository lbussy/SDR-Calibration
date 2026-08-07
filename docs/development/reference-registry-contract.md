# Reference registry contract

## Purpose

The registry describes candidate references and their current evidence. It does
not require every reference to be an authoritative broadcast.

Supported reference kinds are:

- `authority_confirmed`
- `derived_traceable`
- `locally_characterized`
- `ad_hoc`
- `unknown`

Each record carries its assurance score ceiling, nominal or certified frequency,
frequency uncertainty when known, operating status, evidence sources, retrieval
and expiration times, location or connection method, and applicable propagation
or observation limitations.

The class-to-ceiling mapping is normative. Schema validation checks the mapping;
profile semantic validation also checks that no calculated reliability quotient
exceeds the applicable ceiling.

## Local and ad-hoc records

A local record can be private to one installation. Its evidence may identify a
signal generator, GPS-disciplined oscillator, reference output, injected test
signal, or other characterized source. Secrets and credentials are forbidden.

An ad-hoc source can be used even when its absolute traceability is unknown. The
result remains bounded by the ad-hoc assurance ceiling and its measured
uncertainty and repeatability.

## Updates

Registry updates are signed, hashed, schema-validated, versioned, and activated
atomically. Local overlays remain separate from the signed global registry so a
global update cannot erase operator-controlled references.

Conflicts, expiration, and missing sources fail visibly. Source absence does not
prove a transmitter has ceased operation.
