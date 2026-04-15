#include <iostream>
#include <unistd.h>
#include <cstdint>

using namespace std;

void* heap_start;
void* heap_end;
void* heap_curr;

void initialize(int bytes) {
    //assigning the start of the heap
    heap_start = sbrk(0);
    heap_curr = heap_start;

    //allocating the required memory
    sbrk(bytes);

    heap_end = sbrk(0);
}

void reset() {
    heap_curr = heap_start;
}

void* alloc(int bytes) {
    int align_val = 16;

    int size = (char*)heap_end - (char*)heap_curr;
    
    if(bytes > size) {
        cout<<"not enough memory"<<endl;
        return (void*)0;
    }
    void* return_addr = heap_curr;
    cout<<"addr before alignment: "<<heap_curr<<endl;
    uintptr_t aligned = (uintptr_t(heap_curr) + align_val - 1) & ~(align_val - 1);    

    void* aligned_heap_curr = (void*) aligned;

    cout<<"addr after alignment: "<<aligned_heap_curr<<endl;
    heap_curr = (char*)aligned_heap_curr + bytes;
    return return_addr;
}
int main() {
    initialize(4096);
    cout<<"current location of heap allocin: "<<heap_start<<endl;
    cout<<"current location of heap end: "<<heap_end<<endl;
    cout<<"current heap curr: "<<heap_curr<<endl;
    alloc(3);
    cout<<"current heap curr: "<<heap_curr<<endl;
    alloc(3);
    cout<<"curren heap curr: "<<heap_curr<<endl;
    reset();
    cout<<"heap curr after reset: "<<heap_curr<<endl;

    int* trial = (int*)alloc(sizeof(int));
    *trial = 4;
    cout<<*trial<<endl;
    cout<<trial<<endl;
    cout<<heap_curr<<endl;

    char* char_trial = (char*)alloc(sizeof(char));
    *char_trial = 'F';
    cout<<*char_trial<<endl;
    cout<<char_trial<<endl;
    cout<<heap_curr<<endl;
    alloc(3);

}

