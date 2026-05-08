#include "allocator.h"
#include <iostream>

using namespace std;

int main() {
    initialize(4096);
    iter_heap();

//    cout<<allocate(4032)<<endl;
    cout<<allocate(4)<<endl;
    cout<<allocate(4)<<endl;
    cout<<allocate(3984)<<endl;


    iter_heap();

}



