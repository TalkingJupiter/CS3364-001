# Ranking Reliability Report

- Sources: 5
- Total unique items: 9938
- Max inversions for N items: 49376953

## Methodology
We computed a **combined ranking** by summing per-source ranks (lower sum = better). For each source, we mapped its order to the combined order and counted inversions using two authoritative methods (Merge sort and Fenwick/BIT). A quicksort-style method is included for **diagnostic** insight only.

A **reliability score** is defined as `1 - (inversions / max_inversions)` ∈ [0,1]. Higher means closer to the consensus.

## Results (Merge-based)
| Source | n | Inversions (merge) | Reliability |
|---|---:|---:|---:|
| source1.txt | 10000 | 20663210 | 0.581521 |
| source2.txt | 10000 | 20483100 | 0.585169 |
| source3.txt | 10000 | 20540638 | 0.584004 |
| source4.txt | 10000 | 20338069 | 0.588106 |
| source5.txt | 10000 | 20559713 | 0.583617 |

_The quick partition counter is diagnostic and may differ; see `inversions_summary.csv` for all counters._
