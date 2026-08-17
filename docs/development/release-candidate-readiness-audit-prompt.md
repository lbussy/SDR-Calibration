# Release-candidate readiness audit execution prompt

## Objective

Perform a hardware-free, documentation-and-evidence-only audit of the current
repository against the release-candidate entry and exit contracts. Produce an
ordered blocker ledger, identify work that can safely proceed before physical
qualification resumes, and correct stale status claims found during the audit.

This slice does not freeze, build, sign, publish, or qualify a release
candidate. It prepares an accurate decision surface for later gated work.

## Verified starting point

- `main` is clean and synchronized after the exact macOS same-host package
  qualification at `ea1d4c9`.
- The current macOS DMG is Developer ID signed, notarized, stapled, Gatekeeper
  accepted, license-audited, and same-host lifecycle qualified for its exact
  hash. It is not clean-host or prior-version-upgrade qualified.
- The retained Windows MSI passed an exact-host lifecycle under a locally
  trusted self-signed certificate. It is not publicly trusted, clean-host, or
  prior-version-upgrade qualified.
- Raspberry Pi ARM64 has native build, package, and resource evidence but no
  clean-host lifecycle evidence.
- Airspy HF+ Discovery and SDRplay RSP1B have separate capture evidence but no
  end-to-end calibration qualification.
- The matched-receiver comparison and reference-suitability survey are paused
  until the planned splitter permits a controlled shared-antenna arrangement.

## Required execution

1. Reconcile the roadmap, implementation status, platform/device matrix,
   package evidence, end-to-end gate, testing policy, licensing policy, and
   operator-facing package documentation.
2. Correct only factual status drift supported by retained evidence. Preserve
   implementation, qualification, platform support, and release claims as
   distinct statements.
3. Create a durable readiness record containing a current conclusion,
   completed foundations, identified blockers and unblock conditions, scope
   decisions required before a freeze, work that can proceed without hardware,
   and the ordered resumption path after the splitter is available.
4. Treat the matched Airspy/RSP1B test, reference-suitability survey, and
   physical end-to-end calibration as separate gates. Do not transfer capture,
   synthetic, package, or same-host evidence into those cells.
5. Keep clean-host lifecycle, prior-version upgrade, Windows public trust,
   compatibility freeze, release documentation, and evidence-ledger completion
   explicit where they remain open.
6. Update documentation navigation so the prompt and readiness record are
   discoverable.

## Constraints and non-goals

- Do not enumerate, open, configure, or stream from an SDR; connect a splitter,
  antenna, or reference; or perform RF-dependent work.
- Do not rebuild or repackage artifacts, access signing/notarization services,
  install dependencies, alter a host, or perform clean-host simulations.
- Do not implement deferred production key management, trust-store transport,
  profile activation, WSJT-X mutation, or other product features.
- Do not freeze a revision, choose a release version, create final release
  notes, publish artifacts, create a tag, open a release, or change external
  services.
- Do not convert optional or policy-dependent features into release blockers
  without recording the scope decision that would make them required.

## Validation and adversarial review

Run Sphinx with warnings as errors and `git diff --check`. Review the result for
stale package names, obsolete statements that an exact package has not run,
same-host/clean-host conflation, self-signed/public-trust conflation, synthetic
or capture evidence promoted to physical accuracy, unsupported Ubuntu claims,
release freeze language, missing blocker dependencies, and circular exit
criteria. Correct every actionable finding and repeat the affected checks.

## Exit criteria

The retained readiness record answers what is complete, what blocks an initial
release candidate, what can proceed now, and what must wait. All corrected
claims cite existing durable evidence; no new implementation, package, device,
reference, calibration, or release claim is made; validation passes; and the
repository is ready for separately authorized commit and push.
