#!/usr/bin/env python3
"""Format handwritten C/C++ sources with clang-format.

Excludes: build/, _deps/, *.pb.* (see .clang-format-ignore).
Usage (from repo root):
  python scripts/format_source.py
  python scripts/format_source.py --check
"""

from __future__ import annotations

import argparse
import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
EXTS = {".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".hxx"}


def find_clang_format() -> Path | None:
    env = os.environ.get("CLANG_FORMAT")
    if env:
        p = Path(env)
        if p.is_file():
            return p
    for candidate in (
        Path(r"C:\Program Files\LLVM\bin\clang-format.exe"),
        Path(r"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\Llvm\x64\bin\clang-format.exe"),
    ):
        if candidate.is_file():
            return candidate
    from shutil import which

    w = which("clang-format")
    return Path(w) if w else None


def is_excluded(rel: Path) -> bool:
    parts = {p.lower() for p in rel.parts}
    if "build" in parts or "_deps" in parts:
        return True
    name = rel.name.lower()
    if ".pb." in name or name.endswith(".pb.cc") or name.endswith(".pb.h"):
        return True
    return False


def collect_sources() -> list[Path]:
    out: list[Path] = []
    for path in ROOT.rglob("*"):
        if not path.is_file() or path.suffix.lower() not in EXTS:
            continue
        try:
            rel = path.relative_to(ROOT)
        except ValueError:
            continue
        if is_excluded(rel):
            continue
        out.append(path)
    return sorted(out)


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument(
        "--check",
        action="store_true",
        help="Only check formatting (non-zero if changes needed)",
    )
    args = ap.parse_args()

    cf = find_clang_format()
    if not cf:
        print("error: clang-format not found; set CLANG_FORMAT or install LLVM", file=sys.stderr)
        return 2

    files = collect_sources()
    if not files:
        print("no sources found")
        return 0

    mode = ["--dry-run", "--Werror"] if args.check else ["-i"]
    # Batch to avoid command-line length limits on Windows.
    batch = 40
    rc = 0
    for i in range(0, len(files), batch):
        chunk = files[i : i + batch]
        cmd = [str(cf), "-style=file", *mode, *[str(p) for p in chunk]]
        print(f"+ clang-format ({len(chunk)} files){' --check' if args.check else ''}", flush=True)
        r = subprocess.run(cmd, cwd=ROOT)
        if r.returncode != 0:
            rc = r.returncode
            if args.check:
                return rc
    print(f"OK: {'check passed' if args.check else 'formatted'} {len(files)} files")
    return rc


if __name__ == "__main__":
    sys.exit(main())
