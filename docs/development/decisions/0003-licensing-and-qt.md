# 0003 — Licensing and Qt

Status: Accepted

Date: 2026-08-07

## Decision

License original project source and documentation under the MIT License unless
the project owner explicitly chooses otherwise.

Use dynamically linked Qt 6 modules available under suitable open-source terms.
The initial set is Core, Widgets, Network, Concurrent, and OpenGLWidgets only
when justified. Distributed components retain their own licenses and trigger a
release compliance gate.

## Consequences

The MIT License does not relicense Qt or other dependencies. New Qt modules and
all other dependencies require license and packaging review. GPL-only Qt modules
are excluded absent an explicit project-level decision.
