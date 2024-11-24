#include "hash.h"
#include "list.h"

//δεικτης σε συναρτηση που συγκρινει δυο Pointers
typedef int (*CompareFunc)(Pointer a, Pointer b);

//συναρτηση που συγκρινει δυο λεξεις
int compareWords(Pointer a, Pointer b);

//αποθηκευει μια λεξη που ελαβε απο splitter σε μια δομη HashTable
void builderStoreInTable(HashTable table, char* word);

//αφου αποθηκευσει ολες τις λεξεις στην δομη, τις στελνει στον root
void builderSendToRoot(HashTable table, CompareFunc compare, int fd_root_write);

//συγκρινει δυο κομβους του HashTable με βαση το κλειδι τους
int compareHashNodes(Pointer a, Pointer b);