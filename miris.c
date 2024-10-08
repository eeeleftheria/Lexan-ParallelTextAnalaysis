#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "graph.h"


int main() {

    char name[] = "eri";
    char name2[] = "stef";
    char name3[] = "koala";

    Graph graph = graphCreate();
    graphAdd(graph, name);
    graphAdd(graph, name2);
    graphAdd(graph, name3);




    printf("size is %d\n", graphSize(graph));

    // graphDestroy(graph);

}