# 0001 — Application platform

Status: Accepted; platform-target scope superseded by
[0019](0019-initial-platform-target-boundary.md)

Date: 2026-08-07

## Decision

Implement the application in C++20 with CMake. Use SoapySDR as the SDR support
boundary and Qt 6 Widgets as the cross-platform GUI. Provide a headless CLI over
the same application services.

The original target statement was Windows, macOS, and Linux, with Raspberry Pi
OS treated as a separately qualified Linux fixture. Decision 0019 replaces that
broad statement with exact operating-system, architecture, and interface targets.

## Consequences

The calibration core remains independent of Qt and SoapySDR. Platform and
driver differences are isolated behind explicit boundaries. GUI and CLI must
not develop separate calibration semantics.
