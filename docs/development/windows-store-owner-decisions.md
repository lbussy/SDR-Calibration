# Microsoft Store owner decision packet

Status: **Pending explicit owner approval**

Prepared: 2026-08-17

## Purpose and boundary

This packet records the decisions the product owner must make before anyone
enters submission data in Partner Center. Preparing or committing this page is
not approval of any proposed value. It does not authorize starting a
submission, changing Partner Center state, uploading a package or attachment,
requesting certification, or publishing.

The packet is bound to:

- application version `0.1.1`;
- source revision `957fbeb204177c9ba2a1582e936476244b201b9d`; and
- unsigned Store MSIX SHA-256
  `1d9828710dcec5c93862e217606a92f53c1470b5abb412a23725ebc811b1edc1`.

If any binding changes, every decision below returns to pending.

## Decision worksheet

`Recommended` describes the prepared submission plan. `Pending` means the owner
has not approved that value in this record.

| Decision | Recommended value or rule | Approval state |
| --- | --- | --- |
| Base price | Free | Pending |
| Markets | All markets Partner Center permits for the account and truthful declarations | Pending |
| Audience | Public | Pending |
| Discoverability | Available and discoverable | Pending |
| Category | Utilities & tools, if that exact option is visible | Pending |
| Subcategory | None unless Partner Center requires one | Pending |
| Listing language | English (United States) only for the first submission | Pending |
| Website | Omit unless the owner selects an owner-controlled product URL | Pending |
| Support channel | Enter an owner-controlled URL or email directly in Partner Center; do not commit it here | Pending |
| Privacy answer | Preliminary technical assessment is No; owner must reconcile the exact visible question, markets, law, and any support or crash-reporting behavior | Pending |
| Privacy policy | Omit only if the reconciled Partner Center flow and applicable obligations permit it; otherwise provide an owner-controlled truthful policy | Pending |
| Age rating | Answer every visible question from the exact implemented offline, recorded-input-only behavior; do not copy the preparation summary mechanically | Pending |
| Publishing schedule | Manual publication hold; no automatic publication | Pending |
| Certification submission | Separate explicit authorization required after all preflight evidence and visible fields are independently reconciled | Pending |

## Technical facts available to the owner

- The Store build is offline and recorded-input-only.
- It implements no project telemetry, advertising, account, cloud service, or
  network transport.
- It declares only `runFullTrust`, for the packaged Win32 Qt GUI and
  `sdrcal.exe` execution alias at normal user integrity.
- It does not install SDR drivers, access live SDR hardware, transmit RF,
  request elevation, install services, or modify machine registry state.
- WACK passed for the exact candidate; this does not replace Store
  certification or an owner privacy/legal determination.
- Four exact-candidate screenshots and the deterministic certification fixture
  are retained and privacy-reviewed but have not been uploaded.

## Owner attestation gate

All boxes must be explicitly completed by the owner before the owner-decision
gate in the submission-readiness packet can pass:

- [ ] I approve the price, markets, audience, and discoverability selections.
- [ ] I approve the category, language, website disposition, and the existence
      of an owner-controlled support channel entered outside the repository.
- [ ] I reviewed the exact Partner Center privacy wording and approve the
      answer and any required policy disposition.
- [ ] I reviewed and answered every exact visible age-rating question based on
      implemented behavior.
- [ ] I require a manual publication hold and understand certification success
      must not automatically publish the product.
- [ ] I confirm the candidate bindings above still match the intended upload.

Record the approval date and approving owner only when every box is complete:

- Approval date (UTC): **Pending**
- Approving owner: **Pending**

Do not record an email address, phone number, postal address, account
credential, notification recipient, or verification material in this file.

## Exit criteria

This packet becomes approved only when every attestation is checked, both final
approval fields are no longer `Pending`, and the candidate bindings remain
exact. Even then, Partner Center entry and upload remain a separate authorized
slice, and **Submit for certification** remains a later explicit authorization.
