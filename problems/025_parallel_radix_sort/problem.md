# 025 — Parallel Radix Sort (LSD, 8-bit digits)

**Difficulty:** Hard · **Topic:** OpenMP, radix sort, counting sort

## Problem

Sort a vector of non-negative 32-bit integers using LSD radix sort with 8-bit digits (4 passes). Parallelize the counting phase.

**Implement:**
```cpp
std::vector<unsigned> parallel_radix_sort(std::vector<unsigned> v);
```

**Constraints:**
- 0 ≤ n ≤ 10^7
- Values in [0, 2^32 - 1]

## Hints

<details><summary>Hint 1</summary>4 passes, each pass extracts 8 bits. In each pass: parallel count per thread, merge counts, compute prefix sum of merged counts, scatter.</details>
<details><summary>Hint 2</summary>Per-thread histograms, then exclusive scan over merged counts to get output positions.</details>
