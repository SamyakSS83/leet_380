# 036 — MPI Derived Struct Type

**Difficulty:** Medium · **Topic:** MPI, MPI_Type_create_struct

## Problem

Send an array of `Particle` structs from rank 0 to all other ranks using a custom MPI derived type created with `MPI_Type_create_struct`.

```cpp
struct Particle { double x, y, z; int id; };
```

**Implement:**
```cpp
std::vector<Particle> mpi_send_particles(std::vector<Particle> particles);
```

Rank 0 provides particles and broadcasts them. All ranks return the full array.

## Hints

<details><summary>Hint 1</summary>Use `MPI_Type_create_struct` with offsets of `x`, `y`, `z`, `id`. Commit, broadcast, free.</details>
