#include "allocator.h"
#include <iostream>

using namespace std;

int main() {
    void* a = allocate(32);
    void* b = allocate(64);
     
    iter_free_list();
    free_addr(b);
    void* c = allocate(1028);
    iter_free_list();
}



