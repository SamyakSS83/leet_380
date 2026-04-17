# 012 — OMP Schedule Clauses

**Difficulty:** Easy · **Topic:** OpenMP, schedule(dynamic)

## Problem

Compute a weighted sum where each element takes variable work (simulated by inner loop). Use `schedule(dynamic)` to properly load-balance irregular work.

**Implement:**
```cpp
double irregular_sum(const std::vector<int>& work_sizes, double base);
```

For each `i`, compute `base^work_sizes[i]` (pow) and sum all results. The irregular work per element makes dynamic scheduling beneficial.

**Constraints:**
- 0 ≤ n ≤ 10^5
- 0 ≤ work_sizes[i] ≤ 20
- base > 0

## Hints

<details><summary>Hint 1</summary>Use `#pragma omp parallel for schedule(dynamic, chunk_size) reduction(+:sum)`.</details>
<details><summary>Hint 2</summary>A chunk size of 1-16 works well for highly variable work sizes.</details>
