# 015 — Parallel Merge Sort

**Difficulty:** Hard · **Topic:** OpenMP tasks, divide and conquer

## Problem

Sort a vector of integers using merge sort parallelized with OMP tasks. Use a cutoff below which sequential sort is used.

**Implement:**
```cpp
void parallel_merge_sort(std::vector<int>& v, int cutoff);
```

**Constraints:**
- 0 ≤ n ≤ 10^6
- 1 ≤ cutoff ≤ 10^4
- Must produce correctly sorted output

## Hints

<details><summary>Hint 1</summary>Recursively spawn OMP tasks for the two halves, then wait and merge.</details>
<details><summary>Hint 2</summary>For n < cutoff, use `std::sort` directly.</details>
