# 027 — MPI Ping-Pong

**Difficulty:** Easy · **Topic:** MPI point-to-point, latency

## Problem

Rank 0 and rank 1 exchange a message of `msg_size` doubles `N` times (ping-pong). Return the average round-trip time in microseconds, measured by rank 0.

**Implement:**
```cpp
double mpi_ping_pong(int N, int msg_size);
```

**Constraints:**
- 1 ≤ N ≤ 1000
- 1 ≤ msg_size ≤ 10000
- Other ranks (if any) do nothing

## Hints

<details><summary>Hint 1</summary>Rank 0: send then receive, N times. Rank 1: receive then send, N times. Use `MPI_Wtime()` to measure.</details>
