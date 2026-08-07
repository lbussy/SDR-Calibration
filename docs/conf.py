import os
import re
import subprocess

project = "SDR Calibration"
copyright = "2026, Lee Bussy and contributors"
author = "Lee Bussy"


def _git_describe():
    try:
        return subprocess.check_output(
            ["git", "describe", "--tags", "--always"],
            stderr=subprocess.DEVNULL,
            text=True,
        ).strip()
    except Exception:
        return ""


_rtd_version = os.environ.get("READTHEDOCS_VERSION", "").strip()
_rtd_identifier = os.environ.get("READTHEDOCS_GIT_IDENTIFIER", "").strip()
if re.fullmatch(r"[0-9a-fA-F]{7,40}", _rtd_identifier):
    _rtd_identifier = _rtd_identifier[:7]

if _rtd_version == "latest":
    version = "main"
    release = f"main-{_rtd_identifier}" if _rtd_identifier else "main"
elif _rtd_version == "stable":
    version = "stable"
    release = _git_describe() or "stable"
else:
    version = _rtd_version or "development"
    release = _rtd_identifier or _git_describe() or version

extensions = ["myst_parser", "sphinx.ext.todo"]
source_suffix = ".md"
master_doc = "index"
language = "en"
exclude_patterns = [
    "_build",
    ".venv/**",
    "Thumbs.db",
    ".DS_Store",
]

myst_enable_extensions = ["colon_fence", "deflist", "tasklist"]
myst_heading_anchors = 3

html_theme = "sphinx_rtd_theme"
html_theme_options = {
    "collapse_navigation": False,
    "navigation_depth": 4,
    "sticky_navigation": True,
    "style_external_links": True,
    "titles_only": False,
}

todo_include_todos = True
