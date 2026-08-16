# wspr2 Raspbian package and install execution prompt

## Objective

Perform a bounded, hardware-free compatibility test of the recorded-input
`sdrcal` CLI on `wspr2`, a Raspberry Pi Zero 2 W running 32-bit Raspbian 13.
Build natively, construct and audit a one-off `armhf` DEB, install that exact
artifact, exercise startup and representative recorded-input behavior, then
remove it and verify cleanup.

This is an exploratory compatibility result. It does not change the accepted
64-bit Raspberry Pi OS contract, add Raspberry Pi Zero 2 W or 32-bit Raspbian
to the supported matrix, or qualify a release.

## Bound source and host

- Use committed repository `HEAD` as the source payload; do not include local
  uncommitted documentation changes in the candidate.
- Record the complete source revision and independently compute the source and
  DEB hashes.
- Confirm the host name, board model, OS identity, kernel, architecture, RAM,
  storage, compiler, CMake, Ninja, and Debian packaging-tool versions.
- Install only the build tools needed for this test and the resulting `sdrcal`
  package. Do not alter services, boot configuration, users, groups, device
  rules, kernel modules, or networking configuration.

## Build and package

1. Configure the existing `raspberry-pi-cli-release` preset natively with the
   GUI, Qt, SoapySDR, and device access disabled.
2. Build with concurrency appropriate for the 512 MiB fixture and run all
   deterministic hardware-free tests.
3. Stage the normal install payload below `/usr` without root.
4. Construct a one-off `armhf` DEB using the same payload, dependency,
   licensing, RPATH/RUNPATH, unresolved-library, build-path, and forbidden-
   component checks as the reviewed Raspberry Pi package script.
5. Label retained evidence clearly as an exploratory 32-bit Raspbian artifact,
   not the contract's production ARM64 package.

## Install and behavior checks

1. Record whether `sdrcal` is installed before the test.
2. Install the exact audited DEB using APT so only declared dependencies are
   resolved.
3. Verify the installed executable path, package metadata, payload hashes,
   `--help`, and `--version`.
4. Run the repository's bounded production-CLI fixture through the installed
   executable and verify canonical success plus the expected atomic outputs.
5. Run representative fail-closed cases for malformed input and an existing
   output directory.
6. Reinstall the same DEB, verify startup again, remove and purge it, and confirm
   the executable and package-owned files are absent.

## Safety boundary

- Do not enumerate, open, configure, or stream from an SDR or USB device.
- Do not access GPIO, I2C, DMA, PWM, clocks, transmitters, antennas, or RF.
- Do not start or modify a service, listener, scheduler, or automatic updater.
- Do not reboot or shut down the host.
- Stop and report rather than weakening an identity, dependency, payload,
  integrity, behavior, or cleanup check.

## Exit report

Report build and test results, the exact DEB path and SHA-256, installation and
runtime results, removal/purge cleanup, installed dependencies or other host
changes, and every skipped or failed check. State explicitly that the result is
limited to this host, OS, architecture, source revision, and artifact hash.
