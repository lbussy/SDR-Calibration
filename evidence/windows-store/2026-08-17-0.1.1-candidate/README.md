# Windows Store 0.1.1 candidate construction

This record binds the hardware-free Windows construction performed on
`ecm-mule` to source revision
`2e678b8d0e2869d79626c80511398c809d8039b7`.

## Result

- Host: Windows 11 Pro build 26200, x64.
- Toolchain: MSVC 19.51.36256.0, CMake 4.4.2, Ninja 1.13.2, Qt 6.11.1,
  MakeAppx 10.0.26100.8249.
- Clean checkout: `C:\Users\lee\SDR-Calibration-Store-0.1.1-c`, synchronized
  `main`/`origin/main` at the revision above before and after construction.
- Configure and release build completed. The build retained the two existing
  MSVC C4996 diagnostics for `_open` in `ProfileStorage.cpp` and
  `ReferenceRegistry.cpp`; the new fixture-retention code emitted no warning.
- All 19 Windows hardware-free tests passed.
- The staged package audit, Qt runtime deployment, license disposition,
  MakeAppx manifest/package creation, and unpacked-payload inspection passed.
- Both matching Qt 6.11.1 corresponding-source archives were supplied:
  `qtbase` SHA-256
  `3529cc37297a5a7aae4486843b9fd41c30df1d79a770f85e240b537dcc327ca5`
  and `qtsvg` SHA-256
  `767730188d4610a89bf8da502f87acf1c8881a3ac54f1e0eb167ab1e08b03a75`.
- Artifact: `SDRCalibration-0.1.1-Windows-Store-x64.msix`, 95,903,759 bytes,
  SHA-256
  `e359b10ee29216e733d75544cbe2c6c729c9bee646f17f42d71583dbe9a46168`.
- Manifest identity is `LeeBussy.SDRCalibration`, version `0.1.1.0`, x64,
  publisher `CN=66465467-9B9D-4BDE-9CC9-BE392698D910`, with only
  `runFullTrust` declared.
- Signing state is unsigned pre-submission; certification state is not
  submitted. Microsoft Store signing was not observed.

The exact package, unpacked payload, runtime inventory, payload hashes, and
license manifest remain on `ecm-mule` under
`C:\Users\lee\SDR-Calibration-Store-0.1.1-c\build\windows-store-release\windows-store-package`.
They are not committed as binaries.

## Certification fixture

The generator completed successfully from the same clean synchronized source
and Windows build. Its manifest records Windows 11, AMD64, candidate `0.1.1`,
the exact source revision above, `source_tree_clean: true`, and
`certification_ready: true`. The fixture remains at
`C:\Users\lee\SDR-Calibration-Store-0.1.1-fixture-final-2e678b8` and contains
only synthetic test inputs. This does not qualify a device or calibration
accuracy.

## Open gates

- Windows App Certification Kit was not available through the inspected
  command environment and was not run.
- The unsigned package was not installed or development-signed in this slice.
  GUI, CLI alias, launch/relaunch, uninstall, and cleanup remain open for this
  exact hash.
- Exact-candidate screenshots have not been captured.
- No Partner Center inspection, upload, validation, certification request,
  signing, delivery, or publication occurred.
