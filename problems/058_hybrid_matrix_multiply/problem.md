# 058 — Hybrid Matrix Multiply

**Difficulty:** Hard · **Topic:** Hybrid OMP+MPI

## Problem

Compute the matrix product `C = A * B` where:
- Matrix `A` is `(total_rows × k)` and distributed row-wise across MPI ranks.
- Matrix `B` is `(k × m)` and replicated on all ranks.
- Each rank owns `local_rows` contiguous rows of `A` and computes the corresponding rows of `C` using OpenMP threads.
- Rank 0 gathers all rows of `C` and returns the full result.

Matrices are stored in row-major order as flat `std::vector<double>`.

## Function Signature

```cpp
std::vector<double> hybrid_matmul(
    const std::vector<double>& local_A, int local_rows,
    const std::vector<double>& B, int n, int k, int m);
```

**Parameters:**
- `local_A` — local rows of A, size `local_rows × k`
- `local_rows` — number of rows this rank owns
- `B` — full matrix B, size `k × m`, replicated on all ranks
- `n` — total number of rows in A (sum of all `local_rows`)
- `k` — inner dimension
- `m` — number of columns in B (and C)

**Returns:** Full matrix C (`n × m`) on rank 0; empty vector on other ranks.

## Example

With A (4×2) distributed 1 row/rank across 4 ranks, B (2×3):
- Each rank computes 1 row of C using OMP.
- Rank 0 gathers and returns full 4×3 C.

## Constraints

- 1 ≤ local_rows, k, m ≤ 1024
- Values are doubles
- All ranks call the function; only rank 0 returns the full result
