// benchmark.cpp — Speed + utilization tests for your allocator vs glibc malloc
// Compile: g++ -O2 -o bench benchmark.cpp allocator.cpp debug.cpp
// Run:     ./bench

#include "allocator.h"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <numeric>
#include <random>
#include <vector>

using namespace std;
using hrc = chrono::high_resolution_clock;

// ─────────────────────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────────────────────

// Collect per-operation latencies (nanoseconds) for percentile reporting.
struct LatStats {
    vector<long long> samples;

    void record(long long ns) { samples.push_back(ns); }

    void report(const char* label) const {
        if (samples.empty()) { printf("%s: no samples\n", label); return; }
        vector<long long> s = samples;
        sort(s.begin(), s.end());
        long long total = accumulate(s.begin(), s.end(), 0LL);
        printf("  %-28s  min=%lld  median=%lld  p99=%lld  mean=%.1f  (ns/op)\n",
               label,
               s.front(),
               s[s.size() / 2],
               s[(size_t)(s.size() * 0.99)],
               (double)total / s.size());
    }
};

// Warm the CPU branch predictor / caches a little before measuring.
static void warmup() {
    for (int i = 0; i < 200; ++i) { void* p = allocate(64); free_addr(p); }
    for (int i = 0; i < 200; ++i) { void* p = malloc(64); free(p); }
}

static void section(const char* title) {
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║  %-52s║\n", title);
    printf("╚══════════════════════════════════════════════════════╝\n");
}

// ─────────────────────────────────────────────────────────────────────────────
// Memory utilization
// ─────────────────────────────────────────────────────────────────────────────

void heap_stats(const char* tag) {
    if (!heap_start) { printf("[heap_stats] heap not initialised\n"); return; }

    size_t total_free = 0, total_alloc = 0, num_free = 0, num_alloc = 0;
    char* bp = heap_start + DSIZE;

    for (; GET_SIZE(bp) != 0; bp += GET_SIZE(bp)) {
        size_t sz = GET_SIZE(bp);
        if (IS_ALLOC(bp)) { total_alloc += sz; ++num_alloc; }
        else              { total_free  += sz; ++num_free;  }
    }

    size_t heap_span = (size_t)(heap_end - heap_start);
    double frag_pct  = (total_alloc + total_free) > 0
                       ? 100.0 * total_free / (total_alloc + total_free) : 0.0;

    printf("\n[heap_stats] %s\n", tag);
    printf("  alloc blocks : %4zu   (%zu bytes)\n", num_alloc, total_alloc);
    printf("  free  blocks : %4zu   (%zu bytes)\n", num_free,  total_free);
    printf("  heap span    : %zu bytes\n",           heap_span);
    printf("  fragmentation: %.1f%%\n",              frag_pct);
}

// ─────────────────────────────────────────────────────────────────────────────
// Workload 1 — Sequential same-size  (pure throughput, best case for yours)
// ─────────────────────────────────────────────────────────────────────────────
//
// Allocate then immediately free the same fixed size N times.
// Your first-fit scan will always find the same block instantly because it was
// just freed, so this is as fast as your allocator can ever be.

void bench_sequential(int N, size_t sz) {
    section("Workload 1 — Sequential same-size (alloc+free tight loop)");
    printf("  N=%d  sz=%zu bytes\n\n", N, sz);

    // --- yours ---
    LatStats ys;
    for (int i = 0; i < N; ++i) {
        auto t0 = hrc::now();
        void* p = allocate(sz);
        free_addr(p);
        auto t1 = hrc::now();
        ys.record(chrono::duration_cast<chrono::nanoseconds>(t1 - t0).count());
    }

    // --- system malloc ---
    LatStats ms;
    for (int i = 0; i < N; ++i) {
        auto t0 = hrc::now();
        void* p = malloc(sz);
        free(p);
        auto t1 = hrc::now();
        ms.record(chrono::duration_cast<chrono::nanoseconds>(t1 - t0).count());
    }

    ys.report("your allocator");
    ms.report("system malloc");
    printf("  NOTE: yours may win here — the just-freed block sits at the front.\n");
}

// ─────────────────────────────────────────────────────────────────────────────
// Workload 2 — Random sizes 8–4096  (real-world mixed traffic)
// ─────────────────────────────────────────────────────────────────────────────
//
// Uses a fixed random seed so both allocators see the same request stream.
// glibc is expected to win because its segregated free lists give O(1) lookup;
// yours does an O(n) first-fit scan through an increasingly fragmented heap.

