#include <iostream>
#include <unistd.h>
#include <cstdint>

using namespace std;


struct header {
    header* next; //8 bytes
    size_t size; //8 bytes
    uint32_t free; //4 bytes
    int padding;
    char align[8];
};

void* heap_start;
void* heap_end;
void* heap_curr;
header* header_start;
header* last_header;


void initialize(size_t bytes) {
    //assigning the start of the heap
    heap_start = sbrk(0);
    heap_curr = heap_start;
    header_start = nullptr;
    last_header = nullptr;

    //allocating the required memory
    sbrk(bytes);

    heap_end = sbrk(0);
}
void* alloc(size_t bytes) {
    cout<<"heap_curr before alloc: "<<heap_curr<<endl;
    // choosing the alignment value
    size_t align_val = 16;
    header* h;
    
    h = (header*)heap_curr;
    h->next = nullptr;

    if(header_start == nullptr) {
        header_start = h;
        cout<<"header_start assigned"<<endl;
    }

    if(last_header == nullptr) {
        last_header = h;
        cout<<"last header assigned"<<endl;
    }
    

    size_t h_size = sizeof(header); //this should always be 32, if our header padding logic is correct
    cout<<"size of header: "<<h_size<<endl;
    
    uintptr_t aligned_addr = (uintptr_t(heap_curr) + align_val - 1) & ~(align_val - 1); //this gives us the next aligned address where we can put our header and memory
    cout<<"aligned_addr: "<<(void*)aligned_addr<<endl;

    size_t size = (char*)heap_end - (char*)aligned_addr;


    if(h_size + bytes > size) {
        cout<<"not enough memory"<<endl;
        return nullptr;
    }

    h->size = bytes;
    h->free = 0;
    last_header->next = h;
    last_header = h;
    cout<<"size stored in header: "<<h->size<<endl;

    void* payload = (void*)((char*)aligned_addr + h_size);
    cout<<"payload addr: "<<payload<<endl;
    void* payload_end = (void*)(aligned_addr + h_size + bytes);
    void* block_end = (void*)((uintptr_t(payload_end) + align_val - 1) & ~(align_val - 1));


    heap_curr = block_end;
    cout<<"heap_curr after alloc: "<<heap_curr<<endl;

    return payload;
}
void iter_header_list() {
    cout<<endl;
    cout<<"Iterating through the header list"<<endl;
    header* iter = header_start;
    if(iter == nullptr) {
        cout<<"empty list"<<endl;
    }

    while(iter != nullptr) {
        cout<<"Header addr: "<<iter<<endl;
        cout<<"Size of the data: "<<iter->size<<endl;
        cout<<"Is this block free: "<<iter->free<<endl;
        cout<<"Next header location: "<<iter->next<<endl;
        iter = iter->next;
    }
    cout<<"Finished iterating throught the header list"<<endl;
}
void reset() {
    heap_curr = heap_start;
}

int main() {
    initialize(4096);
    void* test = alloc(4);
    alloc(5);
    alloc(16);
    alloc(32);
    cout<<"----- main ------"<<endl;
    cout<<"header_start is : "<<header_start<<endl;
    iter_header_list();
    cout<<"returned addr: "<<test<<endl;

    header* test_head = (header*)((char*)test - 32);
    cout<<"header pointer must be: "<<test_head<<endl;
    cout<<"size stored in the header: "<<test_head->size<<endl;
    cout<<heap_curr<<endl;
}

