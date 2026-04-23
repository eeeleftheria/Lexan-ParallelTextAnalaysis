#include "list.h"

typedef struct hash_node* HashNode;
typedef struct hash_table* HashTable;
typedef void* Pointer; // can point to any type without defining the content of *p

// pointer to a function that takes two Pointers a, b and returns an int
typedef int (*CompareFunc)(Pointer a, Pointer b);

// pointer to a function that destroys the value
typedef void (*DestroyValueFunc)(Pointer value);

// hash function that takes a key and maps it to an index 
int hashFunc(char* word, int numer_of_builders);

// creates an empty hash table
HashTable hashCreate(int size, CompareFunc func);


// adds a node with key and value to the hash table
void hashAdd(HashTable hash_table, Pointer key, Pointer value);

// removes a given node with key from the hash table
void hashRemove(HashTable hash_table, Pointer key);

// for a given node with key returns the list node to which it belongs
ListNode hashFindListNodeWithKey(HashTable hash_table, Pointer key);

Pointer hashFindValue(HashTable table, Pointer key);

//destroy functions
void hashDestroy(HashTable hash_table);
void hashDestroyNode(Pointer hash_node);

//display
void hashDisplay(HashTable table);

//getters
int hashGetSizeOfArray(HashTable table);

// returns the number of nodes in the hash table
int hashGetSize(HashTable table);

int hashGetSizeOfList(HashTable table, int pos);

HashNode hashGetNext(HashTable table, int pos, HashNode node, CompareFunc compare);

HashNode hashGetFirst(HashTable table, int pos);

Pointer hashGetValue(HashNode node);

Pointer hashGetKey(HashNode node);



