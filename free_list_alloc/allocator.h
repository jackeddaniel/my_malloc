#pragma once //tells the compiler to include this only once
#include<iostream>
#include<unistd.h>
#include<cstdint>

using namespace std;

struct header {
    header* next; //8 bytes
    size_t size; //8 bytes
    uint32_t free; //4 bytes
    int padding;
    char align[8];
};

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

//debug.cpp
void iter_header_list();



