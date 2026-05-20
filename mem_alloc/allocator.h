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

constexpr size_t HEAP_SIZE = 1 << 20;
constexpr size_t WSIZE = sizeof(size_t);
constexpr size_t DSIZE = 2 * WSIZE;
constexpr size_t ALIGNMENT = 16;
constexpr size_t MIN_BLOCK_SIZE = 32;

//globals
extern char* heap_start;
extern char* heap_end;
extern char* free_list[];

//macros
#define ALIGN(size) ((size + ALIGNMENT-1) & ~(ALIGNMENT -1))
#define IS_ALLOC(p) (GET(p) & 1)
#define GET(p) (((header*)p)->size)
#define GET_SIZE(p) (GET(p) & ~(ALIGNMENT-1))
#define PACK(size, alloc) (size | alloc)

//free list macros
#define NEXT_FREE_BLK(p) *(char**)(p + WSIZE)
#define PREV_FREE_BLK(p) *(char**)(p + DSIZE)


//allocator.cpp
void initialize(size_t bytes);
void* allocate(size_t bytes);
void free_addr(void* pt);

//free_list_helpers.cpp
void initialize_free_list(char* (&free_list)[16]);
void insert(int index, char* free_block_h);
void insert_free_block(char* free_block_h);
void remove_free_block(char* free_block_h);

//debug.cpp
void iter_heap();
void iter_free_list();

