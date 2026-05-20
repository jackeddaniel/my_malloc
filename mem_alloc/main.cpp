#include "allocator.h"
#include <iostream>

using namespace std;

int main() {
    cout<<"Iterating through the free list now"<<endl;
    allocate(32);
    iter_heap();
    iter_free_list();

}



