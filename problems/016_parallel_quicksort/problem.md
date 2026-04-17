# 016 — Parallel Quicksort

**Difficulty:** Hard · **Topic:** OpenMP tasks, quicksort

## Problem

Sort a vector of integers using quicksort parallelized with OMP tasks. Use median-of-three pivot. Fall back to `std::sort` for subarrays smaller than `cutoff`.

**Implement:**
```cpp
void parallel_quicksort(std::vector<int>& v, int cutoff);
```

**Constraints:**
- 0 ≤ n ≤ 10^6
- Correctly handles duplicates

## Hints

<details><summary>Hint 1</summary>Partition in-place, then spawn tasks for left and right sub-arrays.</details>
<details><summary>Hint 2</summary>Use `#pragma omp task` with `if(size > cutoff)` to avoid spawning tiny tasks.</details>
