#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
    cout<<"lets see where our heap starts"<<endl;
    void* heap_start = sbrk(0);

    cout<<heap_start<<endl;

    sbrk(100);
    cout<<"new heap address"<<endl;
    void* heap_end = sbrk(0);
    cout<<heap_end<<endl;
}
