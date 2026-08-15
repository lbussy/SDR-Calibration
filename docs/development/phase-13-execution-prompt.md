# Phase 13 execution prompt: Qt Widgets desktop application

## Objective

Deliver an accessible Qt 6 Widgets desktop application over the same recorded-
input production workflow and publication transaction as the Phase 12 CLI. The
application must let an operator explicitly select and review a versioned run
request, its independently selected registry-trust file, and a new output
directory; run calibration without blocking the event loop; cancel
cooperatively; and review the resulting native profile, evidence, summary, and
optional lossy WSJT-X projection. It must remain hardware-free and network-free.

## Governing contracts

Follow the application architecture, native profile, reference, evidence,
failure-state, licensing, roadmap, Phase 11 and Phase 12 contracts, and project
instructions. The native SDR Calibration Profile remains authoritative. The
GUI is a delivery surface: it must call the existing production/application
services and must not duplicate scientific, trust, validation, publication, or
interoperability policy.

## Required implementation

1. Add an optional `sdrcal-gui` target using only dynamically linked Qt 6 Core,
   Widgets, and Concurrent. If Qt is unavailable, configuration must clearly
   report that the optional GUI is disabled while the CLI/core remain usable.
2. Present explicit selectors for the recorded-calibration request, independent
   trust-pin file, and output directory. Never discover devices, files,
   credentials, installed WSJT-X instances, or trust anchors.
3. Provide bounded, read-only request review before execution. Show the exact
   selected request and enough structured metadata to review its device,
   effective configuration, observations, references, evidence policy, and
   requested interoperability. Parsing for execution remains authoritative in
   the Phase 12 production service.
4. Delegate execution to `runProductCommand` with the same argument mapping,
   terminal JSON, exit categories, authentication, scientific workflow, and
   coordinated new-directory publication behavior as the CLI.
5. Run calibration on a worker through Qt Concurrent so the GUI event loop
   remains responsive. Disable conflicting controls while running, expose one
   unambiguous Cancel action, use a thread-safe cancellation token, and wait for
   an active worker during orderly application shutdown.
6. Report progress and diagnostics in a bounded, read-only view. Render the
   exact terminal JSON separately. Map success, input, workflow, output, and
   cancellation outcomes to distinct accessible status text without weakening
   or reclassifying the production result.
7. After success, load only the fixed, known artifact names from the selected
   output directory, enforce per-file byte limits, and provide read-only tabs
   for `profile.json`, `evidence.json`, `summary.json`, and optional
   `wsjtx.ini`. Missing required artifacts or unsafe/unreadable files must be
   visible as review failures, not silent success.
8. Provide a read-only Open Result action for an existing result directory.
   Profile management in this phase means inspection only: do not activate,
   replace, sign, revoke, edit, or delete profiles.
9. Supply keyboard navigation, labels associated with inputs, accessible names
   and descriptions, visible focus, scalable native text, logical tab order,
   status announcements, and no color-only state communication. Avoid custom
   painting and OpenGL.
10. Keep all UI text truthful about recorded input, local pinned trust,
    unsigned profiles, lossy WSJT-X output, and the absence of live-device or
    accuracy qualification.
11. Add deterministic, offscreen, hardware-free tests for CLI/GUI argument and
    exit alignment, bounded file review, cancellation-token behavior,
    accessibility metadata, initial/running/completed control states, and
    required/optional artifact review. Normal tests must require no display,
    network, SDR, vendor driver, or external service.
12. Update roadmap, status, architecture, source layout, testing, user guide,
    and toctrees. Mark only the recorded-input desktop workflow implemented.

## Non-goals

- No SDR enumeration, construction, tuning, streaming, GPIO, or RF activity.
- No live-device calibration or production SoapySDR integration.
- No network registry transport, telemetry, credential storage, production
  trust store, Ed25519 key provider, or signing UI.
- No profile activation, replacement, editing, deletion, revocation, or
  evaluation orchestration.
- No installed WSJT-X discovery, backup, locking, mutation, or readback.
- No packaging, installer, signing, notarization, platform-support,
  supported-device, calibration-accuracy, or release-readiness claim.
- No new Qt module, custom widget framework, scientific model, or duplicated
  application policy.

## Validation and exit gate

Run the default configure/build/CTest presets, a GUI-disabled configuration,
build, and CTest run, a SoapySDR-disabled configure/build/CTest run, Sphinx with
warnings as errors, formatting/diff checks, and an adversarial review against
this prompt and the governing contracts. Inject every material finding below,
correct it, and repeat until no blocker remains. Phase 13 is complete only when
the recorded-input desktop path is responsive, accessible, bounded,
fail-closed, and demonstrably aligned with the CLI. UI completion does not
establish live-device integration, production trust, calibration accuracy,
portability, packaging, qualification, or release readiness.

## Adversarial findings injected into the exit gate

Findings from each review cycle are retained here with the invariant that
closed them.

The first review required:

- Progress must be delivered incrementally from the production diagnostic
  stream while work is running, rather than displayed only after completion.
- A required-artifact review failure must remain the terminal visible status;
  the generic workflow-success message cannot overwrite it.
- Every visible path label must be explicitly associated with its line edit so
  assistive technology and keyboard mnemonics retain the form relationship.

The second review required:

- The progress bridge must emit complete diagnostic lines as they are written
  and retain the exact complete diagnostics; buffering an ordinary stream until
  completion does not satisfy responsive long-operation feedback.
- Result-directory review must reject a symbolic-link directory as well as
  symbolic-link artifact files so the fixed-name review boundary cannot be
  redirected after operator selection.
- A GUI-only build must still compile the shared production service without
  producing CLI executables; `SDRCAL_BUILD_CLI=OFF` must not break the GUI.

The third review required the offscreen widget test to exercise the actual
initial, running, and completed-failure control states rather than infer the
transition from helper tests. It also required an explicit block limit on the
live diagnostic view so a long run cannot grow the presentation without bound.
