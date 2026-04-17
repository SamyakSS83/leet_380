# 041 — MPI Gather Stats

**Difficulty:** Medium · **Topic:** MPI, MPI_Gather, reduction, statistics

## Problem

Each rank holds a local array of doubles. Compute the global min, max, sum, and count across all ranks using MPI gather operations. Return the populated `Stats` struct on rank 0; other ranks return a zeroed struct.

**Implement:**
```cpp
struct Stats { double min, max, sum; long long count; };
Stats gather_stats(const std::vector<double>& local_data);
```

- `local_data` may differ in length across ranks.
- On rank 0, `min` and `max` are the global extremes, `sum` is the global total, `count` is the total number of elements across all ranks.
- On ranks > 0, return `Stats{}` (all zeros).

## Hints

<details><summary>Hint 1</summary>Compute local min/max/sum/count first, then use MPI_Gather or MPI_Reduce to combine at rank 0.</details>
<details><summary>Hint 2</summary>Use MPI_Reduce with MPI_MIN, MPI_MAX, and MPI_SUM for scalar reductions, or gather an array of local Stats and reduce manually on rank 0.</details>
