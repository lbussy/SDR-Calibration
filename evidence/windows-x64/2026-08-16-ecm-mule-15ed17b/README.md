# Windows x64 self-signed MSI evidence

This record covers the hardware-free Windows candidate run on `ecm-mule` on
2026-08-16. It is an exact-host lifecycle result, not a clean-host result. The
candidate used the locally trusted `SELF_SIGNED` development mode and is not a
publicly trusted or release-signed artifact.

The project owner subsequently confirmed a clean-host lifecycle pass for this
exact MSI hash. That separate result is retained in the
[owner-confirmed clean-host record](../../clean-host/2026-08-17-owner-confirmed/README.md).
Public-trust signing, timestamping, and SmartScreen qualification remain open.

## Exact inputs

- Source: `15ed17b8cb3e9ed75fa50bb407219cd2f3aaf193`, clean `main` checkout
- Host: Windows 11 Pro 25H2, x64, build 26200.9168
- Visual Studio Community 2026 18.9; MSVC 19.51.36256.0
- CMake/CTest 4.4.2; Ninja 1.13.2; Git 2.55.0.windows.4
- Qt 6.11.1 MSVC 2022 x64; WiX 4.0.5; Windows SDK 10.0.26100.0 SignTool
- SoapySDR disabled; no SDR hardware or RF activity
- Signing certificate subject: `CN=SDR Calibration Development`
- Signing certificate thumbprint: `CA8F9B7FA806AC33860E62222FD21DAD41E8BA40`
- Qt base source SHA-256:
  `3529cc37297a5a7aae4486843b9fd41c30df1d79a770f85e240b537dcc327ca5`
- Qt SVG source SHA-256:
  `767730188d4610a89bf8da502f87acf1c8881a3ac54f1e0eb167ab1e08b03a75`

## Results

- Native release configuration and build passed with three existing MSVC C4996
  deprecation warnings in unchanged source.
- All 14 hardware-free CTest tests passed: `branding_contract_tests`,
  `license_disposition_tests`, `capture_unit_tests`,
  `carrier_estimator_unit_tests`, `observation_acceptance_unit_tests`,
  `model_uncertainty_unit_tests`, `profile_engine_unit_tests`,
  `reference_evidence_unit_tests`, `wsjtx_adapter_unit_tests`,
  `application_workflow_unit_tests`, `capture_cli_argument_tests`,
  `production_cli_tests`, `gui_workflow_tests`, and `gui_widget_tests`.
- Staged package audit and branding contracts passed.
- WiX created the MSI; payload and MSI Authenticode checks passed; the
  administrative extraction, extracted CLI startup, runtime closure, and Qt
  licensing disposition checks passed.
- MSI: `SDRCalibration-0.1.0-Windows-x64.msi`
- MSI SHA-256:
  `7be98c92de35bb31024152161f9b3bb4e76cbf6a3d2728001060f3d338d81973`
- Silent install returned 0. The installed CLI `--help` passed, the GUI remained
  running for the five-second smoke interval, and the Start menu shortcut and
  Add/Remove Programs entry were present.
- A same-candidate maintenance install returned 0. This is not evidence of an
  upgrade from an older version.
- Silent uninstall returned 0. The install directory, shortcut, registration,
  and application processes were absent afterward.

The detailed package evidence and MSI logs remain on `ecm-mule` under the build
package directory and
`C:\Users\lee\Downloads\sdrcal-windows-lifecycle-15ed17b`. They are not
portable trust evidence because the development signing certificate is local to
the test host.
