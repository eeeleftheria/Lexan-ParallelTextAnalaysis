#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "graph.h"
#include "hash.h"


int main() {
    HashTable hash_table = hashCreate(21);

    int name = 1;
    int name2 = 2;
    int name3 = 3;
    int name4 = 6;

    char date[] = "7/7/2024";

    Graph graph = graphCreate();
    graphAdd(graph, name, hash_table);
    graphAdd(graph, name2, hash_table);
    graphAdd(graph, name3, hash_table);
    graphAdd(graph, name4, hash_table);


    graphDisplay(graph);


    // printf("size of list is %d\n", graphSize(graph));
    // printf("size of hash is %d\n", hashSize(hash_table));


    // graphRemove(graph, 2, hash_table);
    
    edgeAdd(graph, 45, date, 4, 2, hash_table);
    edgeAdd(graph, 100, date, 4, 3, hash_table);


    // printf("\nGRAPH AFTER DELETION OF NODE: \n");
    // graphDisplay(graph);

    graphDestroy(graph, graphDestroyNode);
    hashDestroy(hash_table);

}