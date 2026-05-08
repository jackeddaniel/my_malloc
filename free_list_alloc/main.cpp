#include "allocator.h"
#include <iostream>

using namespace std;

int main() {
    initialize(4096);
    iter_heap();

    allocate(4);
    iter_heap();
}



