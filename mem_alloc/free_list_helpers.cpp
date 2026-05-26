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

int compute_bin_index(size_t sz) {
    int index = 0;
    sz = sz >> 5;

    while(sz > 1 && index < 15) {
        sz >>= 1;
        index++;
    }
    return index;
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

    free_list[index] = free_block_h;
    return;
}

void insert_free_block(char* free_block_h) {
    cout<<"INSERTING FREE BLOCK"<<endl;
    size_t free_block_size = GET_SIZE(free_block_h);
    cout<<"Size of the free block: "<<free_block_size<<endl;
    
    int index = compute_bin_index(free_block_size);
    insert(index, free_block_h);
}

void remove_free_block(char* free_block_h) {
    cout<<"REMOVING FREE BLOCK"<<endl;
    char* prev_blk = PREV_FREE_BLK(free_block_h);
    char* next_blk = NEXT_FREE_BLK(free_block_h);

    int index = compute_bin_index(GET_SIZE(free_block_h));
    cout<<"The index of the block in the free list is: "<<index<<endl;
    size_t sz = GET_SIZE(free_block_h);
    cout<<"The size of the free block is: "<<sz<<endl;

    // only one free block
    if(prev_blk == nullptr && prev_blk == next_blk) {
        free_list[index] = nullptr;
    } else if(prev_blk == nullptr) {
        PREV_FREE_BLK(next_blk) = nullptr;
        free_list[index] = next_blk;

    } else if(next_blk == nullptr) {
        NEXT_FREE_BLK(prev_blk) = nullptr;
    } else {
        NEXT_FREE_BLK(prev_blk) = next_blk;
        PREV_FREE_BLK(next_blk) = prev_blk;
    }
}

char* iter_list(char* head, size_t bytes) {
    char* iter = head;
    while(iter != nullptr) {
        if(GET_SIZE(iter) >= bytes) return iter;
        iter = NEXT_FREE_BLK(iter);
    }
    return iter;
}

char* find_free_block(size_t bytes) {
    int bin_index = compute_bin_index(bytes);
    if(bin_index >= 16) return nullptr;
    while(bin_index < 16 && free_list[bin_index] == nullptr) bin_index++;

    while(bin_index < 16) {
        char* free_blk = iter_list(free_list[bin_index], bytes);
        if(free_blk != nullptr) return free_blk;
        bin_index++;
    }
    return nullptr;
}
