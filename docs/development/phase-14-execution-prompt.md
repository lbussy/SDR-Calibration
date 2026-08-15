# Phase 14 execution prompt

## Objective

Implement the roadmap's Phase 14 portability, packaging, and qualification
foundation from the committed Phase 13 state. Make release inputs and claims
machine-checkable without turning a successful build into a platform, device,
distribution-license, signing, or calibration-accuracy claim.

## Governing contracts

Read `AGENTS.md`, the roadmap, implementation status, testing strategy,
application architecture, licensing and distribution policy, decision 0012,
Qualification Gate 1, all prior phase prompts, and the current source/build
contracts before changing files. Preserve the native SDR Calibration Profile as
authoritative and keep normal validation hardware-free and network-free.

## Required implementation

1. Record explicit minimum CMake, C++ compiler-language, Qt, and SoapySDR
   dependency baselines. Release configuration must fail if a requested GUI or
   SoapySDR component is absent or below its baseline; ordinary developer builds
   may still disable or omit optional surfaces explicitly.
2. Add install rules for the production CLI, diagnostic capture CLI when built,
   desktop application when built, project license, third-party notices,
   documentation pointers, and exact dependency inventory.
3. Generate a deterministic, machine-readable SPDX 2.3 dependency inventory for
   the exact configured build. It must distinguish linked/required dependencies
   from optional or externally supplied device modules and must not guess
   transitive packages, copyright holders, licenses, or source dispositions.
4. Provide portable archive packaging through CPack and fail closed when the
   required license, notice, inventory, or production binaries are missing.
   Treat native installers, runtime dependency deployment, signing,
   notarization, and store submission as later evidence-bearing platform gates.
5. Add a repeatable package audit that installs into an isolated prefix, checks
   expected content, rejects build-tree path leakage in distributed metadata,
   verifies that the CLI starts, and records unresolved runtime-library checks
   rather than silently passing them.
6. Add macOS, Linux, and Windows CI recipes that exercise the hardware-free core
   with platform-native compilers. At least one job must exercise the strict
   full release dependency and package audit. CI definitions establish build
   coverage only after they pass; they never establish clean-install, signing,
   device, or calibration qualification.
7. Create an evidence-indexed platform/device support matrix. Every positive
   claim must name the exact OS, architecture, dependency versions, source
   revision, test class, and retained evidence. Unexecuted cells must say
   `Not qualified`, not `Supported`.
8. Define the end-to-end calibration qualification record and acceptance gate.
   It must separately prove acquisition, estimation, observation acceptance,
   fitting, uncertainty, profile generation, integrity/signature disposition,
   and bounded profile evaluation against an authoritative reference. The prior
   capture-only hardware record cannot satisfy it.
9. Update the roadmap, status, testing, repository/source documentation,
   licensing policy, user guidance, toctrees, and decision log so implemented
   tooling, CI intent, remaining qualification, and release readiness agree.
10. Add deterministic tests for new scripts and build behavior where practical.
    New warnings and incomplete or misleading release metadata are defects.

## Non-goals and safety boundary

- Do not access an SDR, enumerate devices, start a stream, transmit RF, or
  perform a new hardware qualification.
- Do not download or install dependencies during normal validation.
- Do not claim Windows, Linux, Raspberry Pi, installer, signing, notarization,
  or clean-host support from source inspection or a local macOS build.
- Do not bundle Qt, SoapySDR, vendor modules, or their transitive dependencies
  in this slice. Do not invent corresponding-source or license dispositions.
- Do not implement live-device production UI integration, production signing or
  trust stores, profile activation, network registry transport, or WSJT-X file
  mutation.
- Do not publish a release.

## Validation and exit gate

Run the baseline debug build/tests/docs checks, a clean strict release build,
release CTest, isolated install/package generation, package audit, SBOM syntax
and determinism checks, license/notice checks, and `git diff --check`. Review the
complete diff and run an adversarial pass for false claims, optional-dependency
bypass, missing payloads, path leakage, non-determinism, unsafe test behavior,
and platform-specific assumptions. Reinject and close every finding, repeating
validation and review as necessary.

This implementation slice is complete when its local checks pass and the
remaining evidence gates are explicit. Roadmap Phase 14 itself is accepted only
after the configured macOS/Linux/Windows CI jobs, clean-install/package checks,
applicable signing checks, and every claimed platform/device/end-to-end
qualification cell have retained passing evidence. A partial matrix is a valid
fail-closed result; relabeling an untested cell is not.
