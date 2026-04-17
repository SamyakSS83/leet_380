# 045 — MPI Distributed BFS

**Difficulty:** Hard · **Topic:** MPI, distributed graph algorithms, BFS, frontier exchange

## Problem

Perform BFS on a distributed graph. Vertices are distributed round-robin: vertex `v` belongs to rank `v % size`. Each rank stores the adjacency list for its local vertices. Implement BFS from vertex 0 and return the distance array on rank 0 (-1 for unreachable vertices).

**Implement:**
```cpp
std::vector<int> distributed_bfs(
    const std::vector<std::vector<int>>& local_adj,  // local_adj[i] = neighbors of local vertex i*size+rank
    int total_vertices,
    int source);
// Returns distance array of size total_vertices on rank 0, empty on others.
```

`local_adj[i]` contains the neighbors of vertex `i * size + rank`.

## Hints

<details><summary>Hint 1</summary>Maintain a local visited/distance array. Each BFS level: process current frontier, collect outgoing edges, send new frontier vertices to the rank that owns them.</details>
<details><summary>Hint 2</summary>Use MPI_Alltoallv to exchange frontier vertices between ranks each BFS level. Terminate when all ranks have an empty frontier.</details>
