# Dynamic Memory Allocator 

## This is a dynamic memory allocator which I built/still working on to increase my understanding of how allocators work

This is a personal project where to test out memory allocation I've built three kinds of allocators, increasing in complexity. 
There are three branches in this repo for each of them.

### Branch: initial_test
This contains a bump allocator and a basic implicit list allocator where I have heavily used structs and address pointers. This was just to gain understanding of how blocks move in a high level. Real allocators heavily use pointer arithmetic to find and traverse blocks, instead of actual pointers to the block which requires extra memory.

### Branch: implicit_free_blocks
Here I implemented an implicit free list allocator where we traverse the heap purely through pointer arithmetic(like how actual allocators work).

*Features:*
* Acquiring a huge heap block from the sbrk() linux syscall and managing it for dynamic allocations.
* Inserting prologue and epilogue blocks as the boundaries for the heap.
* Each block contains header and footer metadata and is 16 byte aligned.
* It traverses all the blocks in the heap to find the first fit free memory block on to allocate when the user calls allocate.
* On free we just flip a bit on the header and footer tags.
* Implemented splitting on allocation and coalescing on freeing to reduce internal and external fragmentation respectively.
* Benchmarked against glibc.

### Branch: main
This branch contains an complete allocater with binning + segregated free lists to handle free blocks.

*Features:*
* We acquire a huge heap block from the sbrk() linux syscall and manage it.
* Contains prologue and epilogue block as the boundaries of the heap.
* Each allocated block contains header and footer tags of 8 bytes each, so a total of 16 bytes of metadata per block.
* Each free block contains a header, a pointer to the next free block and a pointer to the previous free block, 24 bytes of metadata per block.
* We use binning i.e, have an array of bins from which are exponential(contain all blocks from the current power of 2 to the next power of 2).
* Each bin contains the head node to a doubly linked list of all the free blocks whose size lie in that bin range. This prevents us from traversing the whole heap while allocating memory.
* We still split on allocation and coalesce on freeing, and modify the free list accordingly. The metadata inside the free blocks itself guides the traversal so we aren't storing any extra information on the heap. Only the array of bins, which is stored in the stack.


# I will stil be working on this as I learn more about allocators and memory management.
