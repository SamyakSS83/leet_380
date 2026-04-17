# 003 — Parallel Matrix-Vector Multiply

**Difficulty:** Easy · **Topic:** OpenMP, parallel for

## Problem

Given an `m x n` matrix `A` (row-major) and a vector `x` of length `n`, compute `y = A * x` using OpenMP. Each output element `y[i] = sum_j A[i][j] * x[j]`.

**Implement:**
```cpp
std::vector<double> parallel_matvec(
    const std::vector<std::vector<double>>& A,
    const std::vector<double>& x);
```

**Constraints:**
- 0 ≤ m, n ≤ 2000
- Parallelize over rows (outer loop)

## Examples

```
Input:  A=[[1,2],[3,4]], x=[1,1]
Output: [3.0, 7.0]
```

## Hints

<details><summary>Hint 1</summary>Use `#pragma omp parallel for` over the row index `i`.</details>
<details><summary>Hint 2</summary>The inner loop over `j` stays sequential per thread.</details>
