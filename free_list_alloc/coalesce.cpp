#include "allocator.h"

void coalesce(header* h) {
    if(h == nullptr) return;
    if(h->next == nullptr) return;

    size_t h_size = sizeof(header);

    header* pred_next = (header*)((char*)h + h_size + h->size);
    
    if(h->next == pred_next && h->free == 1 && h->next->free == 1) {
        size_t new_size = h->size + h->next->size + h_size;
        h->next = h->next->next;
        h->size = new_size;
    }
} 

