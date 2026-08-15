# Raspberry Pi CLI-only contract

Status: Accepted

Date: 2026-08-15

## 1. Purpose

This contract governs a CLI-only SDR Calibration distribution for Raspberry Pi
OS on 64-bit ARM. It reuses the existing `sdrcal` and, when separately enabled,
`sdrcal-capture` applications. It does not create a fork, a Raspberry Pi-specific
calibration engine, or a second profile format.

The first deliverable is the hardware-free recorded-input `sdrcal` application.
A SoapySDR-enabled capture deliverable is a later, separately reviewed and
qualified extension. Neither deliverable changes the native SDR Calibration
Profile, reference, uncertainty, evidence, integrity, or reliability-quotient
contracts.

## 2. Product variants

### 2.1 Recorded-input CLI

The baseline Raspberry Pi package shall contain `sdrcal` and the project
license, notices, schema, and exact-payload disposition material required by the
distribution contract. It shall be built with:

- `SDRCAL_BUILD_CLI=ON`;
- `SDRCAL_BUILD_GUI=OFF`;
- `SDRCAL_REQUIRE_GUI=OFF`;
- `SDRCAL_ENABLE_SOAPYSDR=OFF`; and
- no Qt dependency or desktop integration payload.

This variant processes explicitly selected, previously captured CF32LE
observations. It shall not enumerate, open, configure, or stream from an SDR.

### 2.2 Capture-enabled CLI

The optional capture variant may additionally contain `sdrcal-capture` and set
`SDRCAL_ENABLE_SOAPYSDR=ON`. It shall use only the existing SoapySDR boundary;
vendor APIs and Raspberry Pi device access must not enter the calibration core.

The capture variant shall be distinguishable in its package name, metadata,
dependencies, and operator documentation. Absence of SoapySDR or a requested
device module shall fail closed. The application shall not automatically fall
back to another device, driver, module, or enumeration index.

## 3. Platform baseline

The first platform fixture shall be one explicitly named Raspberry Pi model
running one exact 64-bit Raspberry Pi OS release and kernel. The retained record
shall include:

- board model and revision, architecture, installed RAM, and storage medium;
- OS image identity, package state, kernel, firmware, and boot configuration;
- compiler, C++ standard library, CMake, Ninja, and packaging-tool versions;
- source revision and complete CMake cache; and
- build, test, package, install, runtime, and cleanup timestamps in UTC.

Thirty-two-bit Raspberry Pi OS, other Debian derivatives, containers,
cross-compiled-only results, and other Pi models receive no support claim from
the first fixture. A native passing run is required; cross-compilation may
produce candidates but cannot qualify the platform.

## 4. Architecture and behavior

The Raspberry Pi applications shall call the same application services,
calibration core, profile engine, reference registry, evidence generator, and
WSJT-X adapter as other platforms. Platform conditionals may isolate operating
system integration but shall not alter scientific policy or artifact meaning.

CLI arguments, request validation, stable exit classifications, canonical JSON
terminal output, cancellation, atomic publication, and fail-closed behavior
shall remain aligned with the production CLI contract. The native profile
remains authoritative; WSJT-X output remains an explicitly lossy adapter.

No Qt target, library, plug-in, notice, source archive, desktop file, icon, or
display-server dependency may enter a CLI-only package.

## 5. Resource contract

Qualification shall define and retain the supported input-size, memory, storage,
runtime, temperature, and throttling envelope for the exact fixture. Tests shall
measure peak resident memory and temporary plus final disk use with
representative and maximum-supported observations.

The implementation shall reject work before publication when available
resources cannot satisfy a documented bound. It shall not rely on uncontrolled
swap growth, an out-of-memory termination, partial output, or filesystem
exhaustion as its resource policy.

The current recorded-input path's whole-observation byte buffer and decoded
sample storage require explicit review. Before a positive Raspberry Pi runtime
claim, the project shall do one of the following:

1. implement and scientifically validate bounded-memory processing with output
   equivalence to the shared workflow; or
2. enforce and document a smaller input bound that completes within the retained
   fixture's resource envelope without changing calibration semantics.

Any new streaming estimator or changed numerical reduction order requires
deterministic equivalence tests, uncertainty review, and adversarial review. A
mere successful small fixture does not qualify the existing maximum artifact
limits.

## 6. Build and hardware-free validation

The repository shall provide a reproducible Raspberry Pi CLI release preset or
an equivalently reviewed toolchain configuration. The recorded-input variant
must configure without Qt and SoapySDR installed.

The minimum native validation is:

```text
cmake --preset <raspberry-pi-cli-release>
cmake --build --preset <raspberry-pi-cli-release>
ctest --preset <raspberry-pi-cli-release>
sphinx-build -W -b html docs docs/_build/html
```

Validation shall additionally cover CLI help/version startup, positive and
negative recorded calibration, cancellation, malformed and oversized input,
digest mismatch, non-finite samples, existing output, publication failure,
filesystem exhaustion simulation, and cleanup. Tests must remain deterministic,
network-free, hardware-free, and safe to repeat.

