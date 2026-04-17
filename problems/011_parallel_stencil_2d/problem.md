# 011 — Parallel 2D Stencil (Jacobi)

**Difficulty:** Medium · **Topic:** OpenMP, 2D stencil

## Problem

Apply the 2D Jacobi stencil for `iters` iterations on an N×N grid (flat row-major vector). Each iteration for interior points: `new[i][j] = (old[i-1][j] + old[i+1][j] + old[i][j-1] + old[i][j+1] + old[i][j]) / 5.0`. Boundary points are fixed.

**Implement:**
```cpp
std::vector<double> stencil_2d(std::vector<double> u, int N, int iters);
```

**Constraints:**
- 2 ≤ N ≤ 1000
- 0 ≤ iters ≤ 500

## Hints

<details><summary>Hint 1</summary>Use double buffering with swap. Parallelize the i (row) loop.</details>
<details><summary>Hint 2</summary>Index as `u[i*N + j]`.</details>
