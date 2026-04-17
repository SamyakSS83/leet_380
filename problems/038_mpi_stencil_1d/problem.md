# 038 — MPI 1D Stencil with Ghost Cells

**Difficulty:** Medium · **Topic:** MPI, domain decomposition, stencil

## Problem

Apply 1D Jacobi stencil for `iters` iterations on a distributed array. Each rank owns a contiguous chunk. Exchange ghost cells (single boundary values) with neighbors before each sweep.

**Implement:**
```cpp
std::vector<double> mpi_stencil_1d(std::vector<double> local_data, int iters);
```

Returns the final local chunk (without ghosts) after `iters` iterations.

## Hints

<details><summary>Hint 1</summary>Allocate local array with 2 extra ghost cells. Each iteration: exchange ghosts with MPI_Sendrecv, then apply stencil.</details>
