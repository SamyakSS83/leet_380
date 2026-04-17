# 035 — MPI Non-Blocking Overlap

**Difficulty:** Medium · **Topic:** MPI, non-blocking, MPI_Isend/MPI_Irecv

## Problem

Each rank sends its local array to rank 0 using non-blocking `MPI_Isend`. While waiting, each rank performs a local computation (sum of squares). Rank 0 collects all arrays.

**Implement:**
```cpp
std::pair<double, std::vector<std::vector<double>>>
    mpi_nonblocking_collect(const std::vector<double>& local_data);
```

Returns `{local_sum_of_squares, all_data}` on rank 0. `all_data[r]` = data from rank r. Other ranks return `{local_sum_of_squares, {}}`.

## Hints

<details><summary>Hint 1</summary>Rank 0: post all Irecvs first, then compute local sum of squares, then Waitall.</details>
<details><summary>Hint 2</summary>Non-root ranks: Isend, compute local sum of squares, Wait.</details>
