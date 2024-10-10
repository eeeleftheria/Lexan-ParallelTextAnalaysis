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

    Graph graph = graphCreate();
    graphAdd(graph, name, hash_table);
    graphAdd(graph, name2, hash_table);
    graphAdd(graph, name3, hash_table);

    graphRemove(graph, 2);


    printf("size of list is %d\n", graphSize(graph));
    printf("size of hash is %d\n", hashSize(hash_table));

    // GraphNode node = hashFindNodeWithKey(hash_table, 2);
    // printf("node with key 2 is : %d\n\n", graphGetUser(node) );


    // graphDestroy(graph, graphDestroyNode);
    // hashDestroy(hash_table);

}