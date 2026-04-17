# 050 — MPI Load Balance Dynamic

**Difficulty:** Hard · **Topic:** MPI, Master-Worker, Dynamic Load Balancing

## Problem

Implement master-worker dynamic load balancing. Rank 0 is the **master**: it distributes task indices (0 to `num_tasks-1`) to worker ranks on demand — as each worker finishes a task, the master sends it a new one. Workers compute `fib(task_index % 35 + 1)` (Fibonacci, 1-indexed) for each assigned task.

**Implement:**
```cpp
std::vector<long long> dynamic_load_balance(int num_tasks);
```

- Rank 0 returns `results[0..num_tasks-1]` where `results[i] = fib(i % 35 + 1)`.
- Other ranks return an empty vector.

## Constraints
- 1 ≤ num_tasks ≤ 200
- At least 2 MPI ranks (rank 0 = master, ranks 1..size-1 = workers)
- Use a tag-based sentinel (e.g., tag = -1 or empty message with special tag) to terminate workers

## Hints

<details><summary>Hint 1</summary>Master sends task index to a worker, worker replies with (task_index, result), master records and sends next task.</details>
<details><summary>Hint 2</summary>Use a special "poison pill" task index (e.g., -1) to signal workers to stop.</details>
<details><summary>Hint 3</summary>Iterative Fibonacci avoids stack overflow and is fast enough for fib(36).</details>
