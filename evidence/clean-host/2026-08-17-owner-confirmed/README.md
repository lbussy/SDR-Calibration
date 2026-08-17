# Owner-confirmed clean-host lifecycle results

Date recorded: 2026-08-17

The project owner confirmed that the following exact, already indexed package
artifacts passed installation and lifecycle checks on clean boxes:

| Platform | Exact artifact | SHA-256 | Result |
| --- | --- | --- | --- |
| macOS ARM64 | `SDRCalibration-0.1.0-macOS.dmg` from source `5eb3670` | `94a831d4549b92edd85222c55e0cd64395dbe8acfd1f4bd5c57351f15bf80ad4` | Clean-host install, first launch, normal relaunch, CLI help, removal, and residue review passed |
| Windows 11 x64 | `SDRCalibration-0.1.0-Windows-x64.msi` from source `15ed17b` | `7be98c92de35bb31024152161f9b3bb4e76cbf6a3d2728001060f3d338d81973` | Clean-host install, launch, maintenance-install, uninstall, residue review, and application/icon review at relevant scaling settings passed using the locally trusted self-signed development certificate |
| Raspberry Pi OS 13 ARM64 | CLI-only DEB from source `5f50d31` | `d3de3d6397aaec7c3959e8854ac1871a787e4850706ae7736761a3fa887605ef` | Clean-host install, CLI execution, removal, and residue review passed |

These are owner-accepted clean-host lifecycle results for the exact hashes
above. They do not transfer to rebuilt artifacts.

The following gates remain separate and open:

- Windows public-trust signing, RFC 3161 timestamping, and resulting
  SmartScreen observations;
- prior-version upgrade and rollback qualification for a candidate newer than
  `0.1.0`;
- physical reference suitability and end-to-end calibration qualification;
- release-candidate binding, final evidence-ledger population, publication,
  and release authorization.
