# wspr4 Raspberry Pi OS package and install execution prompt

## Objective

Perform a bounded, hardware-free package and installation qualification of the
recorded-input `sdrcal` CLI on `wspr4`, a Raspberry Pi 4 running 64-bit
Raspberry Pi OS 13. Build committed source natively, construct and audit the
ARM64 DEB, install that exact artifact, exercise startup and representative
recorded-input behavior, then purge it and verify cleanup.

This result is limited to the exact host, OS, architecture, source revision,
and artifact hash retained by the run. It does not qualify SDR capture,
physical hardware, references, calibration accuracy, or RF behavior.

## Bound source and host

- Use clean committed repository `HEAD`; do not transfer or package local
  uncommitted changes.
- Record the complete source revision and independently compute the DEB hash.
- Confirm the host name, board model, OS identity, kernel, architecture, RAM,
  storage, compiler, CMake, Ninja, and Debian packaging-tool versions.
- Install only the resulting `sdrcal` package and dependencies already declared
  by that exact DEB. Do not alter services, boot configuration, users, groups,
  device rules, kernel modules, or networking configuration.

## Build and package

1. Confirm the checkout is clean and fast-forward it to the bound committed
   candidate.
2. Configure the `raspberry-pi-cli-release` preset natively with GUI, Qt,
   SoapySDR, and device access disabled by its reviewed contract.
3. Build and run all deterministic hardware-free tests.
4. Run `packaging/raspberry-pi/package-deb.sh` against the release build.
5. Retain the script's host, dependency, payload, ELF, license-disposition,
   source-revision, manifest, and SHA-256 evidence.

## Install and behavior checks

1. Record whether `sdrcal` is installed before the test.
2. Install the exact audited DEB using APT so only declared dependencies are
   resolved.
3. Verify the installed executable path, package metadata, payload hashes,
   `--help`, and `--version`.
4. Run the repository's bounded production-CLI fixture through the installed
   executable and verify canonical success plus expected atomic outputs.
5. Run representative fail-closed cases for malformed input and an existing
   output directory.
6. Reinstall the same DEB, verify startup again, remove and purge it, and
   confirm the executable and package-owned files are absent.

## Safety boundary

- Do not enumerate, open, configure, or stream from an SDR or USB device.
- Do not access GPIO, I2C, DMA, PWM, clocks, transmitters, antennas, or RF.
- Do not start or modify a service, listener, scheduler, or automatic updater.
- Do not reboot or shut down the host.
- Stop and report rather than weakening an identity, dependency, payload,
  integrity, behavior, or cleanup check.

## Exit report

Report build and test results, exact source revision, DEB path and SHA-256,
installation and runtime results, reinstall and purge results, final cleanup,
installed dependencies or other host changes, and every skipped or failed
check. State explicitly that qualification is limited to this exact host, OS,
architecture, source revision, and artifact hash.
