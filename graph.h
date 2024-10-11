////////////////////////////////////////////////////////////
// Θα υλοποιησουμε τον γραφο μεσω διπλας συνδεδεμενης λιστας.
// Οι μονες πληροφοριες που χρειαζεται να αποθηκευσουμε σε αυτον ειναι οι κορυφες του
////////////////////////////////////////////////////////////
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "list.h"
// #include "hash.h"
#include <stdbool.h>

typedef struct hash_table* HashTable;

typedef struct graph* Graph;
typedef struct graph_node* GraphNode;
typedef struct edge* Edge;

//η DestroyFunc ειναι δεικτης σε συναρτηση που καταστρεφει το value
typedef void (*DestroyValueFunc)(Pointer value); 

Graph graphCreate();
void graphAdd(Graph graph, int user, HashTable hash_table); //δημιουργια και προσθηκη κομβου
int graphSize(Graph graph); //επιστρεφει το μεγεθος του γραφου
int graphGetUser(GraphNode graph_node);

void graphDestroy(Graph graph, DestroyValueFunc func);
void graphDestroyNode(Pointer graph_node);


///////////////////// ΔΕΝ ΕΧΩ ΥΛΟΠΟΙΗΣΕΙ
void graphRemove(Graph graph, int user, HashTable hash_table);
void graphDisplay(Graph graph);

//////// ΣΥΝΑΡΤΗΣΕΙΣ ΔΙΑΧΕΙΡΙΣΗΣ ΑΚΜΩΝ
void edgeAdd(Graph graph, int amount, char* date, int source_user, int dest_user, HashTable hash_table);
Edge edgeFind(Graph graph, int source_user, int dest_user, HashTable hash_table);
void edgeRemove(Graph graph, int source_user, int dest_user, HashTable hash_table);

///////////////////// ΔΕΝ ΕΧΩ ΥΛΟΠΟΙΗΣΕΙ
void edgeValueDestroy();

void edgeDestroyValueForIncoming(Pointer value);
void edgeDestroyValueForOutgoing(Pointer value);


void incomingEdgeDestroyValue(Pointer value);
void outgoingEdgesDestroyValue(Pointer value);


void incomingEdgesDestroy(GraphNode graph_node);
void outgoingEdgesDestroy(GraphNode graph_node);



