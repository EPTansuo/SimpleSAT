#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import csv
from pathlib import Path
from typing import List, Tuple
import matplotlib.pyplot as plt
from matplotlib.ticker import StrMethodFormatter


def read_cactus_csv(path: Path) -> Tuple[List[int], List[float]]:
    solved, t = [], []
    with open(path, "r", encoding="utf-8") as f:
        r = csv.DictReader(f)
        for row in r:
            solved.append(int(row["solved"]))
            t.append(float(row["time_s"]))
    return solved, t


def default_label_from_name(p: Path) -> str:
    # cactus_kissat.csv -> kissat
    stem = p.stem
    if "_" in stem:
        return stem.split("_", 1)[1]
    return stem


def main():
    ap = argparse.ArgumentParser(description="Plot cactus curves with x=time, y=solved instances.")
    ap.add_argument("csvs", nargs="+", help="Input cactus CSV files, e.g. out/cactus_neuro.csv out/cactus_minisat.csv")
    ap.add_argument("--label", action="append", default=[], help="Labels (repeat in same order as csvs).")
    ap.add_argument("--out", default="", help="Output image path (png/pdf/svg). If empty, show window.")
    ap.add_argument("--timeout", type=float, default=None, help="Timeout (seconds). Draw vertical line and extend curves.")
    ap.add_argument("--title", default="", help="Plot title")
    ap.add_argument("--grid", action="store_true", help="Enable grid")

    # Style knobs
    ap.add_argument("--legend-outside", action="store_true", help="Put legend outside on the right (like SAT comp plots).")
    ap.add_argument("--markersize", type=float, default=4.0)
    ap.add_argument("--linewidth", type=float, default=1.5)

    args = ap.parse_args()

    paths = [Path(p) for p in args.csvs]
    labels = args.label[:]
    while len(labels) < len(paths):
        labels.append(default_label_from_name(paths[len(labels)]))

    markers = ["o", "^", "s", "D", "x", "*", "v", "P", "H", ">", "<", "1", "2", "3", "4"]

    plt.figure()

    for i, (p, lab) in enumerate(zip(paths, labels)):
        solved, t = read_cactus_csv(p)
        if not solved:
            print(f"[warn] empty: {p}")
            continue

        # Build x=time, y=solved, add (0,0)
        x = [0.0] + t
        y = [0] + solved

        # Extend to timeout so it reaches the right boundary like your screenshot
        if args.timeout is not None:
            last_solved = y[-1]
            x.append(float(args.timeout))
            y.append(int(last_solved))

        plt.plot(
            x, y,
            marker=markers[i % len(markers)],
            markersize=args.markersize,
            linewidth=args.linewidth,
            label=lab
        )

    plt.xlabel("CPU time")
    plt.ylabel("solved instances")
    if args.title:
        plt.title(args.title)

    # Comma ticks: 1,000 2,000 ...
    plt.gca().xaxis.set_major_formatter(StrMethodFormatter("{x:,.0f}"))

    if args.grid:
        plt.grid(True, linestyle="--", linewidth=0.7)

    # timeout vertical dashed line
    if args.timeout is not None:
        plt.axvline(args.timeout, linestyle="--", linewidth=1.0)

        plt.xlim(0, args.timeout)

    if args.legend_outside:
        plt.legend(loc="center left", bbox_to_anchor=(1.02, 0.5), frameon=True)
        plt.tight_layout(rect=[0.0, 0.0, 0.99, 1.0])
    else:
        plt.legend(frameon=True)
        plt.tight_layout()

   
    
    ax = plt.gca()
    mx = ax.get_xlim()[1]
    if mx < 1:
        ax.xaxis.set_major_formatter(StrMethodFormatter("{x:.3f}"))
    elif mx < 10:
        ax.xaxis.set_major_formatter(StrMethodFormatter("{x:.2f}"))
    else:
        ax.xaxis.set_major_formatter(StrMethodFormatter("{x:,.0f}"))


    if args.out:
        outp = Path(args.out)
        outp.parent.mkdir(parents=True, exist_ok=True)
        plt.savefig(outp, dpi=200)
        print(f"[saved] {outp}")
    else:
        plt.show()


if __name__ == "__main__":
    main()
