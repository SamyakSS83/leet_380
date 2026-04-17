# 013 — OMP Tasks: Parallel Fibonacci

**Difficulty:** Medium · **Topic:** OpenMP tasks

## Problem

Compute Fibonacci(n) using OMP tasks with a cutoff for small `n` (use sequential for n < cutoff).

**Implement:**
```cpp
long long parallel_fibonacci(int n, int cutoff);
```

**Constraints:**
- 0 ≤ n ≤ 45
- 1 ≤ cutoff ≤ 20 (use sequential fib for n < cutoff)
- fib(0)=0, fib(1)=1, fib(n)=fib(n-1)+fib(n-2)

## Examples

```
parallel_fibonacci(10, 5) → 55
parallel_fibonacci(0, 5)  → 0
```

## Hints

<details><summary>Hint 1</summary>Use `#pragma omp parallel` + `#pragma omp single` to spawn the root task, then `#pragma omp task` for recursive calls.</details>
<details><summary>Hint 2</summary>Use `#pragma omp taskwait` to synchronize before summing.</details>
