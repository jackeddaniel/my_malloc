#include "allocator.h"

char* heap_start = nullptr;
char* heap_end = nullptr;
char* free_list[16];


void initialize(size_t bytes) {
    heap_start = (char*)sbrk(0);
 
    if(sbrk(bytes) == (void*)-1) return;
 
    heap_end = (char*)sbrk(0);

    //make a prologue block
    char* tmp = heap_start;
    ((header*)tmp)->size = 16 | 1;
    tmp = tmp + sizeof(header);
    ((footer*)tmp)->size = 16 | 1;

    //make an epilogoue block
    char* end = heap_end - sizeof(footer);
    //lets bring the epilogue 8 bytes down
    end = end - 8;
    ((header*)end)->size = 0 | 1;

    //make a big free block
    size_t free_size = (bytes - (DSIZE + WSIZE)) & ~(ALIGNMENT-1);
    char* new_tmp = heap_start;
    new_tmp = new_tmp + DSIZE;
    ((header*)new_tmp)->size = free_size;
    new_tmp = new_tmp + (free_size - sizeof(footer));
    ((footer*)new_tmp)->size = free_size;
    initialize_free_list(free_list);
    insert_free_block(heap_start + DSIZE);
}

void reset() {
    heap_start = nullptr;
    heap_end = nullptr;
    initialize_free_list(free_list);
}

void* allocate(size_t bytes) {
    if(heap_start == nullptr) {
        initialize(HEAP_SIZE);
    }

    if(bytes == 0) {
        return nullptr;
    }

    size_t aligned_size = ALIGN(bytes) + DSIZE;
    char* free_block = find_free_block(aligned_size);

    if(free_block == nullptr) return (void*)nullptr;

    remove_free_block(free_block);

    size_t free_blk_sz = GET_SIZE(free_block);
    size_t split_blk_sz = free_blk_sz - aligned_size;

    if(split_blk_sz >= MIN_BLOCK_SIZE) {
        //new headers
        char* old_h = free_block;
        char* splt_h = free_block + aligned_size;
        
        //new footers
        char* new_f = splt_h - WSIZE;
        char* splt_f = splt_h + split_blk_sz - WSIZE;

        ((header*)old_h)->size = aligned_size | 1;
        ((footer*)new_f)->size = aligned_size | 1;

        ((header*)splt_h)->size = split_blk_sz;
        ((footer*)splt_f)->size = split_blk_sz;

        insert_free_block(splt_h);
    } else {
        ((header*)free_block)->size = GET_SIZE(free_block) | 1;
        char* free_blk_footer = free_block + GET_SIZE(free_block) - WSIZE;
        ((footer*)free_blk_footer)->size = GET_SIZE(free_block) | 1;
    }

    char* payload = free_block + WSIZE;
    
    return (void*)payload;
}

void merge(char* first, char* second) {
    if(!(IS_ALLOC(first)) && !(IS_ALLOC(second))) {
        remove_free_block(first);
        remove_free_block(second);
        size_t new_size = ((header*)first)->size + ((header*)second)->size;
        ((header*)first)->size = new_size;
        char* second_footer = second + ((header*)second)->size - WSIZE;
        ((footer*)second_footer)->size = new_size;
        insert_free_block(first);
    }
}

void coalesce(char* pt) {
    if(pt == nullptr) return;

    char* prev_pt_header = pt - GET_SIZE((pt - WSIZE));
    char* next_pt_header = pt + GET_SIZE(pt);

    merge(pt, next_pt_header);
    merge(prev_pt_header, pt);
}

void free_addr(void* pt) {
    if(pt == nullptr) return;
    if(pt < heap_start || pt > heap_end) return;


    char* h = (char*)pt - WSIZE;
    if(!IS_ALLOC(h)) return;
    
    //clearing the alloc bit
    size_t size = GET_SIZE(h);
    ((header*)h)->size = size;

    ((footer*)(h + size - WSIZE))->size = size;
    insert_free_block(h);
    coalesce(h);
}


