#include "allocator.h"

void iter_header_list() {
    cout << endl;
    cout << "Iterating through the header list" << endl;
    header* iter = header_start;
    if (iter == nullptr) {
        cout << "empty list" << endl;
    }
    while (iter != nullptr) {
        cout << "Header addr: " << iter << endl;
        cout << "Size of the data: " << iter->size << endl;
        cout << "Is this block free: " << iter->free << endl;
        cout << "Next header location: " << iter->next << endl;
        iter = iter->next;
    }
    cout << "Finished iterating through the header list" << endl;
}
