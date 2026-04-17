# 037 — MPI Cartesian Topology: 2D Halo Exchange

**Difficulty:** Hard · **Topic:** MPI, Cartesian topology, halo exchange

## Problem

Create a 2D Cartesian communicator for a grid of ranks. Each rank holds a local NxN subgrid. Perform one halo exchange (exchange boundary rows/columns with neighbors).

**Implement:**
```cpp
void halo_exchange_2d(std::vector<double>& grid, int local_N, MPI_Comm cart_comm);
```

After the call, `grid`'s halo rows/columns contain neighbor data. (For simplicity, the grid includes halos: size (local_N+2)x(local_N+2).)

## Hints

<details><summary>Hint 1</summary>Use `MPI_Cart_shift` to find neighbors. Exchange top/bottom rows, then left/right columns.</details>
