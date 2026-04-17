# 051 — MPI Reduce Custom Op

**Difficulty:** Medium · **Topic:** MPI, MPI_Op_create, Custom Reduction

## Problem

Find the global maximum value and the rank that holds it using a single `MPI_Reduce` with a **custom MPI operator** (created via `MPI_Op_create`). Each rank contributes its own `local_value`.

**Implement:**
```cpp
struct ArgMax { double value; int rank; };
ArgMax mpi_argmax(double local_value);
```

- Returns the global argmax `{max_value, rank_of_max}` on rank 0.
- On other ranks the return value is unspecified (may return any value).
- Tie-breaking: if two ranks have the same value, the lower rank wins.

## Constraints
- Values may be negative, zero, or positive.
- At least 2 MPI ranks.

## Hints

<details><summary>Hint 1</summary>Define a struct with `double value; int rank;` and use `MPI_Type_create_struct` or pack into a contiguous user-defined type.</details>
<details><summary>Hint 2</summary>Alternatively, use two separate reductions or `MPI_Allreduce` to find the max, then reduce rank.</details>
<details><summary>Hint 3</summary>For `MPI_Op_create`, the user function signature is `void op(void* in, void* inout, int* len, MPI_Datatype*)`. Compare `in[i].value` vs `inout[i].value`.</details>
