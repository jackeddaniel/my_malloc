#include "allocator.h"

int main() {
    initialize(4096);

    void* a = alloc(4);
    void* b = alloc(8);
    void* c = alloc(128);
    void* d = alloc(128);

    cout << "iteration after allocations" << endl;
    iter_header_list();

    free_addr(c);
    cout << "iteration after freeing" << endl;
    iter_header_list();

    alloc(4);
    cout << "iteration after alloc again" << endl;
    iter_header_list();
}
