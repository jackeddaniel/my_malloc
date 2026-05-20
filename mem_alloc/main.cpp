#include "allocator.h"
#include <iostream>

using namespace std;

int main() {
    

//    void* allocate(4032)<<endl;
    void* a = allocate(4);
    void* b = allocate(4);
    void* c = allocate(8);
    void* d = allocate(32);
    void* e = allocate(16);

    iter_heap();

    cout<<"First Freeing"<<endl;
    cout<<"-----"<<endl;
    free_addr(b);
    iter_heap();
    cout<<""<<endl;
    
    cout<<"Second Freeing"<<endl;
    cout<<"-----"<<endl;
    free_addr(d);
    iter_heap();
    cout<<""<<endl;
    
    cout<<"Third Freeing"<<endl;
    free_addr(c);
    iter_heap();

    cout<<"Iterating through the free list now"<<endl;
    iter_free_list();

}



