
typedef struct hash_node* HashNode;
typedef struct hash_table* HashTable;
typedef void* Pointer;

//δεικτης σε συναρτηση που δεχεται δυο Pointer a, b και επιστρεφει εναν int
typedef int (*CompareFunc)(Pointer a, Pointer b);


int hashFunc(int user, int M); // hush function που παιρνει ενα κλειδι και το αντιστοιχιζει σε ενα index 

HashTable hashCreate(int size);
void hashAdd(HashTable hash_table, int key, Pointer value);
int hashSize(HashTable hash_table);



