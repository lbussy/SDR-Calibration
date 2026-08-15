# 0017 — Ubuntu DEB system-dependency boundary

Status: Accepted

Date: 2026-08-15

## Decision

Package the recorded-input CLI and Qt Widgets application as an Ubuntu 24.04
x86_64 DEB installed below `/usr`. Keep SoapySDR and vendor modules outside this
slice. Dynamically link Qt from Ubuntu system packages and derive the DEB's
runtime dependency declaration from the exact staged ELF payload with
`dpkg-shlibdeps`; do not copy Qt libraries into the package.

Build the archive without root using root-owned archive metadata. Reject dirty
source inputs, a non-Ubuntu-24.04 or non-x86_64 host, unsafe output placement,
missing payloads, incomplete Qt dependencies, build-path leakage,
RPATH/RUNPATH, unresolved libraries, and extracted CLI startup failure. Retain
the source revision, environment and tool versions, control metadata, artifact
and payload hashes, and dynamic-library inspection.

## Consequences

A passing retained run can establish package construction and extracted-payload
integrity for one exact DEB hash. It does not establish a clean installation,
repository compatibility, completed binary-license disposition, general Ubuntu
support, device support, or calibration accuracy. Phase 14.4 must audit the
exact dependency and notice disposition before release.
