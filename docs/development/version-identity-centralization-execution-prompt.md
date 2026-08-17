# Version identity centralization execution prompt

## Objective

Remove executable-version drift before a higher-version release candidate is
created. Make the CMake project version the single source for the production
CLI, diagnostic capture CLI, Qt GUI, macOS bundle, Windows MSI, Debian packages,
CPack filenames, SBOM, and packaging evidence inputs. Add deterministic,
hardware-free validation that fails when a version surface becomes hard-coded
or disconnected.

Do not increment the current `0.1.0` version or freeze a candidate in this
slice.

## Verified context

- `project(SDRCalibration VERSION 0.1.0)` is the intended current source of
  application/package version identity.
- `sdrcal`, `sdrcal-capture`, and `sdrcal-gui` independently hard-code
  `0.1.0`; this prevents the prepared upgrade/rollback candidate entry gate from
  establishing consistent identity after a future version change.
- macOS bundle versions already use `${PROJECT_VERSION}`.
- Windows MSI, Ubuntu/Raspberry Pi DEB scripts, macOS packaging, CPack, and the
  configured SPDX inventory receive `${PROJECT_VERSION}` from CMake.
- Release evidence package preparation is complete locally and must remain
  attributable and included with this slice.

## Required implementation

1. Configure a project-owned C++20 version header from `PROJECT_VERSION` under
   the build tree. Expose it through an interface target so no checked-in
   generated header or duplicate literal is required.
2. Make all three executable entry points use the configured constant for
   `--version` or Qt application version output.
3. Keep package scripts parameterized; do not introduce another authoritative
   version source or allow a packaging script to infer a Git tag.
4. Add a deterministic Python source-contract test that parses the CMake project
   version, verifies the configured header binding, rejects semantic-version
   literals in executable entry points, confirms all three targets link the
   version interface, and checks macOS, Windows, DEB, CPack, and SPDX bindings.
5. Add runtime CTest coverage confirming `sdrcal --version` exactly reports the
   current CMake project version without file, network, device, or RF access.
6. Update active readiness/status documentation to mark version centralization
   complete while leaving candidate version selection, build, qualification,
   signing, calibration, tag, and publication open.

## Constraints and non-goals

- Do not change `PROJECT_VERSION`, schema versions, profile semantics, package
  identities, bundle identifier, WiX `UpgradeCode`, or compatibility policy.
- Do not build production packages, sign, notarize, timestamp, install,
  upgrade, rollback, tag, publish, access external services, or touch hardware.
- Do not add a dependency or generated source file to Git.
- Preserve the completed release-evidence preparation changes.

## Validation

Run the default CMake configure/build/CTest baseline, the documentation build
with warnings as errors, the version contract directly if useful, and
`git diff --check`. Verify the built `sdrcal --version` output and review the
complete and staged diffs before committing.

## Adversarial review

Challenge for a second authoritative version; an executable still containing a
semantic version literal; a target missing the generated include dependency;
GUI/CLI/package disagreement; package scripts accepting a value not sourced
from CMake; tests that merely search for `0.1.0`; generated files staged;
hardware/network access; an accidental version bump; historical evidence
rewritten as candidate evidence; or a claim that centralization freezes or
qualifies a release. Correct every actionable finding and repeat affected
checks until clean.

## Exit criteria

- One CMake project version feeds every production version surface.
- Contract and runtime tests fail closed on drift.
- Current version remains `0.1.0` and no candidate is frozen.
- Release-evidence preparation remains valid and included.
- Hardware-free validation passes.
- Only attributable changes are committed and pushed to the current upstream.

## Execution result

Completed on 2026-08-17 without changing `PROJECT_VERSION`. A configured C++20
header and interface target now bind `sdrcal`, `sdrcal-capture`, and
`sdrcal-gui` to the CMake project version. A deterministic Python contract
checks the executable sources and every existing platform-package, CPack, and
SPDX binding; a separate runtime test verifies exact `sdrcal --version` output.

The default configure and build completed without new warnings, and all 20
hardware-free CTest tests passed. Direct checks showed `sdrcal 0.1.0` and
`sdrcal-capture 0.1.0`. Documentation built with warnings as errors and the
diff check passed.

Adversarial review widened the initial finding from the GUI to both CLI entry
points, which also contained independent `0.1.0` literals; all three were
corrected. The review confirmed no semantic-version literal remains in a
production entry point, generated headers remain build output, package scripts
remain parameterized from CMake, and no version bump, candidate freeze,
packaging, signing, hardware, or release operation occurred. No actionable
finding remained after correction.
