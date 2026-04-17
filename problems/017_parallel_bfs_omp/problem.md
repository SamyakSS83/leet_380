# 017 — Parallel BFS (OpenMP)

**Difficulty:** Hard · **Topic:** OpenMP, BFS, level-synchronous

## Problem

Perform level-synchronous BFS on an undirected graph given as an adjacency list. Return a vector of distances from source, or -1 for unreachable nodes.

**Implement:**
```cpp
std::vector<int> parallel_bfs(const std::vector<std::vector<int>>& adj, int source);
```

**Constraints:**
- 1 ≤ V ≤ 10^5, 0 ≤ E ≤ 5×10^5
- Graph may be disconnected

## Hints

<details><summary>Hint 1</summary>Maintain a frontier vector. In parallel, expand all frontier nodes and collect their unvisited neighbors into the next frontier.</details>
<details><summary>Hint 2</summary>Use atomic CAS or a per-thread buffer to safely add to the next frontier.</details>
