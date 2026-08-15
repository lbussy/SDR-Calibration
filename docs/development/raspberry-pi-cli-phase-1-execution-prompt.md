# Raspberry Pi CLI Phase 1 execution prompt

## Objective

Implement the hardware-free, recorded-input Raspberry Pi OS ARM64 CLI candidate
defined by the Raspberry Pi CLI-only contract. Produce a reproducible CLI-only
configuration, an explicit provisional input-resource ceiling, and fail-closed
archive and DEB packaging. Stop before native Raspberry Pi qualification,
clean-host installation, SoapySDR, device, network, or RF activity.

## Required implementation

1. Add a `raspberry-pi-cli-release` configure/build/test preset that builds
   `sdrcal`, disables the GUI and SoapySDR, enables tests and packaging, and
   applies an explicit provisional recorded-observation byte ceiling.
2. Make the recorded-input ceiling a validated CMake configuration value. The
   production request parser shall reject a declared observation bound above the
   configured ceiling before it creates staging output or reads sample data.
   Preserve the existing 2 GiB default on other build profiles.
3. Add deterministic unit coverage for the configured ceiling and retain shared
   CLI arguments, exit categories, JSON results, scientific workflow, and
   artifact semantics.
4. Add an ARM64 Raspberry Pi OS CLI-only DEB target. It must reject the wrong
   OS, architecture, dirty source, GUI/Qt/SoapySDR configuration, unexpected
   payloads, shared libraries, unresolved dependencies, RPATH/RUNPATH, wrong ELF
   architecture, build-path leakage, and failed extracted CLI startup.
5. Build the DEB without root, derive exact system runtime dependencies from the
   staged `sdrcal` ELF, retain dependency-license evidence, exact payload hashes,
   environment/configuration evidence, and the DEB SHA-256, then extract and
   audit the exact artifact.
6. Keep Qt, GUI executables, desktop files, icons, SoapySDR, vendor modules,
   services, rules, and device integration out of the candidate. Update testing,
   status, and operator-facing CLI documentation without creating a positive Pi
   platform or package claim.

## Validation and adversarial exit gate

Run JSON/preset validation, shell syntax checks, default hardware-free configure,
build, CTest, package audit, portable archive audit, the documentation build with
warnings as errors, and `git diff --check`. Inspect the configured and staged
payload for Qt, SoapySDR, GUI, desktop, icon, service, device-rule, build-path,
and host-architecture leakage.

Phase 1 implementation is complete when the hardware-free source contracts and
locally runnable checks pass. Raspberry Pi OS native build, resource
measurement, DEB construction, clean-host lifecycle, capture, and end-to-end
calibration remain `Not qualified` until separately authorized runs retain the
exact required evidence.

## Safety boundary

- Do not access a Raspberry Pi host or external service.
- Do not enumerate, open, configure, or stream from an SDR.
- Do not install packages, services, rules, kernel modules, or dependencies.
- Do not claim ARM64 build, DEB, installation, device, or calibration
  qualification from macOS source validation.
- Do not resume Phase 14.5 multi-platform clean-host work in this slice.
