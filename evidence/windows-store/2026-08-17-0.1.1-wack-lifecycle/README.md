# Windows Store 0.1.1 WACK and development lifecycle

This record binds hardware-free Windows App Certification Kit and local
development lifecycle results to the unsigned Store MSIX built from source
revision `957fbeb204177c9ba2a1582e936476244b201b9d`.

## Result

- Artifact: `SDRCalibration-0.1.1-Windows-Store-x64.msix`.
- Unsigned artifact SHA-256:
  `1d9828710dcec5c93862e217606a92f53c1470b5abb412a23725ebc811b1edc1`.
- The clean synchronized Windows build passed all 19 hardware-free tests.
- MakeAppx construction and unpack inspection passed with the configured Qt
  6.11.1 runtime and corresponding-source disposition.
- Windows App Certification Kit returned overall `PASS`. The corrected
  50-by-50 package logo, embedded Per-Monitor-v2 declaration, blocked
  executable analysis, and DPI-awareness validation all passed.
- A copied package was signed with a temporary, non-exportable development
  certificate whose subject exactly matched the manifest publisher. The public
  certificate was explicitly trusted in Local Machine `TrustedPeople` for the
  bounded test.
- Windows registered package version `0.1.1.0` with `Developer` signature kind.
  The `sdrcal.exe` execution alias reported `sdrcal 0.1.1`; GUI first launch
  and relaunch passed; uninstall passed.
- Final independent audit found no package registration, GUI process, or
  temporary certificate in either trust location used during troubleshooting.

The WACK XML remains on `ecm-mule` at
`C:\Users\lee\Downloads\SDRCalibration-0.1.1-957fbeb-WACK.xml`. The MSIX,
development-signed copy, public certificate, and private key are intentionally
not committed.

## Boundaries

This is a same-host development-signature lifecycle. It does not establish
Microsoft Store signing, certification, Store delivery or update, clean-host
behavior, prior-version update behavior, public trust, publication, device
support, calibration accuracy, or RF qualification. No package was uploaded or
submitted, and Partner Center was unchanged.
