#!/usr/bin/env python3
"""Run every loadable PUD map through War25's real map-switch lifecycle."""

from __future__ import annotations

import argparse
import concurrent.futures
import os
from pathlib import Path
import subprocess
import sys
import threading


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--executable", required=True, type=Path)
    parser.add_argument("--maps-root", required=True, type=Path)
    parser.add_argument("--jobs", type=int, default=min(4, os.cpu_count() or 1))
    parser.add_argument("--ticks", type=int, default=40)
    parser.add_argument("--timeout", type=float, default=20.0)
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    executable = args.executable.resolve()
    maps_root = args.maps_root.resolve()
    if not executable.is_file():
        print(f"Executable not found: {executable}", file=sys.stderr)
        return 2
    if not maps_root.is_dir():
        print(f"Maps directory not found: {maps_root}", file=sys.stderr)
        return 2

    candidates = sorted(
        path for path in maps_root.rglob("*")
        if path.is_file() and path.suffix.lower() == ".pud"
    )
    maps = [path for path in candidates if path.stat().st_size >= 8]
    skipped = [path for path in candidates if path.stat().st_size < 8]
    print(f"Testing {len(maps)} maps with {args.ticks} ticks each ({args.jobs} jobs).")
    for path in skipped:
        print(f"Skipping invalid empty PUD: {path.relative_to(maps_root)}")

    environment = os.environ.copy()
    environment.setdefault("SDL_AUDIODRIVER", "dummy")
    completed = 0
    progress_lock = threading.Lock()

    def run_map(path: Path) -> tuple[Path, int, str]:
        nonlocal completed
        command = [
            str(executable),
            "--smoke-test-map",
            str(path),
            str(max(1, args.ticks)),
        ]
        try:
            result = subprocess.run(
                command,
                cwd=executable.parent,
                env=environment,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                timeout=args.timeout,
                check=False,
            )
            status, output = result.returncode, result.stdout
        except subprocess.TimeoutExpired as error:
            status = 124
            partial_output = error.stdout or ""
            if isinstance(partial_output, bytes):
                partial_output = partial_output.decode(errors="replace")
            output = partial_output + "\nTimed out"

        with progress_lock:
            completed += 1
            if completed % 100 == 0 or completed == len(maps):
                print(f"Progress: {completed}/{len(maps)}", flush=True)
        return path, status, output

    failures: list[tuple[Path, int, str]] = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=max(1, args.jobs)) as pool:
        for path, status, output in pool.map(run_map, maps):
            if status != 0:
                failures.append((path, status, output))

    if failures:
        print(f"\n{len(failures)} map(s) failed:", file=sys.stderr)
        for path, status, output in failures:
            print(f"\n[{status}] {path.relative_to(maps_root)}", file=sys.stderr)
            print("\n".join(output.splitlines()[-30:]), file=sys.stderr)
        return 1

    print(f"All {len(maps)} loadable maps passed. Skipped {len(skipped)} invalid placeholder(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
