// leet_380 — OMP/MPI local coding platform
// Monaco editor (C++ mode) + FastAPI backend for compile/run.

const state = {
  problems: [],
  current: null,       // {id, starter, markdown, nprocs, compile_cmd, ...}
  view: "mine",        // "mine" | "solution"
  userCode: "",
  solutionCode: null,
  editor: null,
  monaco: null,
  running: false,
  saveTimer: null,
  solvedMap: {},
  filterTopic: "",
};

const SOLVED_KEY = "leet380.solved.v1";

function loadSolvedMap() {
  try { return JSON.parse(localStorage.getItem(SOLVED_KEY) || "{}"); } catch { return {}; }
}
function saveSolvedMap() {
  try { localStorage.setItem(SOLVED_KEY, JSON.stringify(state.solvedMap)); } catch {}
}
function markSolved(pid) {
  const k = String(pid);
  if (state.solvedMap[k]) return;
  state.solvedMap[k] = true;
  saveSolvedMap();
  const li = document.querySelector(`.sidebar li[data-pid="${k}"]`);
  if (li) li.classList.add("solved");
}

// ── bootstrap ────────────────────────────────────────────────────────────────

async function init() {
  setStatus("connecting…");
  try {
    const h = await fetch("/api/health").then(r => r.json());
    const ok = h.compilers["g++"] || h.compilers["mpic++"];
    const parts = [];
    if (h.compilers["g++"]) parts.push("g++");
    if (h.compilers["mpic++"]) parts.push("mpic++");
    parts.push(`${h.cpu_count} cores`);
    setStatus(parts.join(" · "), ok ? "ok" : "bad");
  } catch {
    setStatus("server offline", "bad");
    return;
  }

  state.problems = await fetch("/api/problems").then(r => r.json());
  state.solvedMap = loadSolvedMap();
  renderProblemList();

  await loadMonaco();

  const initial = parseHashPid() || (state.problems[0] && state.problems[0].id);
  if (initial) await loadProblem(initial);

  document.getElementById("run-btn").addEventListener("click", runTests);
  document.getElementById("reset-btn").addEventListener("click", resetToStarter);
  document.getElementById("tab-mine").addEventListener("click", () => switchView("mine"));
  document.getElementById("tab-solution").addEventListener("click", () => switchView("solution"));
  document.getElementById("filter-topic").addEventListener("change", e => {
    state.filterTopic = e.target.value;
    renderProblemList();
  });

  window.addEventListener("hashchange", () => {
    const pid = parseHashPid();
    if (pid && (!state.current || state.current.id !== pid)) loadProblem(pid);
  });
  window.addEventListener("keydown", e => {
    if ((e.ctrlKey || e.metaKey) && e.key === "Enter") { e.preventDefault(); runTests(); }
  });
}

