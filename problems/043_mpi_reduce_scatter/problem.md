# 043 — MPI Reduce Scatter

**Difficulty:** Hard · **Topic:** MPI, MPI_Reduce_scatter, segmented reduction

## Problem

Each rank contributes a full-length array of size `n * size`. Use `MPI_Reduce_scatter` to compute the global element-wise sum and distribute one contiguous segment of length `n` to each rank. Rank `r` receives the sum of elements at indices `[r*n .. (r+1)*n)` across all ranks.

**Implement:**
```cpp
// data: vector of length n * size on every rank
// Returns: vector of length n — this rank's segment of the global sum
std::vector<double> reduce_scatter_sum(const std::vector<double>& data, int n);
```

## Hints

<details><summary>Hint 1</summary>MPI_Reduce_scatter takes a `recvcounts` array specifying how many elements each rank receives. Set each entry to `n`.</details>
<details><summary>Hint 2</summary>The result on rank r equals sum over all ranks of data[r*n .. (r+1)*n).</details>
