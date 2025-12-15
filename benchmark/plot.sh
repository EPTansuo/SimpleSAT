#!/usr/bin/bash

python3 plot_time.py \
  out/neuro.csv \
  out/minisat.csv \
  out/kissat.csv \
  --grid


python3 plot_solved.py \
  out/neuro.csv \
  out/minisat.csv \
  out/kissat.csv \
  --grid
