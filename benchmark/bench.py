#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import csv
import json
import os
import shlex
import subprocess
import time
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Optional, Tuple

# ----------------------------
# Result parsing (minisat-like / generic)
# ----------------------------
def classify_output(stdout: str, stderr: str, returncode: int) -> str:
    """
    Return one of: sat / unsat / unknown / error
    """
    out = (stdout + "\n" + stderr).upper()

    # Minisat-style
    if "UNSATISFIABLE" in out or "UNSAT" in out:
        return "unsat"
    if "SATISFIABLE" in out or "\nSAT\n" in out or out.strip() == "SAT":
        return "sat"

    # Some solvers use exit codes
    if returncode in (10,):
        return "sat"
    if returncode in (20,):
        return "unsat"

    if returncode == 0:
        return "unknown"
    return "error"


def run_one(cmd_template: str, cnf_path: Path, timeout_s: float) -> Tuple[str, float, str]:
    """
    Returns: (status, elapsed_seconds, raw_tag)
      status: solved/unsolved/error
      raw_tag: sat/unsat/unknown/timeout/error
    """
    # Replace placeholder
    cmd_str = cmd_template.replace("{cnf}", str(cnf_path))
    argv = shlex.split(cmd_str)

    t0 = time.perf_counter()
    try:
        cp = subprocess.run(
            argv,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            timeout=timeout_s,
        )
        elapsed = time.perf_counter() - t0
        tag = classify_output(cp.stdout, cp.stderr, cp.returncode)
        if tag in ("sat", "unsat"):
            return "solved", elapsed, tag
        elif tag == "unknown":
            return "unsolved", elapsed, tag
        else:
            return "error", elapsed, tag
    except subprocess.TimeoutExpired as e:
        elapsed = time.perf_counter() - t0
        return "unsolved", elapsed, "timeout"
    except FileNotFoundError:
        return "error", 0.0, "cmd_not_found"
    except Exception:
        elapsed = time.perf_counter() - t0
        return "error", elapsed, "exception"


@dataclass
class SolverCfg:
    name: str
    cmd: str


def collect_cnfs(paths: List[str], exts=(".cnf", ".dimacs")) -> List[Path]:
    files: List[Path] = []
    for p in paths:
        pp = Path(p)
        if pp.is_dir():
            for ext in exts:
                files.extend(pp.rglob(f"*{ext}"))
        else:
            # support glob
            matched = list(Path().glob(p))
            if matched:
                for m in matched:
                    if m.is_dir():
                        for ext in exts:
                            files.extend(m.rglob(f"*{ext}"))
                    else:
                        files.append(m)
            else:
                files.append(pp)
    # unique & sort for determinism
    files = sorted({f.resolve() for f in files if f.exists() and f.is_file()})
    return files


def main():
    ap = argparse.ArgumentParser(
        description="Benchmark solvers on CNF set and dump cactus-plot data (no plotting)."
    )
    ap.add_argument(
        "--inputs",
        nargs="+",
        required=True,
        help="CNF files/dirs/globs. Examples: ./test/aim  ./bench/*.cnf",
    )
    ap.add_argument(
        "--solver",
        action="append",
        required=True,
        help='Solver spec: NAME:CMD  (CMD must contain {cnf}). Example: minisat:"minisat -verb=0 {cnf} /dev/null"',
    )
    ap.add_argument("--timeout", type=float, default=60.0, help="Timeout per instance (seconds).")
    ap.add_argument("--out", default="out/", help="Output prefix (directory/prefix).")
    ap.add_argument("--repeat", type=int, default=1, help="Repeat each instance N times and take min time.")
    args = ap.parse_args()

    solvers: List[SolverCfg] = []
    for s in args.solver:
        if ":" not in s:
            raise SystemExit(f"--solver format must be NAME:CMD, got: {s}")
        name, cmd = s.split(":", 1)
        name = name.strip()
        cmd = cmd.strip().strip('"').strip("'")
        if "{cnf}" not in cmd:
            raise SystemExit(f"Solver CMD must contain {{cnf}} placeholder, got: {cmd}")
        solvers.append(SolverCfg(name=name, cmd=cmd))

    cnfs = collect_cnfs(args.inputs)
    if not cnfs:
        raise SystemExit("No CNF files found.")

    out_prefix = None
    if(args.out.endswith("/") or args.out.endswith("\\")):
        out_prefix = Path(args.out)
        out_prefix.mkdir(parents=True, exist_ok=True)
    else: 
        out_prefix = Path(args.out)
        out_prefix.parent.mkdir(parents=True, exist_ok=True)
    
    

    full_report: Dict[str, Dict] = {
        "timeout": args.timeout,
        "repeat": args.repeat,
        "num_instances": len(cnfs),
        "instances": [str(p) for p in cnfs],
        "solvers": {},
    }

    # Run benchmarks
    for solver in solvers:
        per_inst = []
        solved_times: List[float] = []
        solved_count = 0
        unsolved_count = 0
        error_count = 0

        for cnf in cnfs:
            best = None  # best elapsed among repeats (min)
            best_tag = "unknown"
            best_status = "unsolved"

            for _ in range(max(1, args.repeat)):
                status, elapsed, tag = run_one(solver.cmd, cnf, args.timeout)
                if best is None or elapsed < best:
                    best = elapsed
                    best_tag = tag
                    best_status = status

            assert best is not None
            per_inst.append(
                {
                    "cnf": str(cnf),
                    "status": best_status,
                    "tag": best_tag,
                    "time_s": best,
                }
            )

            if best_status == "solved":
                solved_count += 1
                solved_times.append(best)
            elif best_status == "unsolved":
                unsolved_count += 1
            else:
                error_count += 1

        solved_times.sort()

        # cactus data: x=1..k, y=sorted_time[x-1]
        cactus_rows = [{"solved": i + 1, "time_s": t} for i, t in enumerate(solved_times)]

        # write cactus csv
        if(args.out.endswith("/") or args.out.endswith("\\")):
            csv_path = out_prefix / f"{solver.name}.csv"
        else:
            csv_path = out_prefix.parent / f"{out_prefix.name}_{solver.name}.csv"
        with open(csv_path, "w", newline="", encoding="utf-8") as f:
            w = csv.DictWriter(f, fieldnames=["solved", "time_s"])
            w.writeheader()
            w.writerows(cactus_rows)

        # write per-instance json
        if(args.out.endswith("/") or args.out.endswith("\\")):
            per_path = out_prefix / f"{solver.name}_per_instance.json"
        else:
            per_path = out_prefix.parent / f"{out_prefix.name}_{solver.name}_per_instance.json"
        with open(per_path, "w", encoding="utf-8") as f:
            json.dump(per_inst, f, indent=2, ensure_ascii=False)

        full_report["solvers"][solver.name] = {
            "cmd": solver.cmd,
            "solved": solved_count,
            "unsolved": unsolved_count,
            "error": error_count,
            "cactus_csv": str(csv_path),
            "per_instance_json": str(per_path),
        }

        print(f"[{solver.name}] solved={solved_count}  unsolved={unsolved_count}  error={error_count}")
        print(f"  cactus: {csv_path}")
        print(f"  per-inst: {per_path}")

    if (args.out.endswith("/") or args.out.endswith("\\")):
        summary_path = out_prefix / "summary.json"
    else:
        summary_path = out_prefix.parent / f"{out_prefix.name}_summary.json"
    with open(summary_path, "w", encoding="utf-8") as f:
        json.dump(full_report, f, indent=2, ensure_ascii=False)
    print(f"[summary] {summary_path}")


if __name__ == "__main__":
    main()
