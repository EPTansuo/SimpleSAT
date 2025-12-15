#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import csv
from pathlib import Path
from typing import List, Tuple

import matplotlib.pyplot as plt


def read_cactus_csv(path: Path) -> Tuple[List[int], List[float]]:
    xs, ys = [], []
    with open(path, "r", encoding="utf-8") as f:
        r = csv.DictReader(f)
        for row in r:
            xs.append(int(row["solved"]))
            ys.append(float(row["time_s"]))
    return xs, ys


def default_label_from_name(p: Path) -> str:
    # cactus_neuro.csv -> neuro
    stem = p.stem
    if "_" in stem:
        return stem.split("_")[-1]
    return stem


def main():
    ap = argparse.ArgumentParser(description="Plot cactus curves from cactus CSV files.")
    ap.add_argument("csvs", nargs="+", help="Input cactus CSV files, e.g. out/cactus_neuro.csv out/cactus_minisat.csv")
    ap.add_argument("--label", action="append", default=[], help="Label(s) in same order as csvs. Can repeat.")
    ap.add_argument("--out", default="", help="Output image path (png/pdf/svg). If empty, show window.")
    ap.add_argument("--title", default="", help="Plot title")
    ap.add_argument("--xlabel", default="Problems solved", help="X label")
    ap.add_argument("--ylabel", default="Time (s)", help="Y label")
    ap.add_argument("--grid", action="store_true", help="Enable grid")
    ap.add_argument("--logy", action="store_true", help="Use log scale on Y axis")
    ap.add_argument("--xlim", nargs=2, type=float, default=None, help="xlim min max")
    ap.add_argument("--ylim", nargs=2, type=float, default=None, help="ylim min max")
    args = ap.parse_args()

    paths = [Path(p) for p in args.csvs]
    labels = args.label[:]
    while len(labels) < len(paths):
        labels.append(default_label_from_name(paths[len(labels)]))

    # Marker cycle (optional, just for readability)
    markers = ["o", "^", "s", "D", "x", "*", "v", "P", "H"]

    plt.figure()
    for i, (p, lab) in enumerate(zip(paths, labels)):
        x, y = read_cactus_csv(p)
        if not x:
            print(f"[warn] empty data: {p}")
            continue
        plt.plot(x, y, marker=markers[i % len(markers)], markersize=4, linewidth=1.5, label=lab)

    plt.xlabel(args.xlabel)
    plt.ylabel(args.ylabel)
    if args.title:
        plt.title(args.title)

    if args.grid:
        plt.grid(True, linestyle="--", linewidth=0.7)

    if args.logy:
        plt.yscale("log")

    if args.xlim is not None:
        plt.xlim(args.xlim[0], args.xlim[1])
    if args.ylim is not None:
        plt.ylim(args.ylim[0], args.ylim[1])

    plt.legend()
    plt.tight_layout()

    if args.out:
        outp = Path(args.out)
        outp.parent.mkdir(parents=True, exist_ok=True)
        plt.savefig(outp, dpi=200)
        print(f"[saved] {outp}")
    else:
        plt.show()


if __name__ == "__main__":
    main()
