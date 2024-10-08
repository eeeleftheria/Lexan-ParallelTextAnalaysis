////////////////////////////////////////////////////////////
// Θα υλοποιησουμε τον γραφο μεσω διπλας συνδεδεμενης λιστας.
// Οι μονες πληροφοριες που χρειαζεται να αποθηκευσουμε σε αυτον ειναι οι κορυφες του
////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include "list.h"

typedef struct graph* Graph;
typedef struct graph_node* GraphNode;
typedef struct edge* Edge;

Graph graphCreate();
GraphNode graphCreateNode(char* user);
void graphAdd(Graph graph, GraphNode node);

int graphSize(Graph graph);

void graphDestroy(Graph graph);
void graphDestroyNode(GraphNode graph_node);

/////////////////////
void graphPrint(Graph graph);
void graphRemove(Graph graph, GraphNode node);



