#!/usr/bin/env bash
# run_all_tests.sh — compile and run all leet_380 problems
# Usage: ./run_all_tests.sh [pid_start] [pid_end]
# Output: results logged to run_all_tests.log

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROBLEMS_DIR="$SCRIPT_DIR/problems"
SERVER_DIR="$SCRIPT_DIR/server"
HARNESS_H="$SERVER_DIR/harness.h"
LOG="$SCRIPT_DIR/run_all_tests.log"
TMPDIR_BASE="/tmp/leet380_runall"

PID_START="${1:-1}"
PID_END="${2:-999}"

# Color codes
G="\033[32m"; R="\033[31m"; Y="\033[33m"; B="\033[34m"; D="\033[2m"; X="\033[0m"

pass=0; fail=0; skip=0; total=0

rm -f "$LOG"
echo "leet_380 test run — $(date)" | tee "$LOG"
echo "==========================================" | tee -a "$LOG"

for prob_dir in $(ls -d "$PROBLEMS_DIR"/[0-9]* | sort); do
    folder=$(basename "$prob_dir")
    pid="${folder%%_*}"
    pid=$((10#$pid))   # strip leading zeros

    [ "$pid" -lt "$PID_START" ] && continue
    [ "$pid" -gt "$PID_END" ]   && continue

    name="${folder#*_}"
    total=$((total + 1))

    # Read metadata
    compile_cmd="g++"
    compile_flags="-O2 -std=c++17 -fopenmp"
    run_prefix=""
    nprocs=1

    if [ -f "$prob_dir/tests.py" ]; then
        compile_cmd=$(python3 -c "
import sys; ns={}; exec(open('$prob_dir/tests.py').read(), ns)
print(ns.get('COMPILE_CMD','g++'))
" 2>/dev/null || echo "g++")
        compile_flags=$(python3 -c "
import sys; ns={}; exec(open('$prob_dir/tests.py').read(), ns)
print(' '.join(ns.get('COMPILE_FLAGS',['-O2','-std=c++17','-fopenmp'])))
" 2>/dev/null || echo "-O2 -std=c++17 -fopenmp")
        run_prefix=$(python3 -c "
import sys; ns={}; exec(open('$prob_dir/tests.py').read(), ns)
print(' '.join(ns.get('RUN_PREFIX',[])))
" 2>/dev/null || echo "")
        nprocs=$(python3 -c "
import sys; ns={}; exec(open('$prob_dir/tests.py').read(), ns)
print(ns.get('NPROCS',1))
" 2>/dev/null || echo "1")
    fi

    # Check required files exist
    if [ ! -f "$prob_dir/tests.cpp" ] || [ ! -f "$prob_dir/solution.cpp" ]; then
        echo -e "${Y}SKIP${X} #$(printf '%03d' $pid) $name — missing tests.cpp or solution.cpp" | tee -a "$LOG"
        skip=$((skip + 1))
        continue
    fi

    # Build in temp dir
    tmpdir="$TMPDIR_BASE/p$(printf '%03d' $pid)"
    mkdir -p "$tmpdir"
    cp "$prob_dir/solution.cpp" "$tmpdir/solve.cpp"

    # Generate wrapper
    wrapper="$tmpdir/wrapper.cpp"
    printf '#include "%s"\n#include "%s"\n' "$tmpdir/solve.cpp" "$prob_dir/tests.cpp" > "$wrapper"

    exe="$tmpdir/test_exe"

    # Compile
    compile_out=$($compile_cmd $compile_flags -I"$SERVER_DIR" "$wrapper" -o "$exe" 2>&1) || {
        echo -e "${R}COMPILE FAIL${X} #$(printf '%03d' $pid) $name" | tee -a "$LOG"
        echo "  Compiler: $compile_cmd $compile_flags" | tee -a "$LOG"
        echo "$compile_out" | head -20 | sed 's/^/  /' | tee -a "$LOG"
        fail=$((fail + 1))
        continue
    }

    # Run (60s timeout to catch MPI deadlocks)
    if [ -n "$run_prefix" ]; then
        run_out=$(timeout 60 bash -c "$run_prefix \"$exe\"" 2>&1) || run_status=$?
    else
        run_out=$(timeout 60 "$exe" 2>&1) || run_status=$?
    fi
    if [ "${run_status:-0}" -eq 124 ] 2>/dev/null; then
        echo -e "${R}TIMEOUT${X} #$(printf '%03d' $pid) $name — killed after 60s" | tee -a "$LOG"
        fail=$((fail + 1))
        continue
    fi

    # Parse results — harness prints multi-line JSON; collect all, parse whole blob
    parse_result=$(echo "$run_out" | python3 -c "
import sys, json, re
text = sys.stdin.read()
# find outermost {...} block
start = text.find('{')
if start == -1:
    print('0\n0\n')
    sys.exit(0)
# find matching closing brace
depth = 0
end = -1
for i, c in enumerate(text[start:], start):
    if c == '{': depth += 1
    elif c == '}':
        depth -= 1
        if depth == 0:
            end = i + 1
            break
if end == -1:
    print('0\n0\n')
    sys.exit(0)
try:
    d = json.loads(text[start:end])
    cases = d.get('cases', [])
    total = d.get('total', len(cases))
    passed = sum(1 for c in cases if c.get('passed', False))
    fails = [c['name'] + ': ' + c.get('message','') for c in cases if not c.get('passed', False)]
    print(total)
    print(passed)
    for f in fails: print('  FAIL: ' + f)
except Exception as e:
    print('0\n0\nparse error: ' + str(e))
" 2>/dev/null)

    test_total=$(echo "$parse_result" | sed -n '1p')
    test_passed=$(echo "$parse_result" | sed -n '2p')
    failed_names=$(echo "$parse_result" | tail -n +3)

    if [ "${test_passed:-0}" = "${test_total:-0}" ] && [ "${test_total:-0}" != "0" ]; then
        echo -e "${G}PASS${X} #$(printf '%03d' $pid) $name  ${D}[$test_passed/$test_total tests]${X}" | tee -a "$LOG"
        pass=$((pass + 1))
    else
        echo -e "${R}FAIL${X} #$(printf '%03d' $pid) $name  ${D}[$test_passed/$test_total tests]${X}" | tee -a "$LOG"
        [ -n "$failed_names" ] && echo "$failed_names" | tee -a "$LOG"
        fail=$((fail + 1))
    fi

    # Cleanup
    rm -rf "$tmpdir"
done

echo "" | tee -a "$LOG"
echo "==========================================" | tee -a "$LOG"
echo -e "${G}PASS: $pass${X}  ${R}FAIL: $fail${X}  ${Y}SKIP: $skip${X}  TOTAL: $total" | tee -a "$LOG"
echo "Full log: $LOG"

[ "$fail" -eq 0 ] && exit 0 || exit 1