function setStatus(text, cls) {
  const el = document.getElementById("status");
  el.textContent = text;
  el.className = "status" + (cls ? " " + cls : "");
}
function parseHashPid() {
  const m = (location.hash || "").match(/#\/?(\d+)/);
  return m ? parseInt(m[1], 10) : null;
}

// ── problem list ─────────────────────────────────────────────────────────────

function topicTag(p) {
  const cmd = (p.compile_cmd || "g++").toLowerCase();
  if (cmd.includes("mpic")) return "mpi";
  // check nprocs > 1 as secondary signal
  if (p.nprocs > 1) return "mpi";
  return "omp";
}

function renderProblemList() {
  const ul = document.getElementById("problem-list");
  ul.innerHTML = "";
  const filter = state.filterTopic;
  for (const p of state.problems) {
    const tag = topicTag(p);
    if (filter && tag !== filter && !p.slug.startsWith("hybrid")) {
      // "hybrid" filter: show if slug starts with hybrid
      if (!(filter === "hybrid" && p.slug.startsWith("hybrid_"))) continue;
    }
    if (filter === "omp" && (tag === "mpi" || p.slug.startsWith("hybrid_"))) continue;
    if (filter === "mpi" && (tag !== "mpi" || p.slug.startsWith("hybrid_"))) continue;
    if (filter === "hybrid" && !p.slug.startsWith("hybrid_")) continue;

    const li = document.createElement("li");
    li.dataset.pid = p.id;
    const diff = (p.difficulty || "").toLowerCase();
    li.innerHTML = `
      <span class="pid">#${String(p.id).padStart(3, "0")}</span>
      <span class="title"></span>
      <span class="diff ${diff}"></span>`;
    li.querySelector(".title").textContent = p.title;
    li.querySelector(".diff").textContent = p.difficulty || "";
    if (state.solvedMap[String(p.id)]) li.classList.add("solved");
    li.addEventListener("click", () => { location.hash = `#/${p.id}`; });
    ul.appendChild(li);
  }
}

function highlightActive(pid) {
  for (const li of document.querySelectorAll(".sidebar li")) {
    li.classList.toggle("active", String(li.dataset.pid) === String(pid));
  }
}

// ── problem loading ───────────────────────────────────────────────────────────

async function loadProblem(pid) {
  const [meta, code] = await Promise.all([
    fetch(`/api/problems/${pid}`).then(r => { if (!r.ok) throw new Error("not found"); return r.json(); }),
    fetch(`/api/problems/${pid}/code`).then(r => r.json()),
  ]);
  state.current = meta;
  state.userCode = code.source;
  state.solutionCode = null;
  state.view = "mine";
  highlightActive(pid);
  document.getElementById("pid-label").textContent = `#${String(pid).padStart(3, "0")}`;
  document.getElementById("problem-md").innerHTML = marked.parse(meta.markdown);
  document.getElementById("problem-md").scrollTop = 0;

  // Show compile info
  const ci = document.getElementById("compile-info");
  const flags = (meta.compile_flags || []).join(" ");
  ci.textContent = `${meta.compile_cmd} ${flags}`;

  // Set nprocs default
  const nprocsInput = document.getElementById("nprocs-input");
  nprocsInput.value = "";
  nprocsInput.placeholder = `default: ${meta.nprocs || 1}`;

  setEditorValue(state.userCode);
  updateTabUI();
  document.getElementById("results").innerHTML = "";
}

// ── autosave ─────────────────────────────────────────────────────────────────

function scheduleSave() {
  if (!state.current || state.view !== "mine") return;
  clearTimeout(state.saveTimer);
  state.saveTimer = setTimeout(async () => {
    const src = state.editor.getValue();
    state.userCode = src;
    try {
      await fetch(`/api/problems/${state.current.id}/code`, {
        method: "PUT",
        headers: { "content-type": "application/json" },
        body: JSON.stringify({ source: src }),
      });
    } catch (e) { console.warn("autosave failed", e); }
  }, 800);
}

// ── view toggle ───────────────────────────────────────────────────────────────

function updateTabUI() {
  document.getElementById("tab-mine").classList.toggle("active", state.view === "mine");
  document.getElementById("tab-solution").classList.toggle("active", state.view === "solution");
  document.getElementById("reset-btn").style.display = state.view === "mine" ? "" : "none";
  document.getElementById("run-btn").disabled = state.view !== "mine";
}

async function switchView(view) {
  if (!state.current || state.view === view) return;
  if (state.view === "mine") state.userCode = state.editor.getValue();
  state.view = view;
  updateTabUI();
  if (view === "solution") {
    if (state.solutionCode == null) {
      const { source } = await fetch(`/api/problems/${state.current.id}/solution`).then(r => r.json());
      state.solutionCode = source;
    }
    setEditorValue(state.solutionCode, true);
  } else {
    setEditorValue(state.userCode, false);
  }
}

// ── Monaco ────────────────────────────────────────────────────────────────────

function loadMonaco() {
  return new Promise(resolve => {
    require.config({ paths: { vs: "https://cdn.jsdelivr.net/npm/monaco-editor@0.45.0/min/vs" } });
    require(["vs/editor/editor.main"], () => {
      state.monaco = monaco;
      state.editor = monaco.editor.create(document.getElementById("editor"), {
        value: "",
        language: "cpp",
        theme: "vs-dark",
        fontSize: 13,
        automaticLayout: true,
        minimap: { enabled: false },
        scrollBeyondLastLine: false,
        tabSize: 4,
        insertSpaces: true,
        wordBasedSuggestions: "allDocuments",
        suggestOnTriggerCharacters: true,
        quickSuggestionsDelay: 50,
        quickSuggestions: { other: true, comments: false, strings: false },
        fixedOverflowWidgets: true,
        parameterHints: { enabled: true },
        bracketPairColorization: { enabled: true },
        guides: { indentation: true, bracketPairs: true },
        smoothScrolling: true,
      });

      state.editor.onDidChangeModelContent(() => {
        scheduleSave();
      });

      registerCppSnippets();
      resolve();
    });
  });
}

function setEditorValue(value, readOnly = false) {
  if (!state.editor) return;
  state.editor.updateOptions({ readOnly });
  const pos = state.editor.getPosition();
  state.editor.setValue(value);
  if (pos) state.editor.setPosition(pos);
}

// ── C++ snippet completion ────────────────────────────────────────────────────

function registerCppSnippets() {
  const m = state.monaco;

  // OMP + MPI keyword/snippet completion
  const snippets = [
    // OMP pragmas
    { label: "#pragma omp parallel", kind: 14, insertText: "#pragma omp parallel\n{\n\t$0\n}" },
    { label: "#pragma omp parallel for", kind: 14, insertText: "#pragma omp parallel for\nfor (int ${1:i} = 0; ${1:i} < ${2:n}; ++${1:i}) {\n\t$0\n}" },
    { label: "#pragma omp parallel for reduction", kind: 14, insertText: "#pragma omp parallel for reduction(${1:+}:${2:sum})\nfor (int ${3:i} = 0; ${3:i} < ${4:n}; ++${3:i}) {\n\t$0\n}" },
    { label: "#pragma omp critical", kind: 14, insertText: "#pragma omp critical\n{\n\t$0\n}" },
    { label: "#pragma omp atomic", kind: 14, insertText: "#pragma omp atomic\n$0" },
    { label: "#pragma omp barrier", kind: 14, insertText: "#pragma omp barrier" },
    { label: "#pragma omp single", kind: 14, insertText: "#pragma omp single\n{\n\t$0\n}" },
    { label: "#pragma omp master", kind: 14, insertText: "#pragma omp master\n{\n\t$0\n}" },
    { label: "#pragma omp task", kind: 14, insertText: "#pragma omp task\n{\n\t$0\n}" },
    { label: "#pragma omp taskwait", kind: 14, insertText: "#pragma omp taskwait" },
    { label: "#pragma omp for schedule", kind: 14, insertText: "#pragma omp for schedule(${1|static,dynamic,guided|}${2:, chunk})" },
    { label: "omp_get_thread_num", kind: 1, insertText: "omp_get_thread_num()" },
    { label: "omp_get_num_threads", kind: 1, insertText: "omp_get_num_threads()" },
    { label: "omp_get_max_threads", kind: 1, insertText: "omp_get_max_threads()" },
    { label: "omp_set_num_threads", kind: 1, insertText: "omp_set_num_threads(${1:n})" },
    { label: "omp_init_lock", kind: 1, insertText: "omp_init_lock(&${1:lock})" },
    { label: "omp_set_lock", kind: 1, insertText: "omp_set_lock(&${1:lock})" },
    { label: "omp_unset_lock", kind: 1, insertText: "omp_unset_lock(&${1:lock})" },
    // MPI calls
    { label: "MPI_Init", kind: 1, insertText: "MPI_Init(&argc, &argv)" },
    { label: "MPI_Finalize", kind: 1, insertText: "MPI_Finalize()" },
    { label: "MPI_Comm_rank", kind: 1, insertText: "MPI_Comm_rank(MPI_COMM_WORLD, &${1:rank})" },
    { label: "MPI_Comm_size", kind: 1, insertText: "MPI_Comm_size(MPI_COMM_WORLD, &${1:size})" },
    { label: "MPI_Send", kind: 1, insertText: "MPI_Send(${1:buf}, ${2:count}, ${3:MPI_INT}, ${4:dest}, ${5:tag}, MPI_COMM_WORLD)" },
    { label: "MPI_Recv", kind: 1, insertText: "MPI_Recv(${1:buf}, ${2:count}, ${3:MPI_INT}, ${4:src}, ${5:tag}, MPI_COMM_WORLD, MPI_STATUS_IGNORE)" },
    { label: "MPI_Isend", kind: 1, insertText: "MPI_Isend(${1:buf}, ${2:count}, ${3:MPI_INT}, ${4:dest}, ${5:tag}, MPI_COMM_WORLD, &${6:req})" },
    { label: "MPI_Irecv", kind: 1, insertText: "MPI_Irecv(${1:buf}, ${2:count}, ${3:MPI_INT}, ${4:src}, ${5:tag}, MPI_COMM_WORLD, &${6:req})" },
    { label: "MPI_Wait", kind: 1, insertText: "MPI_Wait(&${1:req}, MPI_STATUS_IGNORE)" },
    { label: "MPI_Waitall", kind: 1, insertText: "MPI_Waitall(${1:count}, ${2:reqs}, MPI_STATUSES_IGNORE)" },
    { label: "MPI_Bcast", kind: 1, insertText: "MPI_Bcast(${1:buf}, ${2:count}, ${3:MPI_INT}, ${4:root}, MPI_COMM_WORLD)" },
    { label: "MPI_Reduce", kind: 1, insertText: "MPI_Reduce(${1:sendbuf}, ${2:recvbuf}, ${3:count}, ${4:MPI_DOUBLE}, ${5:MPI_SUM}, ${6:root}, MPI_COMM_WORLD)" },
    { label: "MPI_Allreduce", kind: 1, insertText: "MPI_Allreduce(${1:sendbuf}, ${2:recvbuf}, ${3:count}, ${4:MPI_DOUBLE}, ${5:MPI_SUM}, MPI_COMM_WORLD)" },
    { label: "MPI_Scatter", kind: 1, insertText: "MPI_Scatter(${1:sendbuf}, ${2:sendcount}, ${3:MPI_DOUBLE}, ${4:recvbuf}, ${5:recvcount}, ${6:MPI_DOUBLE}, ${7:root}, MPI_COMM_WORLD)" },
    { label: "MPI_Gather", kind: 1, insertText: "MPI_Gather(${1:sendbuf}, ${2:sendcount}, ${3:MPI_DOUBLE}, ${4:recvbuf}, ${5:recvcount}, ${6:MPI_DOUBLE}, ${7:root}, MPI_COMM_WORLD)" },
    { label: "MPI_Scatterv", kind: 1, insertText: "MPI_Scatterv(${1:sendbuf}, ${2:sendcounts}, ${3:displs}, ${4:MPI_DOUBLE}, ${5:recvbuf}, ${6:recvcount}, ${7:MPI_DOUBLE}, ${8:root}, MPI_COMM_WORLD)" },
    { label: "MPI_Gatherv", kind: 1, insertText: "MPI_Gatherv(${1:sendbuf}, ${2:sendcount}, ${3:MPI_DOUBLE}, ${4:recvbuf}, ${5:recvcounts}, ${6:displs}, ${7:MPI_DOUBLE}, ${8:root}, MPI_COMM_WORLD)" },
    { label: "MPI_Allgather", kind: 1, insertText: "MPI_Allgather(${1:sendbuf}, ${2:sendcount}, ${3:MPI_DOUBLE}, ${4:recvbuf}, ${5:recvcount}, ${6:MPI_DOUBLE}, MPI_COMM_WORLD)" },
    { label: "MPI_Alltoall", kind: 1, insertText: "MPI_Alltoall(${1:sendbuf}, ${2:sendcount}, ${3:MPI_DOUBLE}, ${4:recvbuf}, ${5:recvcount}, ${6:MPI_DOUBLE}, MPI_COMM_WORLD)" },
    { label: "MPI_Scan", kind: 1, insertText: "MPI_Scan(${1:sendbuf}, ${2:recvbuf}, ${3:count}, ${4:MPI_DOUBLE}, ${5:MPI_SUM}, MPI_COMM_WORLD)" },
    { label: "MPI_Exscan", kind: 1, insertText: "MPI_Exscan(${1:sendbuf}, ${2:recvbuf}, ${3:count}, ${4:MPI_DOUBLE}, ${5:MPI_SUM}, MPI_COMM_WORLD)" },
    { label: "MPI_Barrier", kind: 1, insertText: "MPI_Barrier(MPI_COMM_WORLD)" },
    { label: "MPI_Wtime", kind: 1, insertText: "MPI_Wtime()" },
    { label: "MPI_Type_commit", kind: 1, insertText: "MPI_Type_commit(&${1:type})" },
    { label: "MPI_Type_free", kind: 1, insertText: "MPI_Type_free(&${1:type})" },
    { label: "MPI_Type_create_struct", kind: 1, insertText: "MPI_Type_create_struct(${1:count}, ${2:blocklengths}, ${3:offsets}, ${4:types}, &${5:newtype})" },
    { label: "MPI_Cart_create", kind: 1, insertText: "MPI_Cart_create(MPI_COMM_WORLD, ${1:ndims}, ${2:dims}, ${3:periods}, ${4:reorder}, &${5:comm_cart})" },
    { label: "MPI_Cart_shift", kind: 1, insertText: "MPI_Cart_shift(${1:comm_cart}, ${2:direction}, ${3:disp}, &${4:src}, &${5:dest})" },
    { label: "MPI_Cart_coords", kind: 1, insertText: "MPI_Cart_coords(${1:comm_cart}, ${2:rank}, ${3:ndims}, ${4:coords})" },
  ];

  m.languages.registerCompletionItemProvider("cpp", {
    triggerCharacters: ["_", "M", "o", "#"],
    provideCompletionItems: (model, position) => {
      const word = model.getWordUntilPosition(position);
      const range = new m.Range(position.lineNumber, word.startColumn, position.lineNumber, word.endColumn);
      const linePrefix = model.getLineContent(position.lineNumber).substring(0, position.column - 1);
      return {
        suggestions: snippets.map(s => ({
          label: s.label,
          kind: s.kind,
          insertText: s.insertText,
          insertTextRules: 4, // InsertAsSnippet
          range,
          detail: s.label.startsWith("#pragma omp") ? "OpenMP" : s.label.startsWith("MPI_") ? "MPI" : "OMP runtime",
        })),
      };
    },
  });
}

// ── run tests ─────────────────────────────────────────────────────────────────

async function runTests() {
  if (!state.current || state.running || state.view !== "mine") return;
  const btn = document.getElementById("run-btn");
  state.running = true; btn.disabled = true; btn.textContent = "Compiling…";
  const nprocsRaw = document.getElementById("nprocs-input").value;
  const nprocs = nprocsRaw ? parseInt(nprocsRaw, 10) : null;
  const stop = document.getElementById("stop-first").checked;
  const src = state.editor.getValue();
  state.userCode = src;
  clearTimeout(state.saveTimer);
  try {
    await fetch(`/api/problems/${state.current.id}/code`, {
      method: "PUT", headers: { "content-type": "application/json" },
      body: JSON.stringify({ source: src }),
    });
    btn.textContent = "Running…";
    const res = await fetch(`/api/problems/${state.current.id}/run`, {
      method: "POST",
      headers: { "content-type": "application/json" },
      body: JSON.stringify({ source: src, nprocs, stop_on_first_fail: stop }),
    }).then(r => r.json());
    renderResults(res);
    if (res.ok) markSolved(state.current.id);
  } catch (e) {
    renderResults({ fatal_error: `Network error: ${e.message}`, cases: [], total: 0, passed: 0, ok: false, static_issues: [], compile_output: "" });
  } finally {
    state.running = false; btn.disabled = state.view !== "mine"; btn.textContent = "Run tests";
  }
}

async function resetToStarter() {
  if (!state.current) return;
  if (!confirm("Reset to starter code? Draft will be deleted.")) return;
  const { source } = await fetch(`/api/problems/${state.current.id}/code`, { method: "DELETE" }).then(r => r.json());
  state.userCode = source;
  setEditorValue(source, false);
}

// ── render results ────────────────────────────────────────────────────────────

function renderResults(res) {
  const el = document.getElementById("results");
  el.innerHTML = "";

  if (res.compile_output) {
    const d = document.createElement("div");
    d.className = "compile-out" + (res.fatal_error ? " bad" : " ok");
    d.innerHTML = `<div class="stream-label">compiler output</div><pre class="stream-body"></pre>`;
    d.querySelector(".stream-body").textContent = res.compile_output;
    el.appendChild(d);
  }

  if (res.fatal_error) {
    const d = document.createElement("div");
    d.className = "fatal";
    d.textContent = res.fatal_error;
    el.appendChild(d);
    return;
  }
  if (res.static_issues && res.static_issues.length) {
    const d = document.createElement("div");
    d.className = "static";
    d.textContent = res.static_issues.join("\n");
    el.appendChild(d);
    return;
  }

  const summary = document.createElement("div");
  summary.className = "summary " + (res.ok ? "ok" : "bad");
  summary.textContent = `${res.passed}/${res.total} tests passed`;
  el.appendChild(summary);

  for (const c of res.cases) {
    const row = document.createElement("div");
    row.className = "case " + (c.passed ? "pass" : "fail");
    row.innerHTML = `<span class="tag"></span><span class="name"></span><span class="elapsed"></span>`;
    row.querySelector(".tag").textContent = c.passed ? "PASS" : "FAIL";
    row.querySelector(".name").textContent = c.name;
    row.querySelector(".elapsed").textContent = c.elapsed_ms ? c.elapsed_ms.toFixed(1) + " ms" : "";
    el.appendChild(row);

    if (!c.passed && c.message) {
      const msg = document.createElement("div");
      msg.className = "msg";
      msg.textContent = c.message;
      el.appendChild(msg);
    }
    for (const [label, text] of [["stdout", c.stdout], ["stderr", c.stderr]]) {
      if (text) {
        const block = document.createElement("div");
        block.className = `stream ${label}`;
        block.innerHTML = `<div class="stream-label">${label}</div><pre class="stream-body"></pre>`;
        block.querySelector(".stream-body").textContent = text;
        el.appendChild(block);
      }
    }
  }
}

init();
