#pragma once
#include "graph.h"


typedef struct hash_node* HashNode;
typedef struct hash_table* HashTable;
typedef void* Pointer;

//δεικτης σε συναρτηση που δεχεται δυο Pointer a, b και επιστρεφει εναν int
typedef int (*CompareFunc)(Pointer a, Pointer b);


int hashFunc(int user, int M); // hush function που παιρνει ενα κλειδι και το αντιστοιχιζει σε ενα index 

HashTable hashCreate(int size);
void hashAdd(HashTable hash_table, int key, Pointer value);
int hashSize(HashTable hash_table);

//για εναν δεδομενο user θελουμε να επιστρεψουμε τα στοιχεια του
Pointer hashFindGraphNodeWithKey(HashTable hash_table, int user);
Pointer hashFindListNodeWithKey(HashTable hash_table, int user);
Pointer hashFindListNodeWithValue(HashTable hash_table, Pointer value);

//αφαιρουμε εναν δεδομενο user απο το hash table
void hashRemoveNodewithkey(HashTable hash_table, int user, DestroyValueFunc func);

void hashDestroy(HashTable hash_table);
void hashDestroyListNode(ListNode node);



