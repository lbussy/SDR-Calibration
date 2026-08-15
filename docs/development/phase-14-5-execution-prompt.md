# Phase 14.5 execution prompt

## Objective

Prepare and qualify clean-host installation candidates without carrying the
internal `sdrcal-gui` build-target name into any GUI-client user experience.
Every supported desktop platform shall present the product as `SDR Calibration`
and shall carry the same approved, project-owned application icon before any
clean-host result is accepted. The macOS bundle shall be named
`SDR Calibration.app`.

## Required implementation

1. Keep `sdrcal-gui` as the internal CMake target and executable basename unless
   a separate technical reason requires changing it. Set every platform's
   launcher, application menu, shortcut, installed-application listing, window
   identity where applicable, and package-facing documentation to
   `SDR Calibration`. On macOS this includes the bundle filename, Finder name,
   and menu-bar application name.
2. Update every macOS packaging, signing, notarization, Gatekeeper, launch-smoke,
   inventory, and evidence path to handle `SDR Calibration.app` safely. Paths
   containing spaces must be quoted and covered by hardware-free tests or
   script checks.
3. Add one original, project-owned master icon with a retained editable source
   and reproducible generation of each required platform asset, including macOS
   `.icns`, Windows `.ico` and executable/installer resources, and Linux desktop
   icon-theme assets. Record authorship, source-file hash, license, conversion
   tool/version, and every generated-asset hash; do not import an icon or font
   whose redistribution rights are unclear.
4. Configure `CFBundleIconFile` (or the current equivalent bundle metadata),
   install the icon in `Contents/Resources`, and fail the package gate if the
   declared icon is absent, empty, untracked by the payload inventory, or not
   included in the exact-package license disposition.
5. Configure the Windows executable, installer, installed-app entry, and Start
   menu/desktop shortcuts, plus the Linux freedesktop desktop entry and icon
   theme, to use the product name and generated icon. Fail each package gate if
   its declared asset is absent, empty, unresolved after installation,
   untracked by the payload inventory, or omitted from the exact-package
   license disposition.
6. Verify the final icon at every required platform representation, including
   small launcher/list sizes, Windows scaling variants, and macOS Retina
   Finder/Dock variants, with transparent-background, clipping, contrast, and
   legibility review. Retain rendered review evidence.
7. Rebuild, audit, hash, and apply the platform's required signing checks to
   every GUI package. Earlier packages that expose `sdrcal-gui` to users or omit
   the approved icon do not qualify the renamed candidate.
8. Perform clean-host install, first launch, normal launch, upgrade, uninstall,
   and clean-user-state checks only against the exact renamed and icon-bearing
   candidate. Keep platform, device, and calibration claims separate.

## Non-goals and safety boundary

- Do not rename the native SDR Calibration Profile, CLI commands, macOS bundle
  identifier, or source repository merely to match the product-facing name.
- Do not access, enumerate, open, or stream from SDR hardware.
- Do not treat visual approval, notarization, or clean launch as device support
  or calibration qualification.
- Do not publish a release from this phase.

## Validation and exit gate

Run the narrow hardware-free baseline, documentation build, packaging script
syntax checks, staged metadata inspection, installed launcher/shortcut checks,
icon-resource and payload-inventory checks, applicable platform signing checks,
exact-hash evidence review, and the clean-host lifecycle matrix. Review the
complete diff and exact candidates adversarially for user-visible `sdrcal-gui`
names, stale macOS bundle paths, unquoted spaces, unresolved Linux icon names,
missing Windows resources or icon representations, unclear asset rights,
broken upgrades, false qualification claims, and hardware access.

Phase 14.5 is complete only when retained clean-host evidence identifies every
exact renamed, icon-bearing GUI package hash. Icon design and the final visual
selection require project-owner approval before the candidates are frozen.
