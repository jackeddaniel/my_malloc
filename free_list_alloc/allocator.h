#pragma once //tells the compiler to include this only once
#include<iostream>
#include<unistd.h>
#include<cstdint>

using namespace std;

struct header {
    size_t size; //just 8 bytes of information
};

struct footer {
    size_t size; //just 8 bytes of information
};

constexpr size_t WSIZE = sizeof(size_t);
constexpr size_t DSIZE = 2 * WSIZE;
constexpr size_t ALIGNMENT = 16;
constexpr size_t MIN_BLOCK_SIZE = 32;

//globals
extern char* heap_start;
extern char* heap_end;

//macros
#define ALIGN(size) ((size + ALIGNMENT-1) & ~(ALIGNMENT -1))
#define GET(p) (((header*)p)->size)
#define GET_SIZE(p) (GET(p) & ~(ALIGNMENT-1))
#define PACK(size, alloc) (size | alloc)
#define HDRP(p) ()

//allocator.cpp
void initialize(size_t bytes);
void* allocate(size_t bytes);

//debug.cpp
void iter_heap();

