# 023 — Parallel SAXPY (DAXPY)

**Difficulty:** Easy · **Topic:** OpenMP, BLAS level-1

## Problem

Compute DAXPY: `y[i] = a * x[i] + y[i]` for all `i`, in-place. Parallelize with OpenMP.

**Implement:**
```cpp
void parallel_saxpy(double a, const std::vector<double>& x, std::vector<double>& y);
```

**Constraints:**
- 0 ≤ n ≤ 10^7
- x and y have the same length; modify y in-place

## Hints

<details><summary>Hint 1</summary>Simple `#pragma omp parallel for` over i.</details>
