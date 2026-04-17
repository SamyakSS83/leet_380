"""Core runner for leet_380: compile user C++ code with test harness, run, parse JSON results.

Each problem has:
  - starter.cpp   : skeleton the user fills in
  - solution.cpp  : reference implementation
  - tests.cpp     : C++ harness that #includes solve.cpp and prints JSON results
  - tests.py      : Python metadata (COMPILE_CMD, COMPILE_FLAGS, RUN_PREFIX, NPROCS)

Runs as a local tool — no sandbox.
"""
from __future__ import annotations

import json
import os
import shutil
import subprocess
import tempfile
import time
from dataclasses import dataclass, field
from pathlib import Path

PROBLEMS_DIR = Path(__file__).resolve().parent.parent / "problems"
HARNESS_H    = Path(__file__).resolve().parent / "harness.h"

# ---------------------------------------------------------------------------
# Problem discovery

@dataclass
class ProblemMeta:
    pid: int
    slug: str
    title: str
    difficulty: str
    topic: str
    path: Path
    compile_cmd: str = "g++"
    compile_flags: list = field(default_factory=lambda: ["-O2", "-std=c++17", "-fopenmp"])
    run_prefix: list = field(default_factory=list)
    nprocs: int = 1

    @property
    def folder_name(self) -> str:
        return f"{self.pid:03d}_{self.slug}"


def _parse_front_matter(md_path: Path) -> tuple[str, str, str]:
    title, difficulty, topic = md_path.parent.name, "Unknown", ""
    if not md_path.exists():
        return title, difficulty, topic
    with md_path.open() as f:
        for line in f:
            line = line.strip()
            if line.startswith("# "):
                raw = line[2:].strip()
                for sep in ("—", "-", ":"):
                    if sep in raw:
                        head, _, tail = raw.partition(sep)
                        if head.strip().split()[0].isdigit():
                            raw = tail.strip()
                            break
                title = raw
            if line.startswith("**Difficulty:**"):
                part = line.split("**Difficulty:**", 1)[1]
                # "Easy · **Topic:** OpenMP basics"
                difficulty = part.split("·")[0].replace("**", "").strip()
                if "**Topic:**" in line:
                    topic = line.split("**Topic:**", 1)[1].replace("**", "").strip()
                break
    return title, difficulty, topic


def _load_meta(prob_dir: Path) -> dict:
    """Load compile metadata from tests.py in problem dir."""
    meta = {
        "compile_cmd": "g++",
        "compile_flags": ["-O2", "-std=c++17", "-fopenmp"],
        "run_prefix": [],
        "nprocs": 1,
    }
    tests_py = prob_dir / "tests.py"
    if tests_py.exists():
        ns: dict = {}
        try:
            exec(tests_py.read_text(), ns)
            for k in meta:
                if k.upper() in ns:
                    meta[k] = ns[k.upper()]
        except Exception:
            pass
    return meta


def list_problems() -> list[ProblemMeta]:
    out = []
    if not PROBLEMS_DIR.exists():
        return out
    for child in sorted(PROBLEMS_DIR.iterdir()):
        if not child.is_dir():
            continue
        name = child.name
        if "_" not in name:
            continue
        num, slug = name.split("_", 1)
        try:
            pid = int(num)
        except ValueError:
            continue
        title, diff, topic = _parse_front_matter(child / "problem.md")
        meta = _load_meta(child)
        out.append(ProblemMeta(
            pid=pid, slug=slug, title=title, difficulty=diff, topic=topic,
            path=child, **meta,
        ))
    out.sort(key=lambda p: p.pid)
    return out


def find_problem(pid: int) -> ProblemMeta:
    for p in list_problems():
        if p.pid == pid:
            return p
    raise FileNotFoundError(f"No problem with id {pid}")


# ---------------------------------------------------------------------------
# Data classes

@dataclass
class CaseResult:
    name: str
    passed: bool
    message: str
    elapsed_ms: float = 0.0
    stdout: str = ""
    stderr: str = ""


@dataclass
class RunResult:
    problem_id: int
    ok: bool
    total: int
    passed: int
    cases: list[CaseResult] = field(default_factory=list)
    fatal_error: str | None = None
    static_issues: list[str] = field(default_factory=list)
    compile_output: str = ""


# ---------------------------------------------------------------------------
# Compilation and execution

def _compile(prob: ProblemMeta, solve_path: Path, out_exe: Path) -> tuple[bool, str]:
    """Compile tests.cpp (which includes solve.cpp) into out_exe.
    Returns (success, stderr_output).
    """
    tests_cpp = prob.path / "tests.cpp"
    if not tests_cpp.exists():
        return False, f"tests.cpp not found in {prob.path}"

    # We compile by creating a tiny translation unit that #includes
    # the user solve file and then the problem's tests.cpp.
    # The harness.h lives in server/ and is symlinked/included via -I.
    wrapper = out_exe.parent / f"_wrapper_{prob.pid}.cpp"
    wrapper.write_text(
        f'#include "{solve_path}"\n'
        f'#include "{tests_cpp}"\n'
    )

    cmd = [prob.compile_cmd] + prob.compile_flags + [
        f"-I{HARNESS_H.parent}",   # so tests.cpp can #include "harness.h"
        str(wrapper),
        "-o", str(out_exe),
    ]

    try:
        r = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
        return r.returncode == 0, (r.stdout + r.stderr).strip()
    except subprocess.TimeoutExpired:
        return False, "compilation timed out (>30s)"
    except FileNotFoundError:
        return False, f"compiler not found: {prob.compile_cmd}"


