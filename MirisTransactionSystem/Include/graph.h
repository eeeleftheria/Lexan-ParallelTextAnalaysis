//////////////////////////////////////////////////////////////////
// We will implement the graph using a doubly linked list.      // 
// The only information we need to store in it are the vertices //
//////////////////////////////////////////////////////////////////
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "list.h"
// #include "hash.h"
#include <stdbool.h>
#include <unistd.h>


typedef struct hash_table* HashTable;

typedef struct graph* Graph;
typedef struct graph_node* GraphNode;
typedef struct edge* Edge;

// DestroyFunc is a pointer to a function that destroys the value
typedef void (*DestroyValueFunc)(Pointer value); 


Graph graphCreate();
void graphAdd(Graph graph, int user, HashTable hash_table); // create and add a node with name user
void graphRemove(Graph graph, int user, HashTable hash_table); // remove a vertex with a specific user


// DESTROY FUNCTIONS FOR VERTICES
void graphDestroy(Graph graph, DestroyValueFunc func, HashTable table);
void graphDestroyNode(Pointer graph_node); // destroys a node of the graph, along with what it contains
// that is the two lists with incoming and outgoing edges and then frees the graph's memory



// Helper functions
int graphSize(Graph graph); // returns the size of the graph
int graphGetUser(GraphNode graph_node); // returns the user of the given node
void graphDisplay(Graph graph, FILE* output, HashTable table);


///////////////////////////////////////////////
//////// EDGE MANAGEMENT FUNCTIONS ////////
///////////////////////////////////////////////

// add an edge (transaction) from user source_user to user dest_user
void edgeAdd(Graph graph, int amount, char* date, int source_user, int dest_user, HashTable hash_table);

// find a transaction between source_user - dest_user
Edge edgeFind(Graph graph, int source_user, int dest_user, HashTable hash_table);

// find edge with amount of transaction equal to sum
Edge edgeFindWithAmountAndDate(Graph graph, int source_user, int dest_user, int sum, char* date, HashTable hash_table);

// remove an edge between source_user - dest_user
void edgeRemove(Graph graph, int source_user, int dest_user, HashTable hash_table);

void edgesOfNodeDisplay(Graph graph, int user, HashTable table, FILE* output);
void edgesIncomingOfNodeDisplay(Graph graph, int user, HashTable table);
void edgesOutgoingOfNodeDisplay(Graph graph, int user, HashTable table);

// find the edge with amount and date equal to old_sum and old_date 
// respectively and set them to new_sum and new_date
int edgeModify(Graph graph, HashTable table, int source, int dest, int old_sum, int new_sum, char* old_date, char* new_date);

// DESTROY FUNCTIONS FOR EDGES

// destroys the incoming edges list of a vertex
void incomingEdgesDestroy(GraphNode graph_node);

// destroys the outgoing edges list of a vertex
void outgoingEdgesDestroy(GraphNode graph_node);

// frees the memory of the edge value
void edgeDestroyValueForOutgoing(Pointer value);

// does nothing, its definition is needed to be passed as a parameter to list destroy
// the edge must be freed only once, since it exists in both lists.
// so this happens in the above function for the outgoing edges
void edgeDestroyValueForIncoming(Pointer value);






