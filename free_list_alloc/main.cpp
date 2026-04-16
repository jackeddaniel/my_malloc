#include <iostream>
#include <unistd.h>
#include <cstdint>

using namespace std;

void* heap_start;
void* heap_end;
void* heap_curr;


struct header {
    header* next; //8 bytes
    size_t size; //8 bytes
    uint32_t free; //4 bytes
};
void initialize(size_t bytes) {
    //assigning the start of the heap
    heap_start = sbrk(0);
    heap_curr = heap_start;

    //allocating the required memory
    sbrk(bytes);

    heap_end = sbrk(0);
}
void* alloc(size_t bytes) {
    size_t align_val = 16;

    header h;

    h.size = bytes;

    size_t h_size = sizeof(h);
    
    uintptr_t aligned = (uintptr_t(heap_curr) + align_val - 1) & ~(align_val - 1);
    size_t size = (char*)heap_end - (char*)aligned;

    if(h_size + bytes > size) {
        cout<<"not enough memory"<<endl;
        return nullptr;
    }



    void* return_addr = (void*)(aligned + h_size);
    h.next = (header*)(aligned + h_size + bytes);


    void* aligned_heap_curr = return_addr;
    heap_curr = (char*)aligned_heap_curr + bytes + h_size;
    h.next = (header*)heap_curr;
    return return_addr;
    
    /*
    cout<<"addr before alignment: "<<heap_curr<<endl;
    uintptr_t aligned = (uintptr_t(heap_curr) + align_val - 1) & ~(align_val - 1);    
    size_t size = (char*)heap_end - (char*)aligned;

    if(bytes > size) {
        cout<<"not enough memory"<<endl;
        return (void*)0;
    }
    void* return_addr = (void*) aligned;

    void* aligned_heap_curr = (void*) aligned;
    cout<<"addr after alignment: "<<aligned_heap_curr<<endl;
    heap_curr = (char*)aligned_heap_curr + bytes;
    return return_addr;
    */
}

void reset() {
    heap_curr = heap_start;
}

int main() {
    initialize(4096);
    alloc(3);
    alloc(5);
    alloc(4);
    alloc(43);
    alloc(5);

}

