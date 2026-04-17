#!/usr/bin/env python3
"""CLI for leet_380 — OMP/MPI local coding platform.

Usage:
  python main.py serve             # start web server (http://localhost:8380)
  python main.py list              # list all problems
  python main.py show <pid>        # print problem.md
  python main.py run <pid>         # compile + run tests (starter.cpp)
  python main.py run <pid> file.cpp
  python main.py run <pid> --nprocs 4
  python main.py solve <pid>       # run reference solution
"""
from __future__ import annotations
import argparse
import sys
from pathlib import Path

G = "\033[32m"; R = "\033[31m"; Y = "\033[33m"; B = "\033[34m"; D = "\033[2m"; X = "\033[0m"

sys.path.insert(0, str(Path(__file__).parent))
from server.runner import find_problem, list_problems, run_user_code, read_user_code


def cmd_list():
    probs = list_problems()
    print(f"{'#':>4}  {'Title':<45}  {'Diff':<8}  Compiler")
    print("─" * 75)
    for p in probs:
        dc = G if p.difficulty=="Easy" else Y if p.difficulty=="Medium" else R
        print(f"{B}{p.pid:>4}{X}  {p.title:<45}  {dc}{p.difficulty:<8}{X}  {p.compile_cmd}")


def cmd_show(pid: int):
    p = find_problem(pid)
    print((p.path / "problem.md").read_text())


def cmd_run(pid: int, src_file: str | None, nprocs: int | None, stop: bool):
    if src_file:
        source = Path(src_file).read_text()
    else:
        source, _ = read_user_code(pid)
    p = find_problem(pid)
    print(f"{B}Running #{pid:03d} — {p.title}{X}")
    result = run_user_code(pid, source, nprocs=nprocs, stop_on_first_fail=stop)

    if result.compile_output:
        label = f"{R}COMPILE ERROR{X}" if result.fatal_error else f"{D}compiler{X}"
        print(f"\n{label}:\n{D}{result.compile_output}{X}")
    if result.fatal_error:
        print(f"\n{R}Fatal:{X} {result.fatal_error}")
        return 1

    for c in result.cases:
        tag = f"{G}PASS{X}" if c.passed else f"{R}FAIL{X}"
        ms = f"{D}{c.elapsed_ms:.1f}ms{X}" if c.elapsed_ms else ""
        print(f"  {tag}  {c.name}  {ms}")
        if not c.passed and c.message:
            print(f"       {Y}{c.message}{X}")
        for ln in (c.stdout or "").strip().splitlines()[:20]:
            print(f"       {D}{ln}{X}")
        for ln in (c.stderr or "").strip().splitlines()[:10]:
            print(f"       {R}{ln}{X}")

    colour = G if result.ok else R
    print(f"\n{colour}{result.passed}/{result.total} tests passed{X}")
    return 0 if result.ok else 1


def cmd_solve(pid: int):
    p = find_problem(pid)
    sol = p.path / "solution.cpp"
    if not sol.exists():
        print(f"{R}solution.cpp not found{X}"); return 1
    return cmd_run(pid, str(sol), None, False)


def cmd_serve():
    import uvicorn
    print(f"{G}Starting leet_380 at http://127.0.0.1:8380{X}")
    uvicorn.run("server.app:app", host="127.0.0.1", port=8380, reload=True)


def main():
    ap = argparse.ArgumentParser(prog="leet_380")
    sub = ap.add_subparsers(dest="cmd")
    sub.add_parser("serve")
    sub.add_parser("list")
    sp = sub.add_parser("show"); sp.add_argument("pid", type=int)
    rp = sub.add_parser("run"); rp.add_argument("pid", type=int)
    rp.add_argument("file", nargs="?"); rp.add_argument("--nprocs", type=int); rp.add_argument("--stop", action="store_true")
    slp = sub.add_parser("solve"); slp.add_argument("pid", type=int)

    args = ap.parse_args()
    if args.cmd == "serve":   cmd_serve()
    elif args.cmd == "list":  cmd_list()
    elif args.cmd == "show":  cmd_show(args.pid)
    elif args.cmd == "run":   sys.exit(cmd_run(args.pid, args.file, args.nprocs, args.stop))
    elif args.cmd == "solve": sys.exit(cmd_solve(args.pid))
    else: ap.print_help()


if __name__ == "__main__":
    main()
