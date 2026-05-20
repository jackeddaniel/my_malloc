#include "allocator.h"

size_t round_to_pow2_ceiling(size_t n) {
    if(n == 0) {
        return 1;
    }

    size_t power = 1;
    
    while(power < n) {
        power *= 2;
    }

    return power;
}

void initialize_free_list(char* (&free_list)[16]) {
    cout<<"*********************"<<endl;
    cout<<"INITIALIZING FREE LIST"<<endl;
    for(int i = 0; i < 16; i++) {
        cout<<"we're at: "<<i<<endl;
        free_list[i] = nullptr;
    }
}

void insert(int index, char* free_block_h) {
    //*(char**)(free_block_h + WSIZE) = head;
    char* head = free_list[index];
    NEXT_FREE_BLK(free_block_h) = head;
    
    //*(char**)(free_block_h + DSIZE) = nullptr;
    PREV_FREE_BLK(free_block_h) = nullptr;

    if(head != nullptr) {
        PREV_FREE_BLK(head) = free_block_h;
    }

    head = free_block_h;
    free_list[index] = free_block_h;
    return;
}

void insert_free_block(char* free_block_h) {
    size_t free_block_size = GET_SIZE(free_block_h);
    
    int index = 0;
    size_t sz = free_block_size << 5;

    while(sz > 1 && index < 15) {
        sz >>= 1;
        index++;
    }

    //insert function
    insert(index, free_block_h);
}

