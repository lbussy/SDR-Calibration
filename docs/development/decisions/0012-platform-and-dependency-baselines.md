# 0012 — Platform and dependency baselines

Status: Accepted; platform-target scope superseded by
[0019](0019-initial-platform-target-boundary.md)

Date: 2026-08-07

## Decision

The original scope was Windows, macOS, and Linux with Raspberry Pi OS as a
separately qualified Linux fixture. Decision 0019 replaces that scope with exact
operating-system, architecture, and interface targets. Continue to select exact
Qt, SoapySDR, compiler, and CMake versions through documented platform probes
before a production target is declared supported.

Pin or otherwise reproduce dependency versions for releases. A new dependency
requires ownership, license, transitive-dependency, packaging, and platform
review.

## Consequences

The current scaffold expresses targets, not a supported platform matrix. A
successful build on one machine cannot establish general support.
