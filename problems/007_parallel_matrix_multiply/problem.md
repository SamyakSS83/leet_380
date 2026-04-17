# 007 — Parallel Matrix Multiply

**Difficulty:** Medium · **Topic:** OpenMP, tiled matmul

## Problem

Multiply two N×N matrices A and B, producing C = A × B. Matrices are stored as flat 1D vectors in row-major order: `A[i*N + j]`.

**Implement:**
```cpp
std::vector<double> parallel_matmul(const std::vector<double>& A,
                                     const std::vector<double>& B, int N);
```

**Constraints:**
- 0 ≤ N ≤ 1000
- Use `#pragma omp parallel for` over outer loops

## Examples

```
Input:  A=[1,2,3,4] (2x2), B=[5,6,7,8] (2x2), N=2
Output: [19,22,43,50]
```

## Hints

<details><summary>Hint 1</summary>Parallelize the outermost `i` loop. Use `collapse(2)` for i and j.</details>
<details><summary>Hint 2</summary>For cache efficiency, keep k as the innermost loop.</details>
