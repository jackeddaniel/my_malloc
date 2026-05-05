#include "allocator.h"

void* find_free_space(size_t bytes) {
    header* iter = header_start;
    while (iter != nullptr) {
        if (iter->free && iter->size >= bytes) {
            return (header*) iter;
        }
        iter = iter->next;
    }
    return nullptr;
}

void split(header* h, size_t bytes) {
    size_t h_size = sizeof(header);
    size_t min_alloc_size = h_size + 4;

    header* next_h = h->next;
    if (next_h == nullptr) {
        next_h = (header*)((char*)h + h_size + h->size);
    }

    size_t space = (char*)next_h - (char*)h;
    size_t free_space = space - h_size;

    if (free_space < bytes + min_alloc_size) {
        return;
    }

    header* tmp_h = (header*)((char*)h + h_size + bytes);
    uintptr_t aligned_addr = ((uintptr_t)tmp_h + 15) & ~15;
    header* split_h = (header*) aligned_addr;

    split_h->next = h->next;
    split_h->free = 1;
    split_h->size = ((char*)next_h - (char*)split_h) - h_size;
    h->size = ((char*)split_h - (char*)h) - h_size;
    h->next = split_h;
}
