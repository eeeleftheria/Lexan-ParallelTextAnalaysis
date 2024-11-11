typedef struct hash_node* HashNode;
typedef struct hash_table* HashTable;
typedef void* Pointer;

//δεικτης σε συναρτηση που δεχεται δυο Pointer a, b και επιστρεφει εναν int
typedef int (*CompareFunc)(Pointer a, Pointer b);


int hashFunc(int key, int M); // hash function που παιρνει ενα κλειδι και το αντιστοιχιζει σε ενα index 

HashTable hashCreate(int size);

//προσθετει εναν κομβο στο hash table
void hashAdd(HashTable hash_table, int key, Pointer value);

//αφαιρουμε εναν δεδομενο κομβο με key απο το hash table
void hashRemove(HashTable hash_table, int key, DestroyValueFunc func, CompareFunc compare);

//επιστρεφει το size του hash
int hashSize(HashTable hash_table);

//για εναν δεδομενο key επιστρεφουμε το list node στο οποιο ανηκει 
// Pointer hashFindListNodeWithKey(HashTable hash_table, int key, CompareFunc compare);

//για εναν δεδομενο graph node επιστρεφουμε τον list node στον οποιο ανηκει
Pointer hashFindListNodeWithValue(HashTable hash_table, Pointer value, CompareFunc compare);


//ΣΥΝΑΡΤΗΣΕΙΣ DESTROY 

void hashDestroy(HashTable hash_table);

//δεν κανει τιποτα, ο ορισμος της χρειαζεται για την list destroy
//το value ειναι graph node και καταστρεφεται απο την graph destroy
void hashDestroyValue(Pointer value);
