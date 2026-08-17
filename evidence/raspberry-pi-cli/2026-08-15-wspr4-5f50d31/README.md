# Raspberry Pi recorded-input CLI native build and package evidence

Status: Native build-covered and package-content audited. The project owner
subsequently confirmed a clean-host lifecycle pass for the exact indexed DEB
hash in the
[owner-confirmed clean-host record](../../clean-host/2026-08-17-owner-confirmed/README.md).
Capture, device, prior-version upgrade, and end-to-end calibration are not
qualified.

## Bound identity

- Source revision: `5f50d3190999ee647fa1b650ed39e46175a54fd9`
- Packaging-foundation revision: `aef400706de57da3e7beda0432da2364dd8a009d`
- Resource-fixture hook revision: `0a319fa`
- Host: `wspr4`
- Board: Raspberry Pi 4 Model B Rev 1.1
- Architecture: ARM64 (`aarch64`)
- OS: Raspberry Pi OS, Debian GNU/Linux 13 (trixie), Raspberry Pi reference
  2026-06-18, pi-gen revision `ca8aeed0ae300c2a89f55ce9617d5f96a27e99e5`
- Kernel: `6.18.39+rpt-rpi-v8`
- Installed RAM: 3,886,900 KiB
- Compiler: GCC 14.2.0
- CMake: 3.31.6
- Ninja: 1.12.1
- UTC completion: 2026-08-15T23:13:30Z

The checkout was clean and synchronized with `origin/main`. CMake and Ninja were
the only dependencies installed for this validation. No service required a
restart. No SDR, USB device, GPIO, I2C, network listener, service, reboot, or RF
activity was performed.

## Native validation

The `raspberry-pi-cli-release` preset configured with the GUI and SoapySDR off,
the recorded-input ceiling at 134,217,728 bytes, and packaging/tests on. The
native build used three parallel jobs.

- Native release build: passed, 50 build actions on the first revision
- CTest: 12/12 passed
- Staged CLI-only package audit: passed
- Portable ARM64 archive audit: passed
- Portable archive SHA-256:
  `b746eaed3fdd2924b01b3d36a41bf88453b5148a34f0586e8f4086f7474b802b`
- Rootless ARM64 DEB construction and extracted-payload audit: passed

The DEB contains `sdrcal`, the MIT license, notices, configured SPDX inventory,
the authoritative native-profile schema, and exact dependency-license evidence.
It contains no GUI executable, Qt runtime, SoapySDR component, device module,
desktop entry, icon, service, rule, or conveyed shared library.

## Exact DEB

- Artifact: `sdrcal_0.1.0_arm64.deb`
- Size: 209,532 bytes
- SHA-256:
  `d3de3d6397aaec7c3959e8854ac1871a787e4850706ae7736761a3fa887605ef`
- Architecture: `arm64`
- Installed size: 644 KiB
- Extracted executable: stripped ARM aarch64 PIE, dynamically linked
- Runtime dependencies derived by `dpkg-shlibdeps`:
  - `libc6 (>= 2.38)`; installed `2.41-12+rpt1+deb13u3`
  - `libgcc-s1 (>= 4.0)`; installed `14.2.0-19`
  - `libstdc++6 (>= 13.1)`; installed `14.2.0-19`

The DEB hash was independently recomputed after copying the artifact to the
development Mac and matched the hash produced on `wspr4`. The binary had no
RPATH/RUNPATH or unresolved dependency, and its extracted `--help` startup
passed.

## Maximum-bound resource measurement

Two independent synthetic CF32LE observations were expanded from the reviewed
production CLI fixture to the configured maximum of 134,217,728 bytes each
(16,777,216 complex samples per observation):

- first observation SHA-256:
  `8966367d11f404ae5bef6fdaf0644607bc8fd320fce25153656cbe3b946b2296`
- second observation SHA-256:
  `941b7ae0ae0238326fcd178d4bc7d22e0cf0cf977e7e38bb594e06b3574dfa93`

The exact executable extracted from the DEB completed the shared calibration
workflow and atomically published all three required artifacts:

- exit: 0, canonical terminal status `success`
- peak resident set: 790,636 KiB
- user CPU: 18.440797 seconds
- system CPU: 2.543006 seconds
- swap free before: 2,091,004 KiB
- swap free after: 2,091,024 KiB (no swap growth)
- final throttling state: `0x0`
- final temperature: 47.7 C
- `profile.json` SHA-256:
  `2a5d0337945bf90f141fc012b735758f64d3dbb966c83525fe13d8ea446940d1`
- `evidence.json` SHA-256:
  `dd130b317ef1372b383f62d2a957227eef9aedbcb44e946159aa467a97adf3a0`
- `summary.json` SHA-256:
  `b105f7a1e2ed046abd49f56e23bdb0303f4ca875992f6422cd77c35ed83f8294`

An earlier adversarial probe at a provisional 268,435,456-byte ceiling completed
but peaked at 1,576,988 KiB RSS and increased swap use from zero to approximately
6.2 MiB. That ceiling was rejected rather than qualified. The bound was reduced
to 134,217,728 bytes and the complete build/package/resource sequence was
repeated at the final source revision.

## Claim boundary

This record establishes one native build-covered and package-content-audited
cell for the exact Raspberry Pi/OS/revision/package combination. It does not
establish clean installation, upgrade, reinstall, removal, purge, another Pi
model or OS, SoapySDR capture, an SDR device combination, calibration accuracy,
or release readiness. The DEB itself is retained on `wspr4` and is identified by
its independently verified hash; it is not published as a release artifact.
