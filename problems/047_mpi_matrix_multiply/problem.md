# 047 — MPI Matrix Multiply

**Difficulty:** Hard · **Topic:** MPI, distributed matrix multiply, MPI_Gather

## Problem

Compute C = A * B where A is n×k and B is k×m, both stored flat row-major. Matrix A is distributed row-wise: each rank holds `n/size` consecutive rows. Matrix B is fully replicated on all ranks. Each rank computes its rows of C. Rank 0 gathers and returns the full C (n×m).

**Implement:**
```cpp
// local_A: (n/size) rows of A, length = local_rows * k
// B: full k×m matrix, length = k*m (same on all ranks)
// Returns: full C (n×m) flat row-major on rank 0, empty on others
std::vector<double> distributed_matmul(
    const std::vector<double>& local_A, int local_rows,
    const std::vector<double>& B, int n, int k, int m);
```

**Constraints:** n divisible by number of ranks.

## Hints

<details><summary>Hint 1</summary>Each rank computes `local_rows` rows of C independently. C_local[i][j] = sum over p of A_local[i][p] * B[p][j].</details>
<details><summary>Hint 2</summary>Use MPI_Gather to collect all local C rows to rank 0.</details>
