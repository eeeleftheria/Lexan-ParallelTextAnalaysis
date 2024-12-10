#include "list.h"

typedef struct hash_node* HashNode;
typedef struct hash_table* HashTable;
typedef void* Pointer; //μπορει να δειχνει σε οποιοδηποτε τυπο, χωρις να οριστει το περιεχομενο του *p

//δεικτης σε συναρτηση που δεχεται δυο Pointer a, b και επιστρεφει εναν int
typedef int (*CompareFunc)(Pointer a, Pointer b);

//δεικτης σε συναρτηση που καταστρεφει την τιμη value
typedef void (*DestroyValueFunc)(Pointer value); 

// hash function που παιρνει ενα κλειδι και το αντιστοιχιζει σε ενα index 
int hashFunc(char* word, int numer_of_builders); 

//δημιουργια κενου hash table
HashTable hashCreate(int size, CompareFunc func);


//προσθετει εναν κομβο με κλειδι key και τιμη value στο hash table
void hashAdd(HashTable hash_table, Pointer key, Pointer value);

//αφαιρουμε εναν δεδομενο κομβο με κλειδι key απο το hash table
void hashRemove(HashTable hash_table, Pointer key);

//για ενα δεδομενο κομβο με κλειδι key επιστρεφουμε τον list node στον οποιο ανηκει
ListNode hashFindListNodeWithKey(HashTable hash_table, Pointer key);

Pointer hashFindValue(HashTable table, Pointer key);

//ΣΥΝΑΡΤΗΣΕΙΣ DESTROY 
void hashDestroy(HashTable hash_table);
void hashDestroyNode(Pointer hash_node);

//display
void hashDisplay(HashTable table);

//getters
int hashGetSizeOfArray(HashTable table);

//επιστρεφει τον αριθμο κομβων του hash table
int hashGetSize(HashTable table);

int hashGetSizeOfList(HashTable table, int pos);

HashNode hashGetNext(HashTable table, int pos, HashNode node, CompareFunc compare);

HashNode hashGetFirst(HashTable table, int pos);

Pointer hashGetValue(HashNode node);

Pointer hashGetKey(HashNode node);



