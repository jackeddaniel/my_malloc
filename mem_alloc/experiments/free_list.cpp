#include<iostream>

using namespace std;

int HEAP_SIZE = 1 << 20;
int MIN_BLOCK_SIZE = 32;


void initialize_free_list() {
    int start_size = MIN_BLOCK_SIZE;
    int heap_size = HEAP_SIZE;

    int total_lists = 0;

    for(int i = start_size; start_size <= HEAP_SIZE; start_size = start_size << 1) {
        total_lists++;
    }
    cout<<"The number of lists we'll have: "<<total_lists<<endl;    

}

int main() {
    initialize_free_list();
}
