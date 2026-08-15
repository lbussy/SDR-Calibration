# 0018 — Exact payload license disposition

Status: Accepted

Date: 2026-08-15

## Decision

Bind binary-license disposition to the exact staged payload. macOS and Windows
packages that convey dynamic Qt must also convey the complete qtbase source
archive for the deployed version, verify a release-input SHA-256, retain the
actual runtime inventory, state the no-modification disposition, and provide
practical shared-library replacement instructions. A URL or configured SPDX
entry is not a substitute for corresponding source.

Do not deploy the Windows compiler runtime or software OpenGL library in this
package. Reject any deployed DLL that is neither project code nor in the Qt
library/plug-in naming family so a new third-party payload reopens review rather
than inheriting Qt's disposition.

The Ubuntu DEB conveys no Qt library. Resolve every generated runtime dependency
clause to an installed package and version and require its Debian copyright
record, retaining that mapping inside the DEB and as package evidence.

## Consequences

The source archive increases macOS and Windows artifact size but makes source
availability travel with the binary and avoids a mutable external-source gate.
Every artifact must repeat the audit; evidence does not transfer across payloads
or Qt versions. This engineering disposition does not replace legal review and
does not establish clean installation, platform support, device qualification,
calibration accuracy, or release publication.
