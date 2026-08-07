# Repository layout

The repository separates build policy, source, tests, machine contracts, and
rendered documentation inputs.

```text
SDR-Calibration/
├── CMakeLists.txt
├── CMakePresets.json
├── src/
│   ├── core/
│   ├── profile/
│   ├── soapy/
│   ├── application/
│   ├── adapters/wsjtx/
│   ├── cli/
│   └── gui/
├── tests/
│   ├── unit/
│   ├── integration/
│   └── fixtures/
├── schemas/
└── docs/
    ├── user-guide/
    └── development/
```

Directories are created when their first substantive file is introduced;
placeholder source files are not required merely to preserve empty directories.

## Root files

Root files define project-wide contracts:

- `LICENSE` covers original project source and documentation.
- `THIRD_PARTY_NOTICES.md` records external licensing boundaries.
- `AGENTS.md` defines repository working rules.
- `CONTRIBUTING.md` defines the contributor workflow.
- `.editorconfig`, `.clang-format`, and `.clang-tidy` define source policy.
- `.gitattributes` defines text normalization and binary files.
- `.gitignore` excludes only relevant generated and local state.
- `.readthedocs.yaml` configures hosted documentation builds.

## Source ownership

`src/core` owns calibration algorithms and normalized domain types. It cannot
depend on outward adapters.

`src/profile` owns the durable profile representation, schema-facing
serialization, and consumer-side evaluation.

`src/soapy` converts SoapySDR capabilities and streams into normalized internal
interfaces. Vendor-specific behavior remains explicit here.

`src/application` coordinates workflows without implementing GUI widgets or DSP
algorithms.

`src/adapters` owns lossy external formats. `src/cli` and `src/gui` are thin
delivery surfaces over application services.

## Documentation ownership

`docs/user-guide` describes released operator behavior. Planned behavior must be
identified as planned.

`docs/development` contains architecture, contracts, decisions, testing, and
release policy. Normative machine schemas remain in `schemas/` and are linked
from the documentation rather than duplicated.
