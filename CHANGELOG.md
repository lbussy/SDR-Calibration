# Changelog

All notable released changes will be documented in this file.

The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and releases will use semantic versioning once the public application and
artifact compatibility policy are established.

## [Unreleased]

### Added

- Phase 14.1 signed and notarized macOS DMG production with dynamic Qt
  deployment, Gatekeeper verification, and hash-indexed evidence.
- Phase 14.2 signed Windows x64 MSI production tooling with dynamic Qt,
  Authenticode timestamp verification, and extracted-payload evidence.
- Phase 14.3 Ubuntu 24.04 x86_64 DEB production tooling with Debian-derived
  system dependencies and extracted-payload/runtime evidence.
- Phase 14.4 exact-payload license disposition with accompanying hash-pinned Qt
  source on macOS/Windows and exact system-package copyright evidence on Ubuntu.
- Initial project, documentation, schema, licensing, and build scaffold.
- Portable install/archive packaging, configured SPDX dependency inventory,
  staged-package auditing, cross-platform hardware-free CI definitions, and
  evidence-indexed platform/device and end-to-end qualification gates.
