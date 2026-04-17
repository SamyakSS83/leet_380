# 010 — Parallel 1D Stencil (Jacobi Heat Equation)

**Difficulty:** Medium · **Topic:** OpenMP, stencil, Jacobi iteration

## Problem

Apply the 1D Jacobi heat equation stencil for `iters` iterations on an array of `n` doubles. Each iteration: `new[i] = (old[i-1] + old[i] + old[i+1]) / 3.0` for interior points (1 ≤ i ≤ n-2). Boundary points are fixed (never updated).

**Implement:**
```cpp
std::vector<double> stencil_1d(std::vector<double> u, int iters);
```

**Constraints:**
- 2 ≤ n ≤ 10^6
- 0 ≤ iters ≤ 1000
- Use double-buffering (swap arrays each iteration)

## Examples

```
Input:  u=[0,10,0,0,0], iters=1
Output: [0, 3.33, 3.33, 0, 0]  (approx)
```

## Hints

<details><summary>Hint 1</summary>Allocate a second buffer; each iteration writes to the other buffer, then swap.</details>
<details><summary>Hint 2</summary>Use `#pragma omp parallel for` inside the iteration loop.</details>
