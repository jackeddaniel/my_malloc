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

/*
//globals
extern void* heap_start;
extern void* heap_end;
extern void* heap_curr;
extern header* header_start;
extern header* last_header;

//allocator.cpp
void initialize(size_t bytes);
void reset();
void* alloc(size_t bytes);
void free_addr(void* addr);

//split.cpp
void* find_free_space(size_t bytes);
void split(header* h, size_t bytes);

//coalesce.cpp
void coalesce(header* h);

//debug.cpp
void iter_header_list();
*/



