#include "allocator.h"

char* heap_start;
char* heap_end;

void initialize(size_t bytes) {
    heap_start = (char*)sbrk(0);
 
    sbrk(bytes);
 
    heap_end = (char*)sbrk(0);

    //make a prologue block
    char* tmp = heap_start;
    ((header*)tmp)->size = 16 | 1;
    tmp = tmp + sizeof(header);
    ((footer*)tmp)->size = 16 | 1;

    //make an epilogoue block
    char* end = heap_end - sizeof(footer);
    ((header*)end)->size = 0 | 1;

    //make a big free block
    size_t free_size = (bytes - (DSIZE + WSIZE)) & ~(ALIGNMENT-1);
    char* new_tmp = heap_start;
    new_tmp = new_tmp + DSIZE;
    ((header*)new_tmp)->size = free_size;
    new_tmp = new_tmp + (free_size - sizeof(footer));
    ((footer*)new_tmp)->size = free_size;
}

void reset() {
    heap_start = nullptr;
    heap_end = nullptr;
}

void* allocate(size_t bytes) {
    size_t aligned_size = ALIGN(bytes);
    size_t block_size = sizeof(header) + sizeof(footer) + aligned_size;

    char* bp = heap_start;
    return (void*)bp;
}

int main() {
    initialize(4096);
    
    char* tmp = heap_start;
    cout<<"Prologue: "<<((header*)tmp)->size<<endl;
    tmp = tmp + 16;
    cout<<"size of the first massive block: "<<((header*)tmp)->size<<endl;

    char* new_tmp = heap_end;
    new_tmp = new_tmp - 8;
    cout<<"size of epilogue: "<<((footer*)new_tmp)->size<<endl;

}


