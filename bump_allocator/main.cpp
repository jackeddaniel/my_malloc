// what are the funcs i need
// i need a initializer func which will initialize the memory
// i need a function which return memory address with the requested bytes

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
    heap_curr = (char*)heap_curr + bytes;
    return return_addr;
}
int main() {
    initialize(4096);
    cout<<"current location of heap begin: "<<heap_start<<endl;
    cout<<"current location of heap end: "<<heap_end<<endl;
    cout<<"current heap curr: "<<heap_curr<<endl;
    void* test = beg(4);
    *(reinterpret_cast<int*>(test)) = 4;
    cout<<"current heap curr after memory allocation: "<<heap_curr<<endl;
    cout<<*(reinterpret_cast<int*>(test))<<endl;
}

