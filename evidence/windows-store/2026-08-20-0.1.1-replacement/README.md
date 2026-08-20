# Windows Store 0.1.1 replacement build and lifecycle evidence

Date: 2026-08-20 UTC

This record binds the replacement package produced after the first Partner
Center rejection. The build ran hardware-free on `ecm-mule` from a fresh clone
whose clean `main`, `HEAD`, and `origin/main` all resolved to
`4406a82e01072afc0d61d2516c2fe9607c608ea4`.

The exact unsigned artifact is retained only on the Windows host at:

`C:\Users\lee\SDR-Calibration-Store-0.1.1-h-4406a82\build\windows-store-release\windows-store-package\SDRCalibration-0.1.1-Windows-Store-x64.msix`

- package version: `0.1.1.0`;
- architecture: `x64`;
- byte size: `95910402`; and
- SHA-256: `6d6998bb2130b9f137ac2847c8449f24259f5a526f1f8c67d66f7953f9327f08`.

The permanent wrapper discovered MSVC 19.51 through Visual Studio's
`vswhere.exe`/`VsDevCmd.bat` contract and Qt 6.11.1 at `C:\Qt`. It verified the
retained Qt corresponding-source archives against their previously approved
hashes. Configure, the Release build, all 19 hardware-free tests, package audit,
MakeAppx 10.0.26100.8249 pack, and MakeAppx unpack passed. Two existing C4996
warnings for `_open` remained in unchanged `ProfileStorage.cpp` and
`ReferenceRegistry.cpp`; this slice introduced no C++ change or new warning.

Independent post-build inspection found the unpacked manifest values:

- identity `LeeBussy.SDRCalibration`;
- publisher `CN=66465467-9B9D-4BDE-9CC9-BE392698D910`;
- publisher display name `Lee Bussy`;
- resource/default language `en-us`;
- `Properties/Logo` -> `Assets\StoreLogo.png`, PNG, 50x50;
- `Square150x150Logo` -> `Assets\Square150x150Logo.png`, PNG, 150x150; and
- `Square44x44Logo` -> `Assets\Square44x44Logo.png`, PNG, 44x44.

The same-host elevated interactive lifecycle used a temporary, non-exportable
development certificate and passed signing, signature verification, install,
the `sdrcal.exe` alias, visible GUI first launch, visible GUI relaunch,
uninstall, and cleanup. Independent cleanup inspection found zero installed
packages, zero GUI processes, and no temporary certificate in either store.
The temporary scheduled-task registration was deleted after its successful
run. The one-use launcher and its stdout, stderr, and exit-code files remain in
the Windows user's Downloads folder; the recorded exit code and Task Scheduler
last result were both zero.

No package was selected or uploaded, no Partner Center field was changed, and
no certification or publication action occurred. Local construction and
development lifecycle qualification do not establish Partner Center
acceptance, Microsoft signing, certification, Store delivery, clean-host Store
behavior, or device qualification.

The owner subsequently approved the complete replacement path, revision,
version, architecture, byte size, and SHA-256 binding on 2026-08-20 UTC. That
approval records the intended artifact only. It did not authorize selecting or
uploading the package, and no Partner Center action occurred.
