# Retained 0.1.0 prior-version package baselines

Retention date: 2026-08-17

These exact installable artifacts are deliberately retained as inputs for a
future upgrade and rollback qualification against a candidate whose version is
strictly greater than `0.1.0`. They are not released artifacts, fresh-host
evidence, upgrade evidence, or off-host backups.

Each copy is outside its platform's build tree, stored below a directory named
by its full SHA-256, marked read-only, and independently hashed after copying.
Read-only host files remain administratively deletable; the content address and
this manifest detect substitution but do not provide write-once storage.

## macOS ARM64 DMG

- Package version: `0.1.0`
- Source revision: `5eb3670b08b8aee2c4d915e5553b140394dc0d01`
- Filename: `SDRCalibration-0.1.0-macOS.dmg`
- Size: 79,280,946 bytes
- SHA-256:
  `94a831d4549b92edd85222c55e0cd64395dbe8acfd1f4bd5c57351f15bf80ad4`
- Host: development Mac
- Original path:
  `/Users/lbussy/GitHub/SDR-Calibration/build/macos-official-release/macos-package/SDRCalibration-0.1.0-macOS.dmg`
- Retained path:
  `/Users/lbussy/SDR-Calibration-Release-Baselines/0.1.0/94a831d4549b92edd85222c55e0cd64395dbe8acfd1f4bd5c57351f15bf80ad4/SDRCalibration-0.1.0-macOS.dmg`
- File mode after copy: `0444`
- Signing state: Developer ID signed; application and DMG notarized, stapled,
  and Gatekeeper accepted in the exact-artifact evidence
- Post-copy hash: matched
- Source evidence: [macOS same-host record](../../macos-arm64/2026-08-17-same-host-5eb3670/README.md)

## Windows x64 MSI

- Package version: `0.1.0`
- Source revision: `15ed17b8cb3e9ed75fa50bb407219cd2f3aaf193`
- Filename: `SDRCalibration-0.1.0-Windows-x64.msi`
- Size: 85,491,712 bytes
- SHA-256:
  `7be98c92de35bb31024152161f9b3bb4e76cbf6a3d2728001060f3d338d81973`
- Host: `ecm-mule`
- Original path:
  `C:\Users\lee\SDR-Calibration\build\windows-release\windows-package\SDRCalibration-0.1.0-Windows-x64.msi`
- Retained path:
  `C:\Users\lee\SDR-Calibration-Release-Baselines\0.1.0\7be98c92de35bb31024152161f9b3bb4e76cbf6a3d2728001060f3d338d81973\SDRCalibration-0.1.0-Windows-x64.msi`
- File state after copy: Windows read-only attribute `True`
- Signing state: locally trusted self-signed development certificate without
  public trust or timestamping
- Post-copy hash: matched
- Source evidence: [Windows same-host record](../../windows-x64/2026-08-16-ecm-mule-15ed17b/README.md)

This MSI can exercise transition mechanics from the retained development build,
but it does not become publicly trusted merely because a future MSI is publicly
signed.

## Raspberry Pi ARM64 DEB

- Package version: `0.1.0`
- Source revision: `5f50d3190999ee647fa1b650ed39e46175a54fd9`
- Filename: `sdrcal_0.1.0_arm64.deb`
- Size: 209,532 bytes
- SHA-256:
  `d3de3d6397aaec7c3959e8854ac1871a787e4850706ae7736761a3fa887605ef`
- Host: `wspr4`
- Original path:
  `/home/pi/SDR-Calibration/build/raspberry-pi-cli-release/raspberry-pi-package/sdrcal_0.1.0_arm64.deb`
- Retained path:
  `/home/pi/SDR-Calibration-Release-Baselines/0.1.0/d3de3d6397aaec7c3959e8854ac1871a787e4850706ae7736761a3fa887605ef/sdrcal_0.1.0_arm64.deb`
- File mode after copy: `0444`
- Signing state: no repository or package signing is implemented
- Post-copy hash: matched
- Source evidence: [Raspberry Pi package record](../../raspberry-pi-cli/2026-08-15-wspr4-5f50d31/README.md)

## Qualification boundary

The three baselines are immutable by identity because any byte change changes
the recorded SHA-256. Host read-only state reduces accidental overwrite but is
not permanent storage. Upgrade and rollback qualification still requires:

1. a frozen candidate with a version greater than `0.1.0`;
2. exact candidate artifacts and hashes;
3. suitable fresh platform targets;
4. pre-upgrade state inventory and backup;
5. platform-native upgrade and rollback procedures; and
6. retained preservation, residue, startup, and cleanup evidence.

Reinstalling one of these packages over itself remains maintenance behavior,
not a version-to-version upgrade.
