# Windows Store certification fixture attachment

`SDRCalibration-0.1.1-Store-Certification-Fixture.zip` is the proposed
supplemental attachment for Microsoft certification testing of the exact Store
candidate. It is 6,063 bytes with SHA-256
`f11547cdbbedba715074d55413026a86ac5209597aeb42f8712e7527c7b6ff51`.

The archive contains the certification-ready fixture generated on Windows from
a clean synchronized tree at
`2e678b8d0e2869d79626c80511398c809d8039b7`. Its request is explicitly bound to
application version `0.1.1` and was subsequently exercised successfully by the
exact staged GUI payload associated with Store candidate revision
`957fbeb204177c9ba2a1582e936476244b201b9d` and MSIX SHA-256
`1d9828710dcec5c93862e217606a92f53c1470b5abb412a23725ebc811b1edc1`.

The fixture contains two small synthetic CF32 inputs, one recorded-input
request, one trust-pin file, its generation manifest, usage instructions, and
payload hashes. Independent review found no personal data, credentials, private
keys, device secrets, real device identity, sensitive raw-IQ material, or
calibration-accuracy claim. The checksum file was normalized from Windows CRLF
to LF when the deterministic ZIP was assembled so `shasum -c SHA256SUMS` works
after extraction on both Windows-compatible and Unix review hosts; payload
bytes and their hashes were not changed.

The ZIP uses fixed `2026-08-17 00:00` member timestamps and excludes platform
metadata. Extraction, all four payload hashes, and byte-for-byte comparison
against the retained Windows fixture passed. No attachment was uploaded and no
Partner Center state was changed in this slice.
