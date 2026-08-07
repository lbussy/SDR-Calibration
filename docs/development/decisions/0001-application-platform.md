# 0001 — Application platform

Status: Accepted

Date: 2026-08-07

## Decision

Implement the application in C++20 with CMake. Use SoapySDR as the SDR support
boundary and Qt 6 Widgets as the cross-platform GUI. Provide a headless CLI over
the same application services.

Target Windows, macOS, and Linux. Use Raspberry Pi OS as a project-qualified
Linux test fixture rather than assuming it inherits another platform's support.

## Consequences

The calibration core remains independent of Qt and SoapySDR. Platform and
driver differences are isolated behind explicit boundaries. GUI and CLI must
not develop separate calibration semantics.
