#include "hash.h"
#include "list.h"

// pointer to a function that compares two Pointers
typedef int (*CompareFunc)(Pointer a, Pointer b);

// function that compares two words
int compareWords(Pointer a, Pointer b);

// stores a word received from splitter in a HashTable data structure
void builderStoreInTable(HashTable table, char* word);

// after storing all words in the data structure, sends them to root
void builderSendToRoot(HashTable table, CompareFunc compare, int fd_root_write);

// compares two HashTable nodes based on their key
int compareHashNodes(Pointer a, Pointer b);