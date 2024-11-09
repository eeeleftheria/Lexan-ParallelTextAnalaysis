////////////////////////////////////////////
// ΥΛΟΠΟΙΗΣΗ HASHTABLE ΜΕ SEPERATE CHAINING
// 
//  Για μεγεθος πινακα M και κλειδι k, η hαsh function μας θα ειναι
// h(k) = k mod Μ οπου ο M πρεπει να ειναι πρωτος αριθμος
//
////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "hash.h"


// // HashTable: περιεχει ολες τις πληροφοριες του hash table
// struct hash_table {
// 	HashNode* array;	//πινακας που θα εχει δεικτες σε hash_nodes τα οποια θα δειχνουν σε λιστες
// 	int occupied_buckets; //ποσες θεσεις εχουμε γεμισει		
// 	int size_of_array;	//ποσες θεσεις εχει ο πινακας
// };

// struct hash_node{
//     List list; //λιστα με ολα τα graphNodes που εχουν το ιδιο h(k)
// };


// int hashFunc(int key, int M){
//     int pos = key % M;
//     return pos;
// }


// HashTable hashCreate(int size){
//     HashTable hash_table = malloc(sizeof(struct hash_table));
    
//     hash_table->array = malloc(size*sizeof(struct hash_node));
//     hash_table->occupied_buckets = 0;
//     hash_table->size_of_array = size;

//     for (int i = 0; i < size; i++) {
//         hash_table->array[i] = NULL; 
//     }

//     return hash_table;  

// }


// void hashAdd(HashTable hash_table, int key, Pointer value){

//     int pos = hashFunc(key, hash_table->size_of_array); //σε ποια θεση του hash table πρεπει να μπει

//     //μονο αν ειναι NULL ο κομβος τον δημιουργουμε και φτιαχνουμε τη λιστα
//     if(hash_table->array[pos] == NULL) {
 
//         HashNode hash_node = malloc(sizeof(struct hash_node));

//         if(hash_node == NULL) {
//             printf("failed to allocate memory for hash node\n");
//         }
    
//         hash_table->array[pos] = hash_node;
//         hash_node->list = listCreate(); //δημιουργια της λιστας για το συγκεκριμενο hashtable node
//         listInsert(hash_node->list, value); //προσθηκη του κομβου στη λιστα
//         hash_table->occupied_buckets++; //αυξηση μονο οταν δημιουργουμε νεο hash node
        
//     }

//     //αν δεν ειναι NULL σημαινει οτι εχει δημιουργηθει ηδη και υπαρχει η λιστα
//     // οποτε αρκει να προσθεσουμε σε αυτην
//     else if(hash_table->array[pos] != NULL){
//         listInsert(hash_table->array[pos]->list, value);
//     }   
// }


// int hashSize(HashTable hash_table){
//     return hash_table->occupied_buckets;
// }



// void hashDestroyValue(Pointer value){
//     free(value);
// }

//αφαιρει εναν κομβο απο το hash_table
// void hashRemove(HashTable hash_table, int key, DestroyValueFunc func, CompareFunc compare){
//     int pos = hashFunc(key, hash_table->size_of_array);

//     List list = hash_table->array[pos]->list;

//     listRemove(list, hashFindListNodeWithKey(hash_table, key, compare), hashDestroyValue);

//     hash_table->occupied_buckets--; 

// }   

//για την καταστροφη του hash table, διατρεχουμε τον πινακα και για καθε θεση του 
//διατρεχουμε την αντιστοιχη λιστα, αφαιρωντας ολους τους pointers στα nodes
//και καταστρεφοντας τα value τους

// void hashDestroy(HashTable hash_table){
   
//     for(int i = 0; i < hash_table->size_of_array; i++) {
        
//         if(hash_table->array[i] != NULL) {
//             List list = hash_table->array[i]->list;

//             if (listSize(list) != 0){

//             ListNode node = listFirst(list);

//                 while(node != NULL) {
//                     ListNode next = listGetNext(node);
//                     free(node);
//                     node = next;
//                 }
//             }
//             free(list);
//             free(hash_table->array[i]);
//         }
//     }
//     free(hash_table->array);
//     free(hash_table);
// }

// Pointer hashFindListNodeWithValue(HashTable hash_table, Pointer value, CompareFunc compare){
    // int pos = hashFunc(key, hash_table->size_of_array);

    // List list = hash_table->array[pos]->list;
    // //τα nodes της λιστας εχουν value graph node
    // //αρα πρεπει να βρουμε τον graph node του αντιστοιχου user
    // for(ListNode node = listFirst(list); node != NULL; node = listGetNext(node)){
    //     Pointer value = listNodeValue(list, node);
        
    //     if(compare(value, key) == 0){ 
    //         return node;
    //     }
    // }
    // return NULL;
// }





