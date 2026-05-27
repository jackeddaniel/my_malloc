brk and sbrk increase the heap size.

for the bump allocator, which currently works, the following are the key issues to fix:
* We need a reset() function which clears/resets the memory
* We need to focus on alignment, which is basically rounding requested memory to the nearest 8 or 16 bytes instead of allowing bullshit like requesting 3 bytes, 9 bytes etc.



UPDATE:
* we have allocation, free list and pointer finding a free pointer from the free list going on. 
* the next goal is to implement splitting

Phase 2 — Serious allocator

Build:

segregated free lists
best-fit / first-fit comparisons
fragmentation metrics
alignment guarantees
thread safety (mutex)
calloc
realloc
memory poisoning
double-free detection
canaries
corruption detection
Phase 3 — Resume-worthy

This is where recruiters care.

Do:

benchmark against glibc malloc
multithread stress tests
perf profiling
fragmentation visualization
LD_PRELOAD override
custom stats dashboard
lock contention analysis


