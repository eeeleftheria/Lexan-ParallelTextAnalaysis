#include "hash.h"
#include "list.h"


int compareWords(Pointer a, Pointer b);

void builderStoreInTable(HashTable table, char* word);

void builderSendToRoot(HashTable table, int fd);

