# 042 — MPI Alltoall Transpose

**Difficulty:** Hard · **Topic:** MPI, MPI_Alltoall, distributed matrix transpose

## Problem

Transpose an n×n matrix distributed row-wise across ranks. Each rank initially holds `n/size` consecutive rows (block `rank`). After transposing, each rank holds `n/size` consecutive columns of the original matrix (stored column-major as a flat vector).

Use `MPI_Alltoall` (or `MPI_Alltoallv`) to redistribute data.

**Assume:** `n` is divisible by the number of ranks.

**Implement:**
```cpp
// local_rows: (n/size) rows of the matrix, row-major, length = (n/size)*n
// Returns: (n/size) columns of the transposed matrix, length = n*(n/size)
//   i.e., the local columns stored row-major: result[i*block + j] = A[j*n + rank*block + i]
std::vector<double> distributed_transpose(const std::vector<double>& local_rows, int n);
```

The returned vector on rank `r` should contain columns `[r*block .. (r+1)*block)` of the original matrix laid out row-major (row index changes fastest): `result[row * block + col_offset] = A[row][r*block + col_offset]`.

## Hints

<details><summary>Hint 1</summary>Before calling MPI_Alltoall, rearrange your local rows into blocks — one block per destination rank — so each block contains the elements that destination rank needs.</details>
<details><summary>Hint 2</summary>After MPI_Alltoall, each block you received came from a different source rank's rows. Rearrange (transpose within each block) to get the final column-major layout.</details>
