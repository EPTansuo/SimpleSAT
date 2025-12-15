#!/usr/bin/bash

python3 bench.py \
  --inputs ./cnf/UF250.1065.100 ./cnf/UUF250.1065.100   \
  --timeout 60 \
  --solver neuro:"../build/SimpleSAT -m CDCL {cnf}" \
  --solver minisat:"minisat -verb=0 {cnf}" \
  --solver kissat:"kissat {cnf} --relaxed" \
  --out ./out/ \
  --repeat 1
