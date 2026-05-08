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
}

void reset() {
    heap_start = nullptr;
    heap_end = nullptr;
}

void* allocate(size_t bytes) {
    size_t aligned_size = ALIGN(bytes);
    size_t block_size = sizeof(header) + sizeof(footer) + aligned_size;

    char* bp = heap_start + DSIZE;

    for(bp; GET_SIZE(bp) != 0; bp += GET_SIZE(bp)) {

        if((GET(bp) % ALIGNMENT) == 0 && (aligned_size <= (GET_SIZE(bp) - DSIZE))) {
            
            char* payload = bp + WSIZE;

            size_t new_blk_size = aligned_size + DSIZE;
            size_t splt_blk_size = ((header*)bp)->size - new_blk_size;

            if(splt_blk_size >= MIN_BLOCK_SIZE) {
                char* old_footer = (bp + ((header*)bp)->size) -sizeof(footer);
                char* splt_header = bp + new_blk_size;

                char* new_footer = (bp + new_blk_size) - sizeof(footer);
                ((header*)bp)->size = new_blk_size | 1;
                ((footer*)new_footer)->size = new_blk_size | 1;

                ((header*)splt_header)->size = splt_blk_size;
                ((footer*)old_footer)->size = splt_blk_size;
            }
            ((header*)bp)->size = ((header*)bp)->size | 1;

            return (void*)payload;
        }
    }

    return nullptr;
}

/*
int main() {
    initialize(4096);
    
    char* tmp = heap_start;
    cout<<"Prologue: "<<((header*)tmp)->size<<endl;
    tmp = tmp + 16;
    cout<<"size of the first massive block: "<<((header*)tmp)->size<<endl;

    char* new_tmp = heap_end;
    new_tmp = new_tmp - 16;
    cout<<"size of epilogue: "<<((footer*)new_tmp)->size<<endl;

    size_t test = GET_SIZE(tmp);
    cout<<"GET_SIZE test: "<<test<<endl;
}

*/
