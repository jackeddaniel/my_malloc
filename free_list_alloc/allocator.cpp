#include "allocator.h"

void* heap_start;
void* heap_end;
void* heap_curr;
header* header_start;
header* last_header;

void initialize(size_t bytes) {
    heap_start = sbrk(0);
    heap_curr = heap_start;
    header_start = nullptr;
    last_header = nullptr;
 
    sbrk(bytes);
 
    heap_end = sbrk(0);
}
 
void reset() {
    heap_curr = heap_start;
    header_start = nullptr;
    last_header = nullptr;
}

void* alloc(size_t bytes) {
    void* free_block = find_free_space(bytes);
    if (free_block != nullptr) {
        header* tmp = (header*) free_block;
        split(tmp, bytes);
        tmp->free = 0;
        void* payload = (char*)tmp + sizeof(header);
        return payload;
    }
 
    size_t align_val = 16;
    uintptr_t aligned_addr = (uintptr_t(heap_curr) + align_val - 1) & ~(align_val - 1);
    header* h = (header*)aligned_addr;
 
    if (header_start == nullptr) {
        header_start = h;
    }
 
    if (last_header != nullptr && last_header != h) {
        last_header->next = h;
    }
 
    size_t h_size = sizeof(header);
    size_t size = (char*)heap_end - (char*)aligned_addr;
 
    if (h_size + bytes > size) {
        return nullptr;
    }
 
    void* payload = (void*)((char*)aligned_addr + h_size);
    void* payload_end = (void*)(aligned_addr + h_size + bytes);
    void* block_end = (void*)((uintptr_t(payload_end) + align_val - 1) & ~(align_val - 1));
 
    h->size = (char*)block_end - ((char*)aligned_addr + h_size);
    h->free = 0;
    h->next = nullptr;
    last_header = h;
 
    heap_curr = block_end;
 
    return payload;
}

void free_addr(void* addr) {
    header* to_be_freed = (header*)((char*)addr - sizeof(header));
 
    if (header_start == nullptr) {
        cout << "Header list is empty, nothing to free" << endl;
        return;
    }
 
    header* iter = header_start;
    while (iter != nullptr) {
        if (iter == to_be_freed) {
            iter->free = 1;
            //coalesce(iter);
            return;
        }
        iter = iter->next;
    }
 
    cout << "The address doesn't belong to the list. So nothing to be freed" << endl;
}
