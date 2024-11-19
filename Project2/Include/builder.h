#include "hash.h"
#include "list.h"

typedef int (*CompareFunc)(Pointer a, Pointer b);

int compareWords(Pointer a, Pointer b);

void builderStoreInTable(HashTable table, char* word);

void builderSendToRoot(HashTable table, int fd, CompareFunc compare);

