# Microsoft Store screenshot-upload execution prompt

## Objective

Upload only the four retained, privacy-reviewed, exact-candidate screenshots to
the Desktop English (United States) Store listing and verify the persisted
listing state without selecting any package or requesting certification or
publication.

## Verified context

- Submission 1 is `In draft`; Store listings and Packages are `Incomplete`.
- All approved listing text is saved. Desktop has zero screenshots and requires
  at least one.
- The four approved PNGs and their manifest are retained under
  `evidence/windows-store/2026-08-17-0.1.1-screenshots/`.
- The screenshot manifest binds revision `957fbeb204177c9ba2a1582e936476244b201b9d`,
  version `0.1.1`, and MSIX SHA-256
  `1d9828710dcec5c93862e217606a92f53c1470b5abb412a23725ebc811b1edc1`.

## Scope

1. Recompute each PNG SHA-256 and verify its dimensions, filename, privacy
   disposition, and exact-candidate binding against `manifest.json`.
2. Re-open the English (United States) listing and confirm Desktop initially
   contains no screenshots.
3. Select exactly the four committed PNG paths in manifest order.
4. If Partner Center exposes captions or accessibility descriptions, use only
   short factual descriptions of the visible synthetic workflow; otherwise do
   not invent metadata.
5. Save, re-open, and verify all four Desktop screenshots persist in the
   intended order and the previously saved listing text remains present.
6. Record Partner Center's resulting Store-listing and submission status.

## Constraints and non-goals

- Do not select or upload the MSIX, certification fixture, logos, trailers,
  promotional art, Xbox assets, or any other file.
- Do not edit product name, pricing, availability, Properties, age ratings,
  package device families, listing copy, or submission options.
- Do not select **Submit for certification**, publish, or enable automatic
  publication.
- Refuse any screenshot whose current hash or dimensions differ from the
  committed manifest.
- Do not imply Microsoft certification, device qualification, live SDR support,
  accuracy, telemetry, or RF functionality.

## Validation and evidence

- Verify the selected filenames and count are exactly the four manifest entries.
- Verify the Desktop screenshot count changes from zero to four and remains four
  after a reload.
- Verify no Xbox or optional asset count changes and no package is uploaded.
- Verify **Submit for certification** remains disabled unless all unrelated
  gates happen to be satisfied; never select it in this slice.
- Run the Store contract verifier, targeted Store contract test, strict Sphinx
  build, and `git diff --check`.

## Adversarial review

Inspect for wrong-file selection, reordered images, duplicated screenshots,
privacy leakage, unsupported claims, stale `not uploaded` language, package or
fixture interaction, and any accidental certification/publication action.
Correct every actionable finding and reassess until clean.

## Exit criteria

- Exactly four approved Desktop screenshots are saved in manifest order.
- No other file or Partner Center section is changed.
- Remaining package, fixture, certification, and publication gates remain
  explicit and separately authorized.
- Repository evidence and validation agree with observed Partner Center state.
