// harness.h — shared test harness for leet_380 C++ problems
// Include this in tests.cpp to get RUN_TEST / print_results helpers.
// Output: newline-delimited JSON on stdout, which the Python server parses.
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <string>
#include <functional>
#include <chrono>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <numeric>

struct CaseResult {
    std::string name;
    bool passed;
    std::string message;
    double elapsed_ms;
    std::string stdout_cap;  // not used in C++ version; kept for JSON compat
};

static std::vector<CaseResult> _harness_results;

// JSON-escape a string
static std::string _json_str(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 4);
    for (unsigned char c : s) {
        if (c == '"')  { out += "\\\""; }
        else if (c == '\\') { out += "\\\\"; }
        else if (c == '\n') { out += "\\n"; }
        else if (c == '\r') { out += "\\r"; }
        else if (c == '\t') { out += "\\t"; }
        else if (c < 0x20) { char buf[8]; snprintf(buf,8,"\\u%04x",c); out+=buf; }
        else { out += (char)c; }
    }
    return out;
}

// Check helpers
static inline bool approx_equal(double a, double b, double rtol=1e-4, double atol=1e-5) {
    return fabs(a-b) <= atol + rtol * fabs(b);
}

static inline bool vec_approx_equal(const std::vector<double>& a, const std::vector<double>& b,
                                     double rtol=1e-4, double atol=1e-5) {
    if (a.size() != b.size()) return false;
    for (size_t i=0;i<a.size();i++)
        if (!approx_equal(a[i],b[i],rtol,atol)) return false;
    return true;
}

static inline bool vec_approx_equal(const std::vector<float>& a, const std::vector<float>& b,
                                     float rtol=1e-4f, float atol=1e-5f) {
    if (a.size() != b.size()) return false;
    for (size_t i=0;i<a.size();i++)
        if (fabsf(a[i]-b[i]) > atol + rtol*fabsf(b[i])) return false;
    return true;
}

static inline bool vec_equal(const std::vector<int>& a, const std::vector<int>& b) { return a==b; }
static inline bool vec_equal(const std::vector<unsigned int>& a, const std::vector<unsigned int>& b) { return a==b; }
static inline bool vec_equal(const std::vector<long long>& a, const std::vector<long long>& b) { return a==b; }
static inline bool vec_equal(const std::vector<unsigned long long>& a, const std::vector<unsigned long long>& b) { return a==b; }

// Macros for test registration
#define ASSERT_EQ(a, b) do { \
    if (!((a)==(b))) { \
        std::ostringstream _ss; \
        _ss << "expected " << (b) << ", got " << (a); \
        throw std::runtime_error(_ss.str()); \
    } \
} while(0)

#define ASSERT_APPROX(a, b, ...) do { \
    if (!approx_equal((double)(a),(double)(b),##__VA_ARGS__)) { \
        std::ostringstream _ss; \
        _ss << "expected ~" << (b) << ", got " << (a) \
            << " (diff=" << fabs((double)(a)-(double)(b)) << ")"; \
        throw std::runtime_error(_ss.str()); \
    } \
} while(0)

#define ASSERT_TRUE(cond, msg) do { \
    if (!(cond)) throw std::runtime_error(std::string(msg)); \
} while(0)

#define ASSERT_VEC_EQ(a, b) do { \
    if (!vec_equal((a),(b))) throw std::runtime_error("vector mismatch"); \
} while(0)

#define ASSERT_VEC_APPROX(a, b, ...) do { \
    if (!vec_approx_equal((a),(b),##__VA_ARGS__)) throw std::runtime_error("vector values differ"); \
} while(0)

// Run one test and record result.
// Variadic so lambda bodies containing commas don't confuse the preprocessor.
#define RUN_TEST(test_name, ...) do { \
    auto _test_fn = __VA_ARGS__; \
    auto _t0 = std::chrono::high_resolution_clock::now(); \
    bool _passed = true; \
    std::string _msg = "ok"; \
    try { _test_fn(); } \
    catch (const std::exception& _e) { _passed = false; _msg = _e.what(); } \
    catch (...) { _passed = false; _msg = "unknown exception"; } \
    auto _t1 = std::chrono::high_resolution_clock::now(); \
    double _ms = std::chrono::duration<double,std::milli>(_t1-_t0).count(); \
    _harness_results.push_back({(test_name), _passed, _msg, _ms, ""}); \
} while(0)

// Print all results as JSON to stdout
static void print_results() {
    int total = (int)_harness_results.size();
    printf("{\"total\":%d,\"cases\":[\n", total);
    for (int i=0;i<total;i++) {
        const auto& r = _harness_results[i];
        printf("  {\"name\":\"%s\",\"passed\":%s,\"message\":\"%s\",\"elapsed_ms\":%.2f}%s\n",
            _json_str(r.name).c_str(),
            r.passed ? "true" : "false",
            _json_str(r.message).c_str(),
            r.elapsed_ms,
            i<total-1 ? "," : "");
    }
    printf("]}\n");
    fflush(stdout);
}

// Seeded random number helpers for reproducible tests
static std::vector<double> rand_vec(int n, unsigned seed=42, double lo=-1.0, double hi=1.0) {
    srand(seed);
    std::vector<double> v(n);
    for (auto& x : v) x = lo + (hi-lo)*((double)rand()/RAND_MAX);
    return v;
}
static std::vector<float> rand_vec_f(int n, unsigned seed=42, float lo=-1.f, float hi=1.f) {
    srand(seed);
    std::vector<float> v(n);
    for (auto& x : v) x = lo + (hi-lo)*((float)rand()/RAND_MAX);
    return v;
}
static std::vector<int> rand_vec_i(int n, unsigned seed=42, int lo=0, int hi=100) {
    srand(seed);
    std::vector<int> v(n);
    for (auto& x : v) x = lo + rand()%(hi-lo+1);
    return v;
}
static std::vector<std::vector<double>> rand_mat(int rows, int cols, unsigned seed=42) {
    srand(seed);
    std::vector<std::vector<double>> m(rows, std::vector<double>(cols));
    for (auto& row : m) for (auto& x : row) x = -1.0+2.0*((double)rand()/RAND_MAX);
    return m;
}
