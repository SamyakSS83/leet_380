# 063 — Hybrid BFS

**Difficulty:** Hard · **Topic:** Hybrid OMP+MPI

## Problem

Perform Breadth-First Search (BFS) on a graph whose vertices are distributed **round-robin** across MPI ranks. Within each rank, OpenMP parallelizes the frontier expansion.

**Vertex ownership:** vertex `v` is owned by rank `v % num_ranks`.

**Algorithm:**
1. Initialize distances: `dist[source] = 0`, all others = -1.
2. Each rank holds its local adjacency list `local_adj[i]` for vertex `i * size + rank`.
3. Each BFS level:
   a. Each rank expands its local frontier vertices using OMP threads.
   b. Ranks exchange newly discovered vertices (and their distances) via `MPI_Allreduce` or `MPI_Allgather`.
   c. Repeat until no new vertices are discovered globally.
4. Rank 0 gathers and returns the full distance array.

## Function Signature

```cpp
std::vector<int> hybrid_bfs(
    const std::vector<std::vector<int>>& local_adj,
    int total_vertices, int source);
```

**Parameters:**
- `local_adj` — adjacency list for local vertices; `local_adj[i]` contains neighbors of vertex `i * size + rank`
- `total_vertices` — total number of vertices in the graph
- `source` — BFS source vertex (global index)

**Returns:** Full distance array `dist[0..total_vertices-1]` on rank 0; empty on others.

## Constraints

- Graph is undirected and unweighted
- Connected graph (all vertices reachable from source)
- total_vertices is a multiple of num_ranks
- 0 ≤ source < total_vertices
