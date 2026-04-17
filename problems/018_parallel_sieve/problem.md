# 018 — Parallel Sieve of Eratosthenes

**Difficulty:** Medium · **Topic:** OpenMP, sieve

## Problem

Find all primes up to N using a parallel Sieve of Eratosthenes. Return them sorted.

**Implement:**
```cpp
std::vector<int> parallel_sieve(int N);
```

**Constraints:**
- 0 ≤ N ≤ 10^7

## Examples

```
parallel_sieve(10) → [2, 3, 5, 7]
parallel_sieve(2)  → [2]
parallel_sieve(1)  → []
```

## Hints

<details><summary>Hint 1</summary>First mark multiples of small primes up to sqrt(N) sequentially, then parallelize the marking of remaining composites.</details>
<details><summary>Hint 2</summary>Use `#pragma omp parallel for` over the sieving loops for large primes.</details>
