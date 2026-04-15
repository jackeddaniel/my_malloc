brk and sbrk increase the heap size.

for the bump allocator, which currently works, the following are the key issues to fix:
* We need a reset() function which clears/resets the memory
* We need to focus on alignment, which is basically rounding requested memory to the nearest 8 or 16 bytes instead of allowing bullshit like requesting 3 bytes, 9 bytes etc.



