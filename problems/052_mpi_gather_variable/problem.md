# 052 — MPI Gather Variable

**Difficulty:** Medium · **Topic:** MPI, MPI_Gatherv, Variable-length Communication

## Problem

Each rank `r` holds a vector of `r + 1` doubles. Use `MPI_Gatherv` to collect all local vectors on rank 0, producing the concatenation `rank0_data || rank1_data || rank2_data || ...`.

**Implement:**
```cpp
std::vector<double> gather_variable(const std::vector<double>& local_data);
```

- Rank 0 returns the fully gathered vector (size = 0+1+1+2+...+size-1 = size*(size+1)/2... wait: 1+2+...+size = size*(size+1)/2).
- Other ranks return an empty vector.

## Constraints
- Rank r contributes exactly r+1 elements.
- Use `MPI_Gatherv` (not a loop of point-to-point sends).

## Hints

<details><summary>Hint 1</summary>On rank 0, build `recvcounts` array where `recvcounts[r] = r + 1`, and `displs[r] = r*(r+1)/2`.</details>
<details><summary>Hint 2</summary>Call `MPI_Gatherv` with the root's recv buffer sized to `size*(size+1)/2`.</details>
