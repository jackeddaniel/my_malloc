#include "allocator.h"


void iter_heap() {
    char* bp = heap_start + DSIZE;

    for(bp; GET_SIZE(bp) != 0; bp = bp + GET_SIZE(bp)) {
        cout<<"This the first byte(char): "<<(void*)bp<<endl;
        cout<<"This is the size it stores: "<<GET_SIZE(bp)<<endl;
    }
}

