# 044 — MPI Parallel Sort

**Difficulty:** Hard · **Topic:** MPI, parallel sample sort, collective communication

## Problem

Implement parallel sample sort. Each rank starts with a portion of unsorted integers. After sorting, each rank holds a contiguous, globally sorted sub-range: rank 0 has the smallest elements, rank P-1 has the largest. The concatenation of all ranks' outputs (in rank order) is the globally sorted array.

**Implement:**
```cpp
std::vector<int> parallel_sample_sort(std::vector<int> local_data);
```

## Algorithm outline

1. Each rank locally sorts its data.
2. Each rank selects `size` evenly-spaced samples from its sorted local data.
3. Gather all samples to rank 0; rank 0 sorts them and picks `size-1` splitters.
4. Broadcast splitters to all ranks.
5. Each rank partitions its data into `size` buckets using the splitters.
6. Exchange buckets with `MPI_Alltoallv`.
7. Each rank sorts its received data and returns it.

## Hints

<details><summary>Hint 1</summary>Use std::sort locally. After gathering samples on rank 0, sort and pick splitters at indices size, 2*size, ..., (size-1)*size from the sorted sample array.</details>
<details><summary>Hint 2</summary>Use std::lower_bound to partition your local data into buckets for each destination rank.</details>
