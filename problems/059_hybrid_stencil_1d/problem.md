# 059 — Hybrid Stencil 1D

**Difficulty:** Medium · **Topic:** Hybrid OMP+MPI

## Problem

Apply a 1D Jacobi stencil iteratively to a distributed array. The stencil update is:

```
u_new[i] = 0.5 * (u[i-1] + u[i+1])
```

The domain is decomposed across MPI ranks. Each rank holds a chunk of the array padded with **ghost cells** (the first and last elements of `local_u` are ghosts). After each iteration, ranks exchange ghost cells with their neighbors using MPI point-to-point communication. Within each iteration, the interior update is parallelized with OpenMP.

After all iterations, rank 0 gathers all interior elements and returns the full result.

## Function Signature

```cpp
std::vector<double> hybrid_stencil_1d(
    std::vector<double> local_u, int iterations);
```

**Parameters:**
- `local_u` — local data including ghost cells; interior is `local_u[1..size-2]`
- `iterations` — number of Jacobi sweeps to apply

**Returns:** Full interior result on rank 0 (length = total interior points); empty vector on other ranks.

## Ghost Cell Exchange

Each iteration:
1. Send rightmost interior element to right neighbor's left ghost.
2. Send leftmost interior element to left neighbor's right ghost.
3. Receive from both neighbors.
4. Apply stencil to interior using OMP.

## Boundary Conditions

- Rank 0's left ghost is fixed at 0.0 (Dirichlet).
- Last rank's right ghost is fixed at 0.0 (Dirichlet).

## Constraints

- Each rank has the same number of interior points
- iterations ≥ 0
- Values are doubles