def _run_exe(prob: ProblemMeta, exe: Path, nprocs_override: int | None = None) -> tuple[str, str, int]:
    """Run executable, return (stdout, stderr, returncode)."""
    nprocs = nprocs_override if nprocs_override is not None else prob.nprocs
    if prob.run_prefix:
        # Substitute nprocs placeholder if present
        prefix = []
        for tok in prob.run_prefix:
            if tok == "{nprocs}":
                prefix.append(str(nprocs))
            else:
                prefix.append(tok)
        cmd = prefix + [str(exe)]
    else:
        cmd = [str(exe)]

    try:
        r = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
        return r.stdout, r.stderr, r.returncode
    except subprocess.TimeoutExpired:
        return "", "execution timed out (>60s)", -1
    except Exception as e:
        return "", str(e), -1


def _parse_json_output(stdout: str) -> tuple[list[CaseResult], str | None]:
    """Parse JSON output from test harness."""
    # Find JSON block (may have extra output before/after)
    start = stdout.find("{")
    if start == -1:
        return [], f"No JSON output from test executable.\nstdout: {stdout[:500]}"
    try:
        data = json.loads(stdout[start:])
    except json.JSONDecodeError as e:
        return [], f"Failed to parse test output JSON: {e}\nraw: {stdout[:500]}"

    cases = []
    for c in data.get("cases", []):
        cases.append(CaseResult(
            name=c.get("name", ""),
            passed=bool(c.get("passed", False)),
            message=c.get("message", ""),
            elapsed_ms=float(c.get("elapsed_ms", 0.0)),
            stdout=c.get("stdout", ""),
            stderr=c.get("stderr", ""),
        ))
    return cases, None


def run_user_code(
    pid: int,
    user_source: str,
    *,
    nprocs: int | None = None,
    stop_on_first_fail: bool = False,
) -> RunResult:
    prob = find_problem(pid)
    result = RunResult(problem_id=pid, ok=False, total=0, passed=0)

    with tempfile.TemporaryDirectory(prefix="leet380_") as tmpdir:
        tmp = Path(tmpdir)
        solve_cpp = tmp / "solve.cpp"
        exe = tmp / f"p{pid:03d}_test"

        solve_cpp.write_text(user_source)

        ok, compile_out = _compile(prob, solve_cpp, exe)
        result.compile_output = compile_out
        if not ok:
            result.fatal_error = f"Compilation failed:\n{compile_out}"
            return result

        stdout, stderr, retcode = _run_exe(prob, exe, nprocs)

        cases, parse_err = _parse_json_output(stdout)
        if parse_err:
            result.fatal_error = parse_err
            if stderr:
                result.fatal_error += f"\nstderr: {stderr[:500]}"
            return result

        # If stop_on_first_fail, trim cases after first failure
        if stop_on_first_fail:
            trimmed = []
            for c in cases:
                trimmed.append(c)
                if not c.passed:
                    break
            cases = trimmed

        result.cases = cases
        result.total = len(cases)
        result.passed = sum(1 for c in cases if c.passed)
        result.ok = (result.passed == result.total and result.total > 0)

        # Attach runtime stderr to last failed case if any
        if stderr and cases:
            for c in reversed(cases):
                if not c.passed and not c.stderr:
                    c.stderr = stderr[:2000]
                    break

    return result


# ---------------------------------------------------------------------------
# Reference solution runner (for CLI "solve" command)

def run_reference(pid: int) -> RunResult:
    prob = find_problem(pid)
    sol_cpp = prob.path / "solution.cpp"
    if not sol_cpp.exists():
        r = RunResult(problem_id=pid, ok=False, total=0, passed=0)
        r.fatal_error = "solution.cpp not found"
        return r
    return run_user_code(pid, sol_cpp.read_text())


# ---------------------------------------------------------------------------
# User code persistence

def read_user_code(pid: int) -> tuple[str, bool]:
    """Return (source, is_draft). Falls back to starter.cpp."""
    prob = find_problem(pid)
    user = prob.path / "user.cpp"
    if user.exists():
        return user.read_text(), True
    return (prob.path / "starter.cpp").read_text(), False


def write_user_code(pid: int, source: str) -> None:
    find_problem(pid).path.joinpath("user.cpp").write_text(source)


def clear_user_code(pid: int) -> bool:
    p = find_problem(pid).path / "user.cpp"
    if p.exists():
        p.unlink()
        return True
    return False


def read_file(pid: int, name: str) -> str:
    prob = find_problem(pid)
    f = prob.path / name
    if not f.exists():
        raise FileNotFoundError(name)
    return f.read_text()
