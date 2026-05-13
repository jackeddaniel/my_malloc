PHASE 1 — Explicit Free List
│  Store prev/next pointers in free block payloads
│  LIFO policy (insert at front) first, then try address-ordered
│  Re-read CS:APP 9.9.13
│
PHASE 2 — Segregated Free Lists
│  Pick size classes (powers of 2 works, jemalloc-style is better)
│  First-fit within a class = effectively best-fit globally
│  This is the core of every serious allocator
│
PHASE 3 — Replace sbrk with mmap
│  sbrk is deprecated and not thread-safe
│  Large allocs (>128KB) go direct to mmap, freed back to OS immediately
│  Read: mmap(2) man page, then how glibc decides the threshold (M_MMAP_THRESHOLD)
│
PHASE 4 — realloc + calloc
│  realloc: try to extend in-place before copying
│  calloc: mmap'd memory is already zeroed — exploit this
│
PHASE 5 — Benchmarking
│  Write a trace-driven harness (random alloc/free workloads)
│  Measure: throughput (ops/sec) and utilization (bytes requested / bytes mapped)
│  Compare against system malloc, tcmalloc
│  Look up the "fragmentation ratio" metric specifically
│
PHASE 6 — Thread Safety
   Coarse: single mutex (easy, educational)
   Fine: per-arena locks (what jemalloc does)
   Read the jemalloc paper here — it explains why a global lock kills perf
