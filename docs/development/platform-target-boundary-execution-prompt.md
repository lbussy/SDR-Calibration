# Initial platform target boundary documentation prompt

## Objective

Record the approved initial operating-system and architecture targets throughout
the durable project documentation without turning a product target, build result,
or package result into a support or qualification claim.

## Approved initial targets

- macOS 14.0 or later on Apple Silicon only, with the GUI and CLI.
- Windows 11 x64, with the GUI and CLI.
- Ubuntu 24.04 LTS x86_64, with the GUI and CLI.
- Raspberry Pi OS 13 ARM64 on Raspberry Pi 4, with the recorded-input CLI only.

## Explicitly outside the initial supported matrix

- Intel macOS.
- Windows ARM64.
- Windows 10 unless deliberately retained by a later decision.
- Generic Linux distributions.
- Ubuntu ARM64 desktop.
- 32-bit Raspberry Pi OS.
- Raspberry Pi models other than Raspberry Pi 4 until separately qualified.
- SoapySDR or other live-device operation on Raspberry Pi.

## Required documentation work

1. Add an accepted decision record for the boundary and identify the portions of
   earlier platform decisions that it supersedes.
2. Update current overview, architecture, user-guide, status, and qualification-
   matrix language to use the exact targets.
3. Keep existing evidence rows intact. A target is not automatically build-
   covered, packaged, install-qualified, device-qualified, or supported.
4. Preserve historical execution prompts as records of the scope in force when
   they were executed.
5. Add this prompt and the decision to the Sphinx navigation.

## Constraints

- Documentation only: do not modify production code, build definitions, package
  definitions, workflows, schemas, or tests.
- Do not access SDR hardware, start sample streams, or perform RF testing.
- Do not broaden any device or calibration claim.

## Validation and exit gate

- Build the Sphinx documentation with warnings treated as errors.
- Check documentation changes for whitespace errors and stale current-scope
  language.
- Confirm that only the intended documentation files changed.
- Commit and push the bounded documentation update only after validation passes.
