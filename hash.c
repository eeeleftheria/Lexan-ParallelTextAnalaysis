////////////////////////////////////////////
// ΥΛΟΠΟΙΗΣΗ HASHTABLE ΜΕΣΩ HASH TABLE ΜΕ SEPERATE CHAINING
// 
//  Για μεγεθος πινακα M και κλειδι k, η hush function μας θα ειναι
// h(k) = k mod Μ οπου ο M πρεπει να ειναι πρωτος αριθμος
//
// Για load factor a πρεπει να ισχυει α < 0.9
/////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "hash.h"
#include "graph.h"



// HashTable: περιεχει ολες τις πληροφοριες του hash table
struct hash_table {
	HashNode* array;	//πινακας που θα εχει δεικτες σε hash_nodes τα οποια θα δειχνουν σε λιστες
	int occupied_buckets; //ποσες θεσεις εχουμε γεμισει		
	int size_of_array;	//ποσες θεσεις εχει ο πινακας
};

struct hash_node{

    // int key; ?????????????
    List list; //λιστα με ολα τα graphNodes που εχουν το ιδιο h(k)
};


int hashFunc(int user, int M){
    int pos = user % M;
    return pos;
}


HashTable hashCreate(int size){
    HashTable hash_table = malloc(sizeof(struct hash_table));
    
    hash_table->array = malloc(size*sizeof(struct hash_node));
    hash_table->occupied_buckets = 0;
    hash_table->size_of_array = size;

    for (int i = 0; i < size; i++) {
        hash_table->array[i] = NULL; 
    }

    return hash_table;

    

}


void hashAdd(HashTable hash_table, int key, Pointer value){

    int pos = hashFunc(key, hash_table->size_of_array); //σε ποια θεση του hash table πρεπει να μπει

    //μονο αν ειναι NULL ο κομβος τον δημιουργουμε και φτιαχνουμε τη λιστα
    if(hash_table->array[pos] == NULL) {
 
        HashNode hash_node = malloc(sizeof(struct hash_node));

        if(hash_node == NULL) {
            printf("failed to allocate memory for hash node\n");
        }
    
        hash_table->array[pos] = hash_node;
        hash_node->list = listCreate(); //δημιουργια της λιστας για το συγκεκριμενο hashtable node
        listInsert(hash_node->list, value); //προσθηκη της κορυφης στη λιστα
        hash_table->occupied_buckets++; //αυξηση μονο οταν δημιουργουμε νεο hash node
        
    }

    //αν δεν ειναι NULL σημαινει οτι εχει δημιουργηθει ηδη και υπαρχει η λιστα
    // οποτε αρκει να προσθεσουμε σε αυτην
    else if(hash_table->array[pos] != NULL){
        printf("IN HASH existing node\n\n");
        listInsert(hash_table->array[pos]->list, value);
       
    }

    
}



int hashSize(HashTable hash_table){
    return hash_table->occupied_buckets;
}

//για εναν δεδομενο user θελουμε να επιστρεψουμε τα στοιχεια του
Pointer hashFindGraphNodeWithKey(HashTable hash_table, int user){
    int pos = hashFunc(user, hash_table->size_of_array);

    List list = hash_table->array[pos]->list;
    //τα nodes της λιστας εχουν value graph node
    //αρα πρεπει να βρουμε τον graph node του αντιστοιχου user
    for(ListNode node = listFirst(list); node != NULL; node = listGetNext(node)){
        GraphNode graph_node = listNodeValue(list, node);
        
        if(graphGetUser(graph_node) == user) {
            return graph_node;
        }
    }
}

Pointer hashFindListNodeWithKey(HashTable hash_table, int user){
    int pos = hashFunc(user, hash_table->size_of_array);

    List list = hash_table->array[pos]->list;
    //τα nodes της λιστας εχουν value graph node
    //αρα πρεπει να βρουμε τον graph node του αντιστοιχου user
    for(ListNode node = listFirst(list); node != NULL; node = listGetNext(node)){
        GraphNode graph_node = listNodeValue(list, node);
        
        if(graphGetUser(graph_node) == user) {
            return node;
        }
    }
}



void hashRemoveNodewithkey(HashTable hash_table, int user){
    int pos = hashFunc(user, hash_table->size_of_array);

    List list = hash_table->array[pos]->list;

    listRemove(list, hashFindListNodeWithKey(hash_table, user));

}   









