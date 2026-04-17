# 024 — Parallel Sparse Matrix-Vector Product (CSR)

**Difficulty:** Medium · **Topic:** OpenMP, sparse matrix, CSR format

## Problem

Compute `y = A * x` where `A` is in CSR (Compressed Sparse Row) format. Parallelize over rows.

CSR format:
- `row_ptr[i]` to `row_ptr[i+1]-1` gives column indices for row `i`
- `col_idx[k]` is the column of the k-th non-zero
- `values[k]` is the value of the k-th non-zero

**Implement:**
```cpp
std::vector<double> parallel_spmv(
    const std::vector<int>& row_ptr,
    const std::vector<int>& col_idx,
    const std::vector<double>& values,
    const std::vector<double>& x, int nrows);
```

## Hints

<details><summary>Hint 1</summary>Each row's dot product is independent — use `#pragma omp parallel for`.</details>
