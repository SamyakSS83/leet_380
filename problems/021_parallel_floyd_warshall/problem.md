# 021 — Parallel Floyd-Warshall APSP

**Difficulty:** Hard · **Topic:** OpenMP, APSP, Floyd-Warshall

## Problem

Compute All-Pairs Shortest Paths on a weighted graph with V vertices using Floyd-Warshall. The graph is represented as a V×V distance matrix (flat row-major). Use `INF = 1e18` for no edge.

**Implement:**
```cpp
std::vector<double> parallel_floyd_warshall(std::vector<double> dist, int V);
```

**Constraints:**
- 1 ≤ V ≤ 500
- Parallelize the inner `i` and `j` loops for each `k`

## Hints

<details><summary>Hint 1</summary>The k-loop must be sequential (each k depends on k-1). Parallelize `#pragma omp parallel for collapse(2)` over i and j.</details>
