"""FastAPI backend for leet_380 — local OMP/MPI coding platform."""
from __future__ import annotations

import os
import shutil
import subprocess
from dataclasses import asdict
from pathlib import Path
from typing import Any

from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import FileResponse
from fastapi.staticfiles import StaticFiles
from pydantic import BaseModel

from .runner import (
    PROBLEMS_DIR,
    clear_user_code,
    find_problem,
    list_problems,
    read_file,
    read_user_code,
    run_user_code,
    write_user_code,
)

WEB_DIR = Path(__file__).resolve().parent.parent / "web"

app = FastAPI(title="leet_380", version="0.1")
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"], allow_methods=["*"], allow_headers=["*"],
)


# ---------- models

class RunRequest(BaseModel):
    source: str
    nprocs: int | None = None
    stop_on_first_fail: bool = False


class CodeRequest(BaseModel):
    source: str


# ---------- api

@app.get("/api/health")
def health() -> dict[str, Any]:
    # Check available compilers
    compilers = {}
    for cmd in ["g++", "mpic++", "mpirun"]:
        compilers[cmd] = shutil.which(cmd) is not None
    cpu_count = os.cpu_count() or 1
    return {"ok": True, "compilers": compilers, "cpu_count": cpu_count}


@app.get("/api/problems")
def api_problems():
    return [
        {"id": p.pid, "slug": p.slug, "title": p.title,
         "difficulty": p.difficulty, "topic": p.topic,
         "nprocs": p.nprocs, "compile_cmd": p.compile_cmd}
        for p in list_problems()
    ]


@app.get("/api/problems/{pid}")
def api_problem(pid: int):
    try:
        p = find_problem(pid)
    except FileNotFoundError:
        raise HTTPException(status_code=404, detail=f"problem {pid} not found")
    return {
        "id": p.pid,
        "slug": p.slug,
        "title": p.title,
        "difficulty": p.difficulty,
        "topic": p.topic,
        "nprocs": p.nprocs,
        "compile_cmd": p.compile_cmd,
        "compile_flags": p.compile_flags,
        "markdown": (p.path / "problem.md").read_text(),
        "starter": (p.path / "starter.cpp").read_text(),
    }


@app.get("/api/problems/{pid}/solution")
def api_solution(pid: int):
    try:
        p = find_problem(pid)
    except FileNotFoundError:
        raise HTTPException(status_code=404, detail=f"problem {pid} not found")
    return {"source": (p.path / "solution.cpp").read_text()}


@app.get("/api/problems/{pid}/code")
def api_get_code(pid: int):
    try:
        find_problem(pid)
    except FileNotFoundError:
        raise HTTPException(status_code=404, detail=f"problem {pid} not found")
    source, is_draft = read_user_code(pid)
    return {"source": source, "is_draft": is_draft}


@app.put("/api/problems/{pid}/code")
def api_put_code(pid: int, req: CodeRequest):
    try:
        find_problem(pid)
    except FileNotFoundError:
        raise HTTPException(status_code=404, detail=f"problem {pid} not found")
    write_user_code(pid, req.source)
    return {"ok": True}


@app.delete("/api/problems/{pid}/code")
def api_reset_code(pid: int):
    try:
        find_problem(pid)
    except FileNotFoundError:
        raise HTTPException(status_code=404, detail=f"problem {pid} not found")
    cleared = clear_user_code(pid)
    source, is_draft = read_user_code(pid)
    return {"ok": True, "cleared": cleared, "source": source, "is_draft": is_draft}


@app.post("/api/problems/{pid}/run")
def api_run(pid: int, req: RunRequest):
    try:
        find_problem(pid)
    except FileNotFoundError:
        raise HTTPException(status_code=404, detail=f"problem {pid} not found")

    result = run_user_code(
        pid,
        req.source,
        nprocs=req.nprocs,
        stop_on_first_fail=req.stop_on_first_fail,
    )
    return {
        "ok": result.ok,
        "total": result.total,
        "passed": result.passed,
        "fatal_error": result.fatal_error,
        "static_issues": result.static_issues,
        "compile_output": result.compile_output,
        "cases": [asdict(c) for c in result.cases],
    }


# ---------- static frontend

@app.get("/")
def index():
    return FileResponse(WEB_DIR / "index.html")


app.mount("/static", StaticFiles(directory=str(WEB_DIR)), name="static")


def main():
    import uvicorn
    host = os.environ.get("HOST", "0.0.0.0")
    port = int(os.environ.get("PORT", "8380"))
    uvicorn.run("server.app:app", host=host, port=port, reload=False)


if __name__ == "__main__":
    main()
