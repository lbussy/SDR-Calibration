# Security policy

## Supported versions

SDR Calibration has no released application version yet. The `main` branch
receives best-effort security maintenance during development, but it is not a
stable release and carries no supported-version or remediation guarantee.

After the first release, only the latest stable release is supported unless a
release-specific notice explicitly extends support. A release becomes
unsupported when its successor is published unless a security advisory states
otherwise.

| Version | Supported |
| --- | --- |
| Latest stable release | Yes, after the first release |
| Earlier releases | No, unless a security advisory says otherwise |
| Unreleased `main` | Best effort; not a stable-version guarantee |

## Privately report a vulnerability

Use GitHub's private **Report a vulnerability** workflow:

[Privately report an SDR Calibration vulnerability](https://github.com/lbussy/SDR-Calibration/security/advisories/new)

Do not disclose a suspected vulnerability, exploit, sensitive reproduction
details, or affected-user information in a public issue. If GitHub's private
form is unavailable, open a public issue containing no sensitive details and
ask the maintainer to establish private contact; do not include the report
itself.

Please include only what is necessary:

- affected version or full source revision and artifact SHA-256;
- operating system and architecture;
- affected device or driver when relevant;
- concise reproduction steps, impact, and any known mitigation;
- whether credentials, profiles, evidence, private identifiers, or safety-
  relevant behavior may be exposed; and
- a safe way to coordinate larger or sensitive supporting files.

Never include authentication tokens, passwords, signing or private keys,
unnecessary personal information, unrelated device identifiers, or live
credentials. Do not attach raw IQ, large evidence bundles, or encrypted files
until the maintainer confirms the minimum required material and transfer method.

## Response and disclosure policy

Repository owners and administrators triage private reports. The current
repository has one designated primary maintainer and no independent backup
security maintainer; this single-maintainer limitation remains explicit.

Targets, not guarantees:

- acknowledge a complete report within three business days;
- provide an initial assessment or request for needed information within ten
  business days; and
- coordinate remediation and disclosure timing according to severity,
  exploitability, affected users, fix complexity, and reporter needs.

The project will keep the report private while investigating, minimize access
to sensitive evidence, and avoid public disclosure before a mitigation is
available unless active exploitation or user safety requires earlier notice.
The maintainer and reporter will coordinate credit; anonymous credit is
available. The maintainer decides whether to request a CVE based on the
validated impact and release exposure.

Sensitive submissions are retained only as long as needed for investigation,
remediation, evidence obligations, and coordinated disclosure. Unneeded local
copies are deleted after closure when safe and lawful; GitHub's advisory audit
record remains subject to GitHub retention and repository-administration
controls.
