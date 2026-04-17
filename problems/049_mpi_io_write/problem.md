# 049 — MPI Parallel File I/O

**Difficulty:** Medium · **Topic:** MPI, MPI-IO, MPI_File_write_at, parallel file write

## Problem

Each rank writes its portion of a distributed array to a shared binary file using `MPI_File_write_at`. The file is organized so that rank `r`'s data comes at offset `r * local_size * sizeof(double)` bytes, forming the global array in rank order.

**Implement:**
```cpp
void parallel_file_write(const std::vector<double>& local_data, const char* filename);
// Each rank writes local_data at its offset in the file (rank-order layout).
// All ranks participate; the file is closed collectively.
```

The test harness will call this function and then verify by reading back the file on rank 0.

## Hints

<details><summary>Hint 1</summary>Use MPI_File_open with MPI_MODE_CREATE | MPI_MODE_WRONLY, compute byte offset as rank * local_data.size() * sizeof(double), then call MPI_File_write_at.</details>
<details><summary>Hint 2</summary>Use MPI_File_close after writing. All ranks must call open/close collectively.</details>