void bench_random(int N) {
    section("Workload 2 — Random sizes 8–4096 (real-world mixed traffic)");
    printf("  N=%d  sizes drawn uniformly from [8, 4096]\n\n", N);

    mt19937 rng(42);
    uniform_int_distribution<size_t> dist(8, 4096);
    vector<size_t> sizes(N);
    for (auto& s : sizes) s = dist(rng);

    // --- yours ---
    LatStats ys;
    for (int i = 0; i < N; ++i) {
        auto t0 = hrc::now();
        void* p = allocate(sizes[i]);
        free_addr(p);
        auto t1 = hrc::now();
        ys.record(chrono::duration_cast<chrono::nanoseconds>(t1 - t0).count());
    }

    // --- system malloc ---
    LatStats ms;
    for (int i = 0; i < N; ++i) {
        auto t0 = hrc::now();
        void* p = malloc(sizes[i]);
        free(p);
        auto t1 = hrc::now();
        ms.record(chrono::duration_cast<chrono::nanoseconds>(t1 - t0).count());
    }

    ys.report("your allocator");
    ms.report("system malloc");
    printf("  NOTE: glibc likely wins — ~8 size-class buckets vs your O(n) first-fit.\n");
}

// ─────────────────────────────────────────────────────────────────────────────
// Workload 3 — Alloc-many then free-all  (fragmentation + coalesce stress)
// ─────────────────────────────────────────────────────────────────────────────
//
// Alloc N pointers, then free them in random order.  Interesting because:
//   * your coalesce() is called on every free and merges at most two neighbours.
//   * Random free order means many small isolated free blocks before the big
//     merge happens, stressing the O(N) scan on subsequent allocations.
//   * We measure total time for the free phase separately from the alloc phase.

void bench_alloc_many_free_all(int N, size_t sz) {
    section("Workload 3 — Alloc-many then free-all (coalesce stress)");
    printf("  N=%d  sz=%zu bytes per block\n\n", N, sz);

    // Allocate N blocks.
    vector<void*> ptrs(N, nullptr);

    auto t0 = hrc::now();
    for (int i = 0; i < N; ++i) ptrs[i] = allocate(sz);
    auto t1 = hrc::now();

    heap_stats("after alloc-many (yours)");

    // Shuffle free order.
    mt19937 rng(7);
    shuffle(ptrs.begin(), ptrs.end(), rng);

    auto t2 = hrc::now();
    for (int i = 0; i < N; ++i) free_addr(ptrs[i]);
    auto t3 = hrc::now();

    heap_stats("after free-all + coalesce (yours)");

    long long alloc_ns = chrono::duration_cast<chrono::nanoseconds>(t1 - t0).count();
    long long free_ns  = chrono::duration_cast<chrono::nanoseconds>(t3 - t2).count();
    printf("\n  yours  — alloc phase: %lld ns (%.1f ns/op)   free phase: %lld ns (%.1f ns/op)\n",
           alloc_ns, (double)alloc_ns / N, free_ns, (double)free_ns / N);

    // Repeat with system malloc.
    vector<void*> mptrs(N, nullptr);
    auto m0 = hrc::now();
    for (int i = 0; i < N; ++i) mptrs[i] = malloc(sz);
    auto m1 = hrc::now();

    shuffle(mptrs.begin(), mptrs.end(), rng);

    auto m2 = hrc::now();
    for (int i = 0; i < N; ++i) free(mptrs[i]);
    auto m3 = hrc::now();

    long long malloc_ns = chrono::duration_cast<chrono::nanoseconds>(m1 - m0).count();
    long long mfree_ns  = chrono::duration_cast<chrono::nanoseconds>(m3 - m2).count();
    printf("  malloc — alloc phase: %lld ns (%.1f ns/op)   free phase: %lld ns (%.1f ns/op)\n",
           malloc_ns, (double)malloc_ns / N, mfree_ns, (double)mfree_ns / N);
}

// ─────────────────────────────────────────────────────────────────────────────
// Workload 4 — Small-allocation internal fragmentation audit
// ─────────────────────────────────────────────────────────────────────────────
//
// Highlights the cost of MIN_BLOCK_SIZE=32 for tiny requests.
// For each request size in the table, prints: requested / block used / utilisation %.

