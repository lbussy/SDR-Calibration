#!/usr/bin/env python3
"""Prepare and validate the synthetic Microsoft Store certification fixture."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import shutil
import subprocess
import tempfile
from pathlib import Path


INPUTS = ("first.cf32", "second.cf32", "request.json", "trust.json")


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def run(command: list[str], **kwargs: object) -> subprocess.CompletedProcess[str]:
    return subprocess.run(command, check=False, capture_output=True, text=True, **kwargs)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-dir", required=True, type=Path)
    parser.add_argument("--fixture-test", required=True, type=Path)
    parser.add_argument("--sdrcal", required=True, type=Path)
    parser.add_argument("--output-dir", required=True, type=Path)
    parser.add_argument("--candidate-version", required=True)
    parser.add_argument("--development-tree", action="store_true")
    args = parser.parse_args()

    source = args.source_dir.resolve()
    output = args.output_dir.resolve()
    fixture_test = args.fixture_test.resolve()
    sdrcal = args.sdrcal.resolve()
    require(
        re.fullmatch(r"[0-9]+\.[0-9]+\.[0-9]+", args.candidate_version) is not None,
        "candidate version must contain exactly three numeric components",
    )
    require(fixture_test.is_file(), "fixture-generating production_cli_tests is missing")
    require(sdrcal.is_file(), "sdrcal executable is missing")
    require(not output.exists(), "refusing to reuse an existing fixture output directory")
    require(source != output and source not in output.parents,
            "fixture output directory must be outside the source tree")

    configured = (source / "CMakeLists.txt").read_text(encoding="utf-8")
    require(
        f"VERSION {args.candidate_version}" in configured,
        "candidate version does not match CMake project version",
    )
    version_result = run([str(sdrcal), "--version"])
    require(version_result.returncode == 0, "could not read sdrcal runtime version")
    require(
        version_result.stdout == f"sdrcal {args.candidate_version}\n" and not version_result.stderr,
        "sdrcal runtime version does not match candidate version",
    )
    revision_result = run(["git", "-C", str(source), "rev-parse", "HEAD"])
    require(revision_result.returncode == 0, "could not resolve source revision")
    revision = revision_result.stdout.strip()
    status_result = run(["git", "-C", str(source), "status", "--porcelain"])
    require(status_result.returncode == 0, "could not inspect source state")
    dirty = bool(status_result.stdout.strip())
    if not args.development_tree:
        require(not dirty, "refusing certification fixture preparation from a dirty tree")
        upstream_result = run(["git", "-C", str(source), "rev-parse", "@{upstream}"])
        require(upstream_result.returncode == 0, "source branch has no upstream")
        require(
            upstream_result.stdout.strip() == revision,
            "source revision is not synchronized with its upstream",
        )

    with tempfile.TemporaryDirectory(prefix="sdrcal-store-fixture-") as temporary:
        generated = Path(temporary) / "generated"
        environment = os.environ.copy()
        environment["SDRCAL_TEST_FIXTURE_DIR"] = str(generated)
        fixture_result = run([str(fixture_test)], env=environment)
        require(
            fixture_result.returncode == 0,
            f"fixture generator failed:\n{fixture_result.stdout}{fixture_result.stderr}",
        )
        for name in INPUTS:
            require((generated / name).is_file(), f"generated fixture input is missing: {name}")

        output.mkdir(parents=True)
        for name in INPUTS:
            shutil.copyfile(generated / name, output / name)
        request_path = output / "request.json"
        request = json.loads(request_path.read_text(encoding="utf-8"))
        request["software_version"] = args.candidate_version
        request_path.write_text(
            json.dumps(request, ensure_ascii=False, separators=(",", ":"), sort_keys=True) + "\n",
            encoding="utf-8",
        )

    validation_output = output / "validation-output"
    validation = run(
        [
            str(sdrcal),
            "calibrate",
            "--request",
            str(output / "request.json"),
            "--trust-file",
            str(output / "trust.json"),
            "--output-dir",
            str(validation_output),
        ]
    )
    require(
        validation.returncode == 0,
        f"generated fixture validation failed:\n{validation.stdout}{validation.stderr}",
    )
    require('"status":"success"' in validation.stdout, "fixture did not report success")
    shutil.rmtree(validation_output)

    hashes = []
    for name in INPUTS:
        digest = hashlib.sha256((output / name).read_bytes()).hexdigest()
        hashes.append(f"{digest}  {name}")
    (output / "SHA256SUMS").write_text("\n".join(hashes) + "\n", encoding="utf-8")
    manifest = {
        "schema_version": 1,
        "candidate_version": args.candidate_version,
        "source_revision": revision,
        "source_tree_clean": not dirty,
        "certification_ready": not dirty,
        "classification": "synthetic test fixture; no device or accuracy claim",
        "inputs": list(INPUTS),
    }
    (output / "manifest.json").write_text(
        json.dumps(manifest, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )
    (output / "README.txt").write_text(
        "SDR Calibration Microsoft Store certification fixture\n\n"
        "Synthetic recorded-input data only. No SDR hardware, RF observation, personal data, "
        "credential, or calibration-accuracy claim is present. Select request.json and trust.json "
        "in the GUI and choose a new output directory. The expected terminal state is success.\n",
        encoding="utf-8",
    )
    print(f"Prepared validated Store certification fixture: {output}")


if __name__ == "__main__":
    main()
