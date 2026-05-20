#include "allocator.h"

void iter_heap() {
    char* bp = heap_start + DSIZE;

    for(bp; GET_SIZE(bp) != 0; bp += GET_SIZE(bp)) {

        cout<<"This the first byte(char): "<<(void*)bp<<endl;
        cout<<"This is the size it stores: "<<GET_SIZE(bp)<<endl;
        if(IS_ALLOC(bp)) {
            cout<<"This block is allocated"<<endl;
        } else {
            cout<<"This block is free"<<endl;
        }
    }
}

void iter_free_list() {
    for(int i = 0; i < 16; i++) {
        cout<<"we're at: "<<i<<endl;
        char* iter = free_list[i];

        if(iter == nullptr) {
            cout<<"Empty list "<<(void*)iter<<endl;
            continue;
        }

        while(iter != nullptr) {
            cout<<"    "<<(void*)iter<<endl;

            iter = NEXT_FREE_BLK(iter);
        }

    }
}

