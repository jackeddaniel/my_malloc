#include <iostream>
#include <unistd.h>

using namespace std;

struct header {
    size_t size;
};

struct footer {
    size_t footer;
};

int main() {
    int ALIGNMENT = 16;
    int bytes = 9;
    size_t aligned_size = (bytes + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
    
    size_t block_size = sizeof(header) + sizeof(footer) + aligned_size;

    cout<<block_size;
}
