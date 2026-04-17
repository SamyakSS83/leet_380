# leet_380

65 medium-hard OMP/MPI/hybrid C++ problems. Like leetcode but for HPC.

## Problems

- 001–025: OpenMP (reductions, stencils, tasks, locks, sorts, BFS, sieve)
- 026–055: MPI (collectives, P2P, derived types, RMA, I/O, topology)
- 056–065: Hybrid MPI+OMP

## Run all tests

```bash
bash run_all_tests.sh
```

## Web IDE

```bash
pip install -r requirements.txt
python main.py serve
# open http://localhost:8380
```

## CLI

```bash
python main.py list
python main.py show 42
python main.py run 42
python main.py solve 42   # show reference solution
```

## Deps

- `g++` with OpenMP (`-fopenmp`)
- `mpic++` / `mpirun`
- Python 3.10+, fastapi, uvicorn
