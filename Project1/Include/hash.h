#pragma once
#include "graph.h"


typedef struct hash_node* HashNode;
typedef struct hash_table* HashTable;
typedef void* Pointer;

// pointer to a function that takes two Pointers a, b and returns an int
typedef int (*CompareFunc)(Pointer a, Pointer b);


int hashFunc(int user, int M); // hash function that takes a key and maps it to an index 

HashTable hashCreate(int size);

// adds a vertex to the hash table
void hashAdd(HashTable hash_table, int user, Pointer value);

// remove a given user from the hash table
void hashRemove(HashTable hash_table, int user, DestroyValueFunc func);

// returns the size of the hash
int hashSize(HashTable hash_table);


// for a given user we want to return their information
Pointer hashFindGraphNodeWithKey(HashTable hash_table, int user);

// for a given user we return the list node it belongs to
Pointer hashFindListNodeWithKey(HashTable hash_table, int user);

// for a given graph node we return the list node it belongs to
Pointer hashFindListNodeWithValue(HashTable hash_table, Pointer value);


// DESTROY FUNCTIONS

void hashDestroy(HashTable hash_table);

// does nothing, its definition is needed for list destroy
// the value is a graph node and is destroyed by graph destroy
void hashDestroyValue(Pointer value);



