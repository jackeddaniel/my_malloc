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
}

void* allocate(size_t bytes) {
    if(heap_start == nullptr) {
        initialize(HEAP_SIZE);
    }

    if(bytes == 0) {
        return nullptr;
    }


    size_t aligned_size = ALIGN(bytes);

    char* bp = heap_start + DSIZE;

    for(bp; GET_SIZE(bp) != 0; bp += GET_SIZE(bp)) {

        if(!(IS_ALLOC(bp)) && (aligned_size <= (GET_SIZE(bp) - DSIZE))) {
            
            char* payload = bp + WSIZE;
            remove_free_block(bp);

            size_t new_blk_size = aligned_size + DSIZE;
            size_t splt_blk_size = GET_SIZE(bp) - new_blk_size;

            if(splt_blk_size >= MIN_BLOCK_SIZE) {
                char* old_footer = (bp + ((header*)bp)->size) - WSIZE;
                char* splt_header = bp + new_blk_size;

                char* new_footer = (bp + new_blk_size) - WSIZE;
                ((header*)bp)->size = new_blk_size | 1;
                ((footer*)new_footer)->size = new_blk_size | 1;

                ((header*)splt_header)->size = splt_blk_size;
                ((footer*)old_footer)->size = splt_blk_size;
            }
            ((header*)bp)->size = ((header*)bp)->size | 1;
            ((footer*)(bp + GET_SIZE(bp) - WSIZE))->size = GET_SIZE(bp) | 1;
            

            return (void*)payload;
        }
    }

    return nullptr;
}

void merge(char* first, char* second) {
    if(!(IS_ALLOC(first)) && !(IS_ALLOC(second))) {
        size_t new_size = ((header*)first)->size + ((header*)second)->size;
        ((header*)first)->size = new_size;
        char* second_footer = second + ((header*)second)->size - WSIZE;
        ((footer*)second_footer)->size = new_size;
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
    coalesce(h);

    insert_free_block(h);
}



