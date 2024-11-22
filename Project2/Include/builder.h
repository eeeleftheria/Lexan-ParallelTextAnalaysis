#include "hash.h"
#include "list.h"

typedef int (*CompareFunc)(Pointer a, Pointer b);

int compareWords(Pointer a, Pointer b);

void builderStoreInTable(HashTable table, char* word);

void builderSendToRoot(HashTable table, CompareFunc compare, int fd_root_write);

int compareHashNodes(Pointer a, Pointer b);