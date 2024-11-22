#include <stdio.h>
#include <stdlib.h>


int global_v = 8;
int main() {
    int local_v;  // Stack
    int* heap_v = malloc(sizeof(int));  

    printf("Text segment address: %p\n", &main); //διεθυνση text segment
    
    printf("Initialized data address: %p\n", &global_v); //διευθυνση initialized data
    printf("Stack address: %p\n", &local_v); //διευθυνση στοιβας
    free(heap_v);
        
    printf("Heap address: %p\n", heap_v); //διευθυνση σωρου
    return 0;
}