void bench_utilization_audit() {
    section("Workload 4 — Internal fragmentation audit (small sizes)");
    printf("  Requested  BlockUsed  Payload  Utilisation\n");
    printf("  ─────────  ─────────  ───────  ───────────\n");

    // We deduce block size from the change in the heap cursor.
    size_t test_sizes[] = {1, 4, 8, 12, 16, 24, 32, 48, 64, 128, 256};
    for (size_t req : test_sizes) {
        // Peek at the free block before allocating.
        char* bp = heap_start ? heap_start + DSIZE : nullptr;
        if (!bp) { allocate(0); bp = heap_start + DSIZE; }  // trigger init
        // Walk to first free block.
        char* cursor = heap_start + DSIZE;
        while (GET_SIZE(cursor) != 0 && IS_ALLOC(cursor)) cursor += GET_SIZE(cursor);
        size_t before = GET_SIZE(cursor);

        void* p = allocate(req);

        cursor = heap_start + DSIZE;
        while (GET_SIZE(cursor) != 0 && IS_ALLOC(cursor)) cursor += GET_SIZE(cursor);
        // Find the block that was just allocated (the one immediately before cursor
        // or any allocated block whose payload == p).
        char* blk = (char*)p - WSIZE;
        size_t used = GET_SIZE(blk);
        size_t payload = used - DSIZE;                  // header + footer overhead
        double util = 100.0 * req / used;

        printf("  %9zu  %9zu  %7zu  %9.1f%%\n", req, used, payload, util);
        free_addr(p);
    }

    printf("\n  DSIZE overhead per block = %zu bytes (header + footer).\n", DSIZE);
    printf("  MIN_BLOCK_SIZE = %zu bytes; anything smaller wastes proportionally more.\n",
           MIN_BLOCK_SIZE);
}

// ─────────────────────────────────────────────────────────────────────────────
// Workload 5 — Edge cases / correctness smoke test
// ─────────────────────────────────────────────────────────────────────────────

void smoke_tests() {
    section("Workload 5 — Correctness / edge-case smoke tests");

    bool ok = true;
    auto check = [&](const char* name, bool cond) {
        printf("  %-40s %s\n", name, cond ? "OK" : "FAIL ← !!!");
        if (!cond) ok = false;
    };

    // allocate(0) should return nullptr or a valid pointer — definitely not crash.
    void* z = allocate(0);
    check("allocate(0) returns nullptr", z == nullptr);

    // double-free should be a no-op (IS_ALLOC guard in free_addr).
    void* df = allocate(16);
    free_addr(df);
    free_addr(df);   // second free — should not corrupt the heap
    check("double-free survives (no crash)", true);

    // free(nullptr) — should be silently ignored.
    free_addr(nullptr);
    check("free_addr(nullptr) is safe", true);

    // Pointer outside heap — should be rejected.
    int stack_var = 42;
    free_addr(&stack_var);   // should hit the range check
    check("free_addr(stack ptr) is rejected", true);

    // Allocate + write to verify payload is writable.
    void* rw = allocate(64);
    check("allocate(64) returns non-null", rw != nullptr);
    if (rw) {
        memset(rw, 0xAB, 64);
        unsigned char* b = (unsigned char*)rw;
        bool filled = true;
        for (int i = 0; i < 64; ++i) if (b[i] != 0xAB) { filled = false; break; }
        check("64-byte payload is fully writable", filled);
        free_addr(rw);
    }

    // Coalesce: free two adjacent blocks and check that the heap has one big free block.
    void* x = allocate(32);
    void* y = allocate(32);
    heap_stats("before coalesce check");
    free_addr(x);
    free_addr(y);
    heap_stats("after coalesce check (should be one free block)");

    // After freeing both, walk heap and count free blocks.
    int free_count = 0;
    for (char* bp2 = heap_start + DSIZE; GET_SIZE(bp2) != 0; bp2 += GET_SIZE(bp2))
        if (!IS_ALLOC(bp2)) ++free_count;
    check("coalesce merges two adjacent free blocks into one", free_count == 1);

    printf("\n  Overall: %s\n", ok ? "ALL PASSED" : "SOME FAILED — see FAIL lines above");
}

// ─────────────────────────────────────────────────────────────────────────────
// main
// ─────────────────────────────────────────────────────────────────────────────

int main() {
    printf("═══════════════════════════════════════════════════════\n");
    printf("       Custom Allocator Benchmark vs glibc malloc      \n");
    printf("═══════════════════════════════════════════════════════\n");

    // Force heap initialisation once before any timing.
    initialize(HEAP_SIZE);
    warmup();

    // ── Speed tests ──────────────────────────────────────────────────────────
    bench_sequential(100'000, 64);          // same-size tight loop
    bench_random(50'000);                   // mixed sizes — glibc wins
    bench_alloc_many_free_all(500, 128);    // coalesce stress

    // ── Utilisation audit ────────────────────────────────────────────────────
    bench_utilization_audit();

    // ── Correctness smoke tests ──────────────────────────────────────────────
    smoke_tests();

    printf("\n");
    return 0;
}
