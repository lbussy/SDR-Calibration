# 0004 — Repository and documentation layout

Status: Accepted

Date: 2026-08-07

## Decision

Keep all project-owned C++ source and headers under `src/`. Keep machine schemas
under `schemas/`, tests under `tests/`, and Sphinx/MyST documentation under
`docs/` for Read the Docs-compatible rendering.

User-facing documentation and development contracts share the documentation
site but remain in separate sections.

## Consequences

Repository root files carry only project-wide build, contribution, licensing,
formatting, and hosting policy. Empty source directories are introduced with
their first substantive implementation rather than placeholder code.
