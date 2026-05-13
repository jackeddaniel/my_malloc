Next Steps in Order
Phase 1 — Fix What's Broken (do this first)

Backward coalescing: right now two freed neighbors only merge if the left one is freed second. Free them in the other order and you get fragmentation. Fix this with a footer/boundary tag on each block
Your split() has an alignment bug — it computes the split header address with alignment but then h->size is calculated from the pre-alignment pointer, so the sizes can be slightly off under certain inputs
Test with a stress loop: alloc/free thousands of times in random order and check that the heap never corrupts

Phase 2 — Make it Actually Fast

Segregated free lists: instead of one linked list, maintain ~8 lists bucketed by size class. Allocation becomes "find the right bucket, pop the head" — near O(1)
Switch from sbrk() to mmap() — this lets you return memory to the OS when a region is fully free, which sbrk() cannot do
Add a small-object optimization: for allocations under ~256 bytes (the vast majority in real programs), use a slab-style pool

Phase 3 — Harden it

Thread safety with per-thread caches (like TCMalloc's design — each thread has a local cache, only hits a global lock on overflow)
Canary values in headers to detect corruption
Handle edge cases: alloc(0), double-free, out-of-bounds free

Phase 4 — Measure Everything

Write a benchmark suite: throughput (allocs/sec), fragmentation ratio, latency distribution
Compare against malloc, jemalloc, and mimalloc on the same workloads
Understand why you're slower or faster in each case — this is where the real learning is


When to Put it on Your Resume
Right now — one honest bullet:

"Implemented a custom heap allocator in C++ with first-fit allocation, block splitting, and forward/backward coalescing"

That's truthful and already shows more systems knowledge than most candidates.
After Phase 1+2 — a stronger bullet:

"Built a heap allocator with segregated free lists, boundary-tag coalescing, and mmap-backed arenas"

At this point it's a genuine talking point in any systems interview.
After Phase 4 — a project, not just a bullet:

"Custom allocator benchmarked against jemalloc — 2x faster on small short-lived allocations, with writeup on design tradeoffs"

Now you can link a GitHub repo with a README and graphs, and it holds up to scrutiny from a senior engineer.

When it Competes with Industry Standard
Honestly — it won't, and that's fine, but here's the gap:
jemalloc and mimalloc have 15+ years of tuning, NUMA awareness, huge page support, lock-free paths, and are tested against every allocation pattern that exists in production software. You're not beating them with a semester project.
What "competes" actually means for you: if you can make your allocator faster than glibc malloc on a specific, well-defined workload — say, a workload heavy on small same-size allocations — and you can explain exactly why, that is legitimately impressive. Not "I beat jemalloc" but "I outperformed the general-purpose allocator on this narrow case because of this specific design decision."
That's the framing that makes a senior engineer lean forward in an interview. The goal isn't to win — it's to show you understand the tradeoffs deeply enough to win somewhere.

