# 014 — OMP Locks: Fine-Grained Hash Map

**Difficulty:** Hard · **Topic:** OpenMP, locks, concurrency

## Problem

Implement a fixed-size hash map with concurrent insert using per-bucket `omp_lock_t`. Each bucket is a linked list. Insertions to different buckets can proceed in parallel.

**Implement:**
```cpp
struct HashMap; // opaque
HashMap* hashmap_create(int num_buckets);
void hashmap_insert(HashMap* hm, int key, int value);
int  hashmap_get(HashMap* hm, int key);  // returns -1 if not found
void hashmap_destroy(HashMap* hm);
```

**Constraints:**
- 1 ≤ num_buckets ≤ 1024
- Keys are non-negative integers
- Concurrent inserts must be thread-safe using per-bucket locks

## Hints

<details><summary>Hint 1</summary>Each bucket has its own `omp_lock_t`. Lock the bucket before modifying it.</details>
<details><summary>Hint 2</summary>Use chaining (linked list) for collision handling.</details>
