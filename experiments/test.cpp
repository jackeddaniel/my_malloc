#include <iostream>
#include <unistd.h>    // For sbrk, brk
#include <sys/mman.h>  // For mmap
#include <fstream>     // For reading /proc/self/maps
#include <string>

void print_memory_map() {
    std::cout << "--- Memory Map (/proc/self/maps) ---" << std::endl;
    std::ifstream maps("/proc/self/maps");
    std::string line;
    while (std::getline(maps, line)) {
        if (line.find("[heap]") != std::string::npos || line.find("[stack]") != std::string::npos) {
            std::cout << line << std::endl;
        }
    }
}

void check_stack_depth(void* main_stack_addr) {
    int local_var = 42;
    size_t stack_used = (size_t)main_stack_addr - (size_t)&local_var;
    std::cout << "Current Stack Usage: " << stack_used << " bytes" << std::endl;
}

int main() {
    int stack_start_marker = 0; // Local variable to mark stack start
    
    void* initial_break = sbrk(0);
    std::cout << "Initial Program Break (Heap End): " << initial_break << std::endl;

    sbrk(4096);     void* new_break = sbrk(0);
    std::cout << "New Program Break after sbrk(4096): " << new_break << std::endl;

    size_t mmap_size = 8192;
    void* mmap_ptr = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, 
                          MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    if (mmap_ptr != MAP_FAILED) {
        std::cout << "mmap allocated 8KB at: " << mmap_ptr << std::endl;
    }

    print_memory_map();
    check_stack_depth(&stack_start_marker);

    munmap(mmap_ptr, mmap_size);
    
    return 0;
}
