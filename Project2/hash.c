////////////////////////////////////////////
//
// ΥΛΟΠΟΙΗΣΗ HASHTABLE ΜΕ SEPERATE CHAINING
//
////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "hash.h"


// HashTable: περιεχει ολες τις πληροφοριες του hash table
struct hash_table {
	List* array;	//πινακας που θα εχει δεικτες σε λιστες	
	int size_of_array;	//ποσες θεσεις εχει ο πινακας
    int size; //αριθμος κομβων 
    CompareFunc compare;
};
    
struct hash_node{
    Pointer key;
    Pointer value;
};


int hashFunc(char* word, int m){
    int key = 0;
    //προσθετουμε τους αριθμους ascii του καθε χαρακτηρα
    for(int i = 0; i < strlen(word); i++){
        key += word[i];
    }

    int pos = key % m;
    return pos;
}


HashTable hashCreate(int size, CompareFunc func){
    HashTable hash_table = malloc(sizeof(struct hash_table));
    
    hash_table->array = malloc(size*sizeof(List)); // δεσμευουμε χωρο για size δεικτες σε λιστες List
    hash_table->size_of_array = size;
    hash_table->compare = func; //ορισμος της compare func που συγκρινει τα κλειδια

    for (int i = 0; i < size; i++) {
        hash_table->array[i] = NULL; 
    }

    return hash_table;  

}



void hashAdd(HashTable hash_table, Pointer key, Pointer value){

    int pos = hashFunc(key, hash_table->size_of_array); //σε ποια θεση του hash table πρεπει να μπει

    //αν υπαρχει ηδη αυτο το κλειδι, δεν θελουμε να το ξαναβαλουμε
    if(hashFindListNodeWithKey(hash_table, key) != NULL){
        return;
    }

    //δημιουργια του hash node
    HashNode hash_node = malloc(sizeof(struct hash_node));

    if(hash_node == NULL) {
        printf("failed to allocate memory for hash node\n");
        return;
    }

    hash_node->key = key;
    hash_node->value = value;

    //μονο αν ειναι NULL δημιουργουμε τη λιστα
    if(hash_table->array[pos] == NULL) {
        
        hash_table->array[pos] = listCreate();  
    
        listInsert(hash_table->array[pos], hash_node); //προσθηκη του κομβου στη λιστα
        
    }

    //αν δεν ειναι NULL σημαινει οτι εχει δημιουργηθει ηδη η λιστα
    // οποτε αρκει να προσθεσουμε σε αυτην τον κομβο
    else if(hash_table->array[pos] != NULL){
        listInsert(hash_table->array[pos], hash_node);
    }   

    hash_table->size++;

}

//καταστρεφει το hash node
void hashDestroyNode(Pointer hash_node){
    HashNode node = (HashNode)hash_node;

    if(node->key != NULL){
        free(node->key);
        node->key = NULL;
    }

    // if (node->value != NULL) {
    //     free(node->value);
    //     node->value = NULL;
    // }


    free(node);
}

//για καθε θεση του πινακα, καταστρεφουμε τη λιστα
void hashDestroy(HashTable hash_table){
   
    for(int i = 0; i < hash_table->size_of_array; i++) {
        
        if(hash_table->array[i] != NULL) {
            List list = hash_table->array[i];
            
            listDestroy(list, hashDestroyNode);

        }
    }
    free(hash_table->array);
    free(hash_table);
}



//αφαιρει εναν κομβο απο το hash_table
void hashRemove(HashTable hash_table, Pointer key){
    int pos = hashFunc(key, hash_table->size_of_array);

    List list = hash_table->array[pos];
    ListNode node = hashFindListNodeWithKey(hash_table, key);

    if(node == NULL){
        return;
    }

    listRemove(list, node, hashDestroyNode);
    hash_table->size--;

}   


ListNode hashFindListNodeWithKey(HashTable hash_table, Pointer key){
    
    int pos = hashFunc(key, hash_table->size_of_array);

    List list = hash_table->array[pos];

    if(list == NULL){
        // printf("list null\n");
        return NULL;
    }
    
    //τα nodes της λιστας εχουν value hash node
    for(ListNode node = listFirst(list); node != NULL; node = listGetNext(node)){
        HashNode value = listNodeValue(list, node);
        Pointer key_to_find = value->key;
        
        //αν βρουμε τον hash node με αυτο το key τον επιστρεφουμε
        if(hash_table->compare(key_to_find, key) == 0){ 
            // printf("found same node \n");
            return node;
        }
    }
    return NULL;
}


void hashDisplay(HashTable table){
    if(table->size_of_array == 0){
        return;
    }

    for(int i = 0; i < table->size_of_array; i++){
        if(table->array[i] == NULL){
            continue;
        }

        List list = table->array[i];
        printf("bucket %d -> ",i);
        
        ListNode node;
        for(node = listFirst(list); node != NULL; node = listGetNext(node)){
            HashNode hash_node = listNodeValue(list, node);

            printf("key: %s ", (char*)hash_node->key);

            if(hash_node->value != NULL){
                printf("count: %d\n", (*(int*)hash_node->value));
            }
        }
    }
}


Pointer hashFindValue(HashTable table, Pointer key){
    int pos = hashFunc(key, table->size_of_array);

    List list = table->array[pos];
    if(list == NULL){
        return NULL;
    }

    ListNode node = hashFindListNodeWithKey(table, key);
    if(node == NULL){
        return NULL;
    }

    HashNode hash_node = listNodeValue(list, node);
    return hash_node->value;
}


List hashGetList(HashTable table, int pos){
    return table->array[pos];
}

int hashGetSizeOfList(HashTable table, int pos){

    if(table->array[pos] == NULL){
        return 0;
    }

    return listSize(table->array[pos]);
}


int hashGetSizeOfArray(HashTable table){
    return table->size_of_array;
}


int hashGetSize(HashTable table){
    return table->size;
}

HashNode hashGetNext(HashTable table, int pos, HashNode node, CompareFunc compare){
    
    ListNode list_node = listfindNodeWithValue(table->array[pos], node, compare);
    if(list_node == NULL){
        return NULL;
    }

    ListNode next = listGetNext(list_node);
    if(next == NULL){
        return NULL;
    }

    return listNodeValue(table->array[pos], next);
}

HashNode hashGetFirst(HashTable table, int pos){
    
    HashNode node = listNodeValue(table->array[pos], listFirst(table->array[pos]));

    if(node == NULL){
        return NULL;
    }

    return node;
}

Pointer hashGetKey(HashNode node){
    return node->key;
}

Pointer hashGetValue(HashNode node){
    return node->value;
}








