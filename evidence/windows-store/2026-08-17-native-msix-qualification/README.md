# Native Windows Store MSIX qualification

This record retains the hardware-free native Windows construction and local
development lifecycle for the Windows Store MSIX source contract at revision
`99d96a61aabe6c421fcf59f788f04df5293609e0`.

## Result

- Windows 11 Pro build `26200`, x64, MSVC 19.51.36256.0, CMake 4.4.2,
  Ninja 1.13.2, and Qt 6.11.1 were used on `ecm-mule`.
- The release build completed and all 19 hardware-free tests passed.
- `package-audit` passed.
- MakeAppx schema validation, package creation, unpack inspection, runtime
  closure, payload hashes, and LGPL disposition passed.
- The unsigned pre-submission artifact is
  `SDRCalibration-0.1.0-Windows-Store-x64.msix`, 95,903,766 bytes, SHA-256
  `de28c7d9c11c1fcf95e70ffc80a5147c6dfd5f26e93c8f6138906a6f6ba95f85`.
- A separate copy was signed with a temporary development certificate matching
  the manifest publisher. Windows registered it with `Developer` signature
  kind; the `sdrcal.exe` execution alias reported `sdrcal 0.1.0`; the GUI
  launched and relaunched; uninstall passed.
- Post-test audit found no package registration, GUI process, or temporary
  certificate in the checked current-user or local-machine stores.

The MSIX binary, development-signed copy, public certificate, and private key
are intentionally not retained in Git. The selected text/XML evidence here is
non-secret and sufficient to bind the result to the artifact hash, manifest,
runtime inventory, tests, lifecycle checkpoints, and cleanup.

## Boundaries

This is a same-host, locally trusted development lifecycle. It does not establish
Microsoft Store signing, certification, Store-delivered installation or update,
clean-host behavior, prior-version update behavior, publication, live-device
support, or RF qualification. Partner Center remained a draft reservation; no
package was uploaded or submitted.

`manifest.json` records `store_lifecycle_qualified: false` because the packaging
manifest reserves that field for the Store-certified delivery lifecycle. The
separate local development lifecycle is recorded by `installed-package.txt`,
`cli-version.txt`, `gui-smoke.txt`, `lifecycle-checkpoints.txt`, and
`cleanup.txt`.
