#include <iostream>
#include <unistd.h>

using namespace std;

int MAX_SIZE = 16384;

void* bump_alloc(void* heap_start, void* heap_end, void* heap_curr, int size) {
    if (heap_curr < heap_start) {
        cout<<"Wrong current pointer. Smaller than the start, pls correct"<<endl;
        return nullptr;
    }
    if (heap_curr >= heap_end) {
        cout<<"You've exhausted you're memory. Sorry"<<endl;
        return nullptr;
    }


}


int main() {
    void* heap_start = sbrk(0);
    cout<<"Heap starts at :"<<heap_start<<endl;

    sbrk(MAX_SIZE);
    cout<<"Heap size is :"<<sbrk(0)<<endl;
}
