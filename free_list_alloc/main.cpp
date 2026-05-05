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


void reset() {
    heap_curr = heap_start;
    header_start = nullptr;
    last_header = nullptr;
}


void* find_free_space(size_t bytes) {
    cout<<endl;
    cout<<"Iterating through the header list"<<endl;
    header* iter = header_start;
    if(iter == nullptr) {
        cout<<"empty list"<<endl;
    }

    while(iter != nullptr) {
        if(iter->free && iter->size >= bytes) {
            return (header*) iter;
        }
       iter = iter->next;
    }
    cout<<"Finished iterating throught the header list"<<endl;
    return nullptr;
}

void split(header* h, size_t bytes) {
    size_t h_size = sizeof(header);
    size_t min_alloc_size = h_size + 4;

    header* next_h = h->next;
    cout<<"NEXT POINTER: "<<next_h<<endl;
    if(next_h == nullptr) { 
        header* end_of_block = (header*)((char*)h + h_size + h->size);
        next_h = (header*)end_of_block;
    }
    cout<<"NEXT POINTER AFTER FIXING: "<<next_h<<endl;
    
    //total space between two header pointers
    size_t space = (char*)next_h - (char*)h;
   
    //total free usable space
    size_t free_space = space - h_size;

    //check to see if the memory is worthwhile to split
    if(free_space < bytes + min_alloc_size) {
        cout<<"No need to split, memory not large enough"<<endl;
        return;
    }
   
    //temporary header address
    header* tmp_h = (header*)((char*)h + h_size + bytes);

    //aligned temporary header address
    //we now have the aligned address of the new pointer
    uintptr_t aligned_addr = ((uintptr_t)tmp_h + 15) & ~15; 

    header* split_h = (header*) aligned_addr;
    
    split_h->next = h->next;
    split_h->free = 1;
    split_h->size = ((char*)next_h - (char*)split_h) - h_size;
    h->size = ((char*)split_h - (char*)h) - h_size;

    h->next = split_h;

    return;
}



void* alloc(size_t bytes) {
    void* free_block = nullptr;

    free_block = find_free_space(bytes);
    if(free_block != nullptr) {
        header* tmp = (header*) free_block;
        split(tmp, bytes);
        tmp->free = 0;
        void* payload = (char*)tmp + 32;
        return payload;
    }

    // choosing the alignment value
    size_t align_val = 16;
    header* h;
    
    uintptr_t aligned_addr = (uintptr_t(heap_curr) + align_val - 1) & ~(align_val - 1); //this gives us the next aligned address where we can put our header and memory
    h = (header*)aligned_addr;


    if(header_start == nullptr) {
        header_start = h;
    }

    if(last_header != nullptr && last_header != h) {
        last_header->next = h;
    }

    size_t h_size = sizeof(header); //this should always be 32, if our header padding logic is correct
    
    size_t size = (char*)heap_end - (char*)aligned_addr;


    if(h_size + bytes > size) {
        return nullptr;
    }
    

    void* payload = (void*)((char*)aligned_addr + h_size);
    void* payload_end = (void*)(aligned_addr + h_size + bytes);
    void* block_end = (void*)((uintptr_t(payload_end) + align_val - 1) & ~(align_val - 1));

    h->size = (char*)block_end - ((char*)aligned_addr + h_size);
    h->free = 0;
    last_header = h;

    heap_curr = block_end;

    return payload;
}


void free_addr(void* addr) {
    cout<<"Addr to be freed: "<<addr<<endl;
    cout<<"Begin the freeing"<<endl;
    header* to_be_freed = (header*) ((char*)addr - sizeof(header));

    if(header_start == nullptr) {
        cout<<"Header list is empty, nothing to free"<<endl;
        return;
    }

    header* iter = header_start;
    while(iter != nullptr) {
        cout<<"I am here: "<<iter<<endl;
        if(iter == to_be_freed) {
            iter->free = 1;
            cout<<"Address freed"<<endl;
            return;
        }
        iter = iter->next;
    }
    cout<<"The address doesn't belong to the list. So nothing to be freed"<<endl;
    return;
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
int main() {
    initialize(4096);

    void* a = alloc(4);
    void* b = alloc(8);
    void* c = alloc(128);
    void* d = alloc(128);


    cout<<"iteration after allocations"<<endl;
    iter_header_list();
    free_addr(c);
    cout<<"iteration after freeing"<<endl;
    iter_header_list();

    alloc(4);
    cout<<"iteration after alloc again"<<endl;
    iter_header_list();

}

