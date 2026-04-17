# 022 — OMP Nested Parallelism

**Difficulty:** Medium · **Topic:** OpenMP, nested parallel regions

## Problem

Compute the sum of a 2D matrix using nested parallelism: outer `parallel` over rows, inner `parallel for` over columns within each row.

**Implement:**
```cpp
double nested_parallel_sum(const std::vector<std::vector<double>>& mat);
```

**Constraints:**
- 0 ≤ rows, cols ≤ 2000
- Must use `omp_set_nested(1)` and nested `#pragma omp parallel`

## Hints

<details><summary>Hint 1</summary>Call `omp_set_nested(1)` before the outer parallel region.</details>
<details><summary>Hint 2</summary>Use reduction inside both parallel regions.</details>
