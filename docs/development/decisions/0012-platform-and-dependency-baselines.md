# 0012 — Platform and dependency baselines

Status: Accepted

Date: 2026-08-07

## Decision

Support Windows, macOS, and Linux with Raspberry Pi OS as a project-qualified
Linux fixture. Select exact minimum operating-system, architecture, Qt,
SoapySDR, compiler, and CMake versions through a documented platform probe
before the first production target is declared supported.

Pin or otherwise reproduce dependency versions for releases. A new dependency
requires ownership, license, transitive-dependency, packaging, and platform
review.

## Consequences

The current scaffold expresses targets, not a supported platform matrix. A
successful build on one machine cannot establish general support.
