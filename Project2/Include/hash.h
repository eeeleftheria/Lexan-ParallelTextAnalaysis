
#include "list.h"

typedef struct hash_node* HashNode;
typedef struct hash_table* HashTable;
typedef void* Pointer;

//δεικτης σε συναρτηση που δεχεται δυο Pointer a, b και επιστρεφει εναν int
typedef int (*CompareFunc)(Pointer a, Pointer b);
typedef void (*DestroyValueFunc)(Pointer value); 

int hashFunc(char* word, int numer_of_builders); // hash function που παιρνει ενα κλειδι και το αντιστοιχιζει σε ενα index 

HashTable hashCreate(int size);

//προσθετει εναν κομβο με κλειδι key και τιμη value στο hash table
void hashAdd(HashTable hash_table, Pointer key, Pointer value);

//αφαιρουμε εναν δεδομενο κομβο με κλειδι key απο το hash table
void hashRemove(HashTable hash_table, Pointer key, CompareFunc compare);

//για ενα δεδομενο κομβο με κλειδι key επιστρεφουμε τον list node στον οποιο ανηκει
ListNode hashFindListNodeWithKey(HashTable hash_table, Pointer key, CompareFunc compare);


//ΣΥΝΑΡΤΗΣΕΙΣ DESTROY 
void hashDestroy(HashTable hash_table);
void hashDestroyNode(Pointer hash_node);

//display
void hashDisplay(HashTable table);



