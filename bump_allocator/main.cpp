// what are the funcs i need
// i need a initializer func which will initialize the memory
// i need a function which return memory address with the requested bytes
//

#include <iostream>
#include <unistd.h>

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

void* beg(int bytes) {
    int size = (char*)heap_end - (char*)heap_curr;
    if(bytes > size) {
        cout<<"not enough memory"<<endl;
        return (void*)0;
    }

    if(heap_start == heap_end) {
        cout<<"memory limit reached"<<endl;
        return (void*)0;
    }

    void* return_addr = heap_curr;

    cout<<"Here, take you memory boi"<<endl;
    heap_curr = (char*)heap_curr + x;
    return return_addr;
}
int main() {


























































































}
