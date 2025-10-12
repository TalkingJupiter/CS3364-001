# Ranking Reliability — Project 1

**Consensus ranking + inversion-based reliability** using three inversion-counting methods.  

This project reads multiple ranking lists, builds a combined consensus order, and evaluates the reliability of each source compared to the consensus. Reliability is measured via **inversion counts** (disagreement in orderings).

---

## Features
- **Consensus ranking** by sum of ranks (lower is better, average as tie-breaker).  
- **Inversion counting** via three independent algorithms:
  - **Merge Sort** (O(n log n), authoritative).  
  - **Fenwick Tree / BIT** (O(n log n), authoritative).  
  - **Quick Partition** (O(n log n) expected, diagnostic).  
- **Reliability score** = `1 - inversions / max_inversions` ∈ [0,1].  
- **Reports** in both CSV and Markdown.

---

## Inputs
- One or more **source files** (`.txt`), each containing a newline-separated ranked list of items.  
- Items can be strings, numbers, or IDs. Identical lines are treated as the same item.

Example (`source1.txt`):
A
B
C

---

## Outputs
All results are written under the output directory (`--out OUT_DIR`):

- combined_order.csv        → Final consensus order  
- inversions_summary.csv    → Per source: inversions (3 methods) + reliability  
- <source>_positions.csv    → Mapping of each source’s items to consensus positions  
- report.md                 → Human-readable summary (Markdown)  

---

##  Usage

### Build
    make

### Run
    ./rank_reliability --out out_rank source1.txt source2.txt source3.txt

### Example
    ./rank_reliability --out results data/source1.txt data/source2.txt data/source3.txt

---

## Reliability Definition
- Let N = number of unique items.  
- Max possible inversions = N*(N-1)/2.  
- Reliability of a source:
    reliability = 1 - (inversions / max_inversions)

- Score near 1.0 → close to consensus.  
- Score near 0.0 → highly inconsistent with consensus.  

---

## My Methodology
1. Read all sources → build universe of items.  
2. Assign ranks (missing items get worst rank).  
3. Compute consensus ranking by total rank sum.  
4. For each source:
   - Map to consensus order.  
   - Count inversions (3 methods).  
   - Compute reliability score.  
5. Write outputs (CSV + Markdown).  

---

## Development Tools
- Language: C++17  
- Build: make (uses g++/clang++)  
- Dependencies: STL only (no external libs).  
- Optional: Doxygen docs (make docs if you add a Doxyfile).  

---

## SSR Recap (Requirements)
- Input: ≥1 text files, each = ranked list.  
- Process: consensus = sum of ranks → map each source to consensus → count inversions.  
- Output: CSV + Markdown (order, inversions, reliability).  
- Reliability: 1 - inversions / max_inversions.  
- Correctness: merge & BIT must match; quick is diagnostic.  

