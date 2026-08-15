# Phase 14.3 execution prompt

## Objective

Implement the Ubuntu 24.04 x86_64 DEB production gate from the committed Phase
14.2 state. Produce a dynamically linked Qt package whose source identity,
system-runtime dependency declarations, payload, metadata, and artifact hash
are independently reviewable.

## Required implementation

1. Add a dedicated Ubuntu 24.04 x86_64 release configuration for the
   recorded-input CLI and GUI with SoapySDR and vendor modules disabled.
2. Install under the normal `/usr` filesystem layout in an isolated `DESTDIR`
   and derive exact system-library dependencies with Debian tooling.
3. Reject dirty source, the wrong distribution or architecture, missing Debian
   tooling, unsafe output paths, unexpected capture payloads, incomplete Qt
   dependencies, and unresolved runtime libraries.
4. Build an `amd64` DEB with root-owned archive metadata without requiring
   root, then extract and audit its control metadata and payload.
5. Reject missing required content, build-tree path leakage, unexpected ELF
   architecture, RPATH/RUNPATH entries, or CLI startup failure.
6. Retain the exact source revision, Ubuntu release, architecture, CMake,
   compiler and Qt versions, dependency policy, artifact and payload hashes,
   dynamic-library inspection, and explicit open-gate dispositions.
7. Exercise the gate on Ubuntu CI and retain the DEB plus evidence as a CI
   artifact.
8. Update build, testing, licensing, roadmap, status, matrix, user/developer
   guidance, and the decision log without claiming clean-host installation or
   completed binary-license disposition.

## Non-goals and safety boundary

- Do not access SDR hardware, enumerate devices, start streams, or perform RF
  or calibration qualification.
- Do not bundle SoapySDR, vendor modules, or Qt libraries. The DEB uses Ubuntu
  system packages through generated dependencies.
- Do not implement Phase 14.4 license disposition, clean-host installation,
  repository publication, release publication, or automatic updates.
- Do not treat extraction, dependency resolution, or startup on the build host
  as clean-install, general Ubuntu support, device support, or calibration
  evidence.

## Validation and adversarial exit gate

Run baseline build/tests/docs, Ubuntu release build/tests/staged-package audit,
shell syntax and lint checks, DEB construction, control and extracted-payload
audit, runtime/startup inspection, evidence inspection, and `git diff --check`.
Review the complete diff and artifact adversarially for privilege requirements,
unsafe replacement, mutable inputs, missing dependencies or payloads, path
leakage, architecture/RPATH mistakes, false claims, and unsafe hardware
behavior. Inject every finding below, correct it, and repeat.

Phase 14.3 implementation is complete when hardware-free checks pass and the
Ubuntu-only production gate is ready to fail closed. Artifact qualification
requires a passing retained Ubuntu run against one exact DEB hash; clean-host
installation and Phase 14.4 distribution-license disposition remain open.

## Injected adversarial findings

Findings discovered during execution are recorded here with their corrective
disposition before the final repeated validation. An empty final list means the
last adversarial pass found no open issue, not that later Phase 14 gates passed.

1. **Desktop integration was absent:** the initial payload installed the GUI
   executable but no desktop launcher. Corrected by installing and auditing a
   freedesktop desktop entry that invokes `sdrcal-gui` without a terminal.
2. **Production script lint was local-only:** the initial Ubuntu job could run
   the package script without first checking its syntax and shell diagnostics.
   Corrected by declaring ShellCheck and running both `bash -n` and ShellCheck
   before configuration.
3. **Tool preflight was incomplete:** the initial script used `mkdir` and `rm`
   without listing them among required tools. Corrected by including every
   external filesystem command in the fail-closed preflight.
4. **Maintainer metadata was not policy-conformant:** the initial Debian control
   record named the contributor group without a contact address. Corrected by
   using a stable project-owner contact in both the source and binary control
   records.
5. **Derived dependencies were parsed under the wrong substitution prefix:**
   the first Ubuntu CI run correctly failed because `dpkg-shlibdeps -p sdrcal`
   emits `sdrcal:Depends`, while the script accepted only `shlibs:Depends`.
   Corrected by accepting the package-qualified Debian substitution prefix and
   retaining the derived dependency inventory as evidence.