New compiler and static-analysis warnings in changed code are defects. A build
or test pass establishes only the exact platform/build cell; it does not
establish packaging, installation, device, capture, or calibration accuracy.

## 7. Packaging and distribution

The first distributable may be an audited ARM64 archive. A production Raspberry
Pi OS package shall be a CLI-only DEB installed below `/usr`; it shall not reuse
the GUI-conditioned Ubuntu DEB target without separating its payload and
dependency assumptions.

Package construction shall:

- run without root and use deterministic root-owned archive metadata;
- derive runtime dependencies from the exact staged ARM64 ELF payload;
- reject build-path leakage, unsafe RPATH/RUNPATH, unresolved libraries,
  unexpected shared objects, and undeclared payloads;
- include the MIT license, third-party notices, an SPDX inventory, dependency
  evidence, and exact file hashes;
- retain source revision, package metadata, payload inventory, and package
  SHA-256; and
- keep the recorded-input package free of Qt, SoapySDR, vendor modules, and
  vendor runtimes.

For a capture package, SoapySDR core, each device module, and every vendor
runtime are separate inventory and license-disposition items. System packages
are preferred where practical. No dependency may be added or distributed until
the licensing and distribution contract has been applied to its exact runtime
closure.

## 8. Clean-host lifecycle qualification

Packaging evidence shall precede clean-host qualification. The exact package
hash shall then be tested on a clean instance of the named Raspberry Pi fixture
for:

1. installation using only declared dependencies;
2. `--help` and `--version` startup without file, network, or device access;
3. a bounded recorded-input success case and representative fail-closed cases;
4. output ownership, permissions, integrity, and independent hash verification;
5. upgrade and reinstall behavior;
6. removal and, when applicable, purge behavior; and
7. final cleanup with no undocumented service, user, group, rule, or mutable
   state left behind.

The package shall not install or enable a service, timer, udev rule, kernel
module, boot setting, network listener, or automatic update mechanism unless a
later contract explicitly requires and reviews it.

## 9. Capture and device qualification

SoapySDR compilation and mocked adapter tests remain hardware-free evidence.
Real enumeration, construction, streaming, or USB/device access requires a
separate authorization naming the exact Pi and SDR and bounding the test.

A positive capture cell shall retain the package hash; Pi, OS, kernel, USB and
power configuration; SDR hardware identity; SoapySDR core, module, driver,
firmware, and vendor-runtime versions; antenna/input; requested and effective
settings; duration and byte bounds; overflow, timeout, cancellation, abort, and
cleanup results; artifact hashes; and final device state.

Capture qualification applies only to that exact combination. It does not
establish general Raspberry Pi support, another SDR's support, estimator
validity, profile correctness, end-to-end calibration, or RF accuracy.

## 10. End-to-end calibration qualification

End-to-end qualification is a later, separately authorized gate using the exact
installed package and an authorized SDR/reference setup. It shall satisfy the
project's end-to-end qualification contract and retain the complete chain from
acquisition through independent artifact-hash verification and final safe
state.

A package build, clean installation, CLI success, synthetic observation,
SoapySDR discovery, or capture-qualified artifact cannot substitute for this
gate.

## 11. Evidence and claim levels

The platform matrix is the authoritative index of positive claims. Evidence
shall advance independently through these states:

1. source-configured;
2. native build-covered;
3. package-content audited;
4. clean-host lifecycle qualified;
5. capture-qualified for one exact device combination; and
6. end-to-end calibration qualified for one exact combination.

A later state does not silently fill a different platform, package, or device
cell. Missing, conflicting, or unretained evidence is `Not qualified`, not an
inferred pass.

## 12. Non-goals

This contract does not authorize or require:

- a GUI, web UI, remote-control API, daemon, or unattended scheduler;
- 32-bit Raspberry Pi OS or a general Debian/Ubuntu ARM support claim;
- cross-platform scientific divergence or a Raspberry Pi profile variant;
- bundled Qt in any CLI-only artifact;
- automatic SDR selection or automatic fallback when capture support is absent;
- a new SDR dependency, vendor SDK, kernel module, or device rule without its
  own review;
- live SDR, USB, GPIO, I2C, network, reference, or RF activity; or
- publication of a release merely because a build or package exists.

## 13. Exit gate

The recorded-input Raspberry Pi CLI is qualified only when one exact source
revision and package hash have passing retained evidence for native build and
tests, resource bounds, exact-payload and license disposition, clean-host
lifecycle, representative production-CLI behavior, cleanup, and independent
review, and the platform matrix records only those demonstrated claims.

The capture variant and end-to-end calibration remain `Not qualified` until
their separate gates pass. Any unresolved portability, resource, dependency,
identity, integrity, uncertainty, cleanup, or evidence finding fails closed.
