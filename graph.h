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
void graphAdd(Graph graph, int user, HashTable hash_table); //δημιουργια και προσθηκη κομβου με ονομα user
void graphRemove(Graph graph, int user, HashTable hash_table); //αφαιρεση κορυφης με συγκεκριμενο user


//ΣΥΝΑΡΤΗΣΕΙΣ DESTROY ΚΟΡΥΦΩΝ
void graphDestroy(Graph graph, DestroyValueFunc func, HashTable table);
void graphDestroyNode(Pointer graph_node); //καταστρεφει εναν κομβο του γραφου, μαζι με ο,τι αυτος περιεχει
//δηλαδη τις δυο λιστες με τις εισερχομενες και εξερχομενες ακμες και επειτα αποδεσμευει τη μνημη του γραφου



// Βοηθητικές συναρτήσεις
int graphSize(Graph graph); //επιστρεφει το μεγεθος του γραφου
int graphGetUser(GraphNode graph_node); //επιστρεφει τον user του συγκεκριμενου κομβου
void graphDisplay(Graph graph);


///////////////////////////////////////////////
//////// ΣΥΝΑΡΤΗΣΕΙΣ ΔΙΑΧΕΙΡΙΣΗΣ ΑΚΜΩΝ ////////
///////////////////////////////////////////////

//προσθηκη ακμης(συναλλαγης) απο τον χρηστη source_user προς τον dest_user
void edgeAdd(Graph graph, int amount, char* date, int source_user, int dest_user, HashTable hash_table);

//ευρεση συναλλαγης μεταξυ source_user - dest_user
Edge edgeFind(Graph graph, int source_user, int dest_user, HashTable hash_table);

//aφαιρεση ακμης μεταξυ source_user - dest_user
void edgeRemove(Graph graph, int source_user, int dest_user, HashTable hash_table);


//ΣΥΝΑΡΤΗΣΕΙΣ DESTROY ΑΚΜΩΝ

//καταστρεφει την λιστα incoming edges μιας κορυφης
void incomingEdgesDestroy(GraphNode graph_node);

//καταστρεφει την λιστα outgoing edges μιας κορυφης
void outgoingEdgesDestroy(GraphNode graph_node);

//αποδεσμευει την μνημη της ακμης value
void edgeDestroyValueForOutgoing(Pointer value);

//δεν κανει τιποτα, ο ορισμος της χρειαζεται για να περαστει ως παραμετρος στην list destroy
//η ακμη πρεπει να αποδεσμευτει μονο μια φορα, αφου υπαρχει και στις δυο λιστες.
//οποτε αυτο γινεται στην απο πανω συναρτηση για τις outgoing 
void edgeDestroyValueForIncoming(Pointer value);






