////////////////////////////////////////////////////////////
// Θα υλοποιησουμε τον γραφο μεσω διπλας συνδεδεμενης λιστας.
// Οι μονες πληροφοριες που χρειαζεται να αποθηκευσουμε σε αυτον ειναι οι κορυφες του
////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include <stdbool.h>


typedef struct graph* Graph;
typedef struct graph_node* GraphNode;
typedef struct edge* Edge;

Graph graphCreate();
void graphAdd(Graph graph, int user); //δημιουργια και προσθηκη κομβου
int graphSize(Graph graph); //επιστρεφει το μεγεθος του γραφου

void graphDestroy(Graph graph);
void graphDestroyNode(GraphNode graph_node);


///////////////////// ΔΕΝ ΕΧΩ ΥΛΟΠΟΙΗΣΕΙ
void graphPrint(Graph graph);
bool graphContainsNode(Graph graph, GraphNode node); //επιστρεφει true αν υπαρχει ο κομβος μεσα στο γραφο
void graphRemove(Graph graph, GraphNode node);

//////// ΣΥΝΑΡΤΗΣΕΙΣ ΔΙΑΧΕΙΡΙΣΗΣ ΑΚΜΩΝ
void addEdge(Graph graph, int amount, char* date, GraphNode source_node, GraphNode dest_node);



