/////////////////////////////////////////////////////////////
// HASH TABLE IMPLEMENTATION WITH SEPARATE CHAINING        //
//                                                         //
//  For table size M and key k, our hash function will be  //
//  h(k) = k mod M where M should be a prime number        //
//                                                         //
/////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "hash.h"
#include "graph.h"



// HashTable: contains all information of the hash table
struct hash_table {
	HashNode* array;	// array that will have pointers to hash_nodes which point to lists
	int occupied_buckets; // how many positions we have filled		
	int size_of_array;	// how many positions the array has
};

struct hash_node{

    List list; // list with all graphNodes that have the same h(k)
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

    int pos = hashFunc(key, hash_table->size_of_array); // which position in the hash table it should go to

    // only if the node is NULL do we create it and set up the list
    if(hash_table->array[pos] == NULL) {
 
        HashNode hash_node = malloc(sizeof(struct hash_node));

        if(hash_node == NULL) {
            printf("failed to allocate memory for hash node\n");
        }
    
        hash_table->array[pos] = hash_node;
        hash_node->list = listCreate(); // create the list for this particular hashtable node
        listInsert(hash_node->list, value); // add the vertex to the list
        hash_table->occupied_buckets++; // increase only when we create a new hash node
        
    }

    // if it is not NULL it means it has already been created and the list exists
    // so we just need to add to it
    else if(hash_table->array[pos] != NULL){
        
        listInsert(hash_table->array[pos]->list, value);
       
    }   
}



int hashSize(HashTable hash_table){
    return hash_table->occupied_buckets;
}



// for a given user we want to return their information
Pointer hashFindGraphNodeWithKey(HashTable hash_table, int user){
    int pos = hashFunc(user, hash_table->size_of_array);

    if (hash_table->array[pos] == NULL) {
        return NULL;
    }
    else{

        List list = hash_table->array[pos]->list;
        // the nodes of the list have graph node values
        // so we need to find the graph node of the corresponding user
        for(ListNode node = listFirst(list); node != NULL; node = listGetNext(node)){
            GraphNode graph_node = listNodeValue(list, node);
            
            if(graphGetUser(graph_node) == user) {
                return graph_node;
            }
        }
    }
    return NULL;
}


// return the list node that contains the user
Pointer hashFindListNodeWithKey(HashTable hash_table, int user){
    int pos = hashFunc(user, hash_table->size_of_array);

    List list = hash_table->array[pos]->list;
    // the nodes of the list have graph node values
    // so we need to find the graph node of the corresponding user
    for(ListNode node = listFirst(list); node != NULL; node = listGetNext(node)){
        GraphNode graph_node = listNodeValue(list, node);
        
        if(graphGetUser(graph_node) == user) {
            return node;
        }
    }
    return NULL;
}



void hashDestroyValue(Pointer value){

    // we do not want to destroy the value!!!!!!!!
    // the graph remove already does it
}

// remove a vertex from the hash_table
void hashRemove(HashTable hash_table, int user, DestroyValueFunc func){
    int pos = hashFunc(user, hash_table->size_of_array);

    List list = hash_table->array[pos]->list;

    listRemove(list, hashFindListNodeWithKey(hash_table, user), hashDestroyValue);

    hash_table->occupied_buckets--; 



}   

// to destroy the hash table, we traverse the array and for each position
// we traverse the corresponding list, removing all pointers to nodes
// we do NOT want to delete the nodes themselves, since they are graph nodes and
// this would lead to a double free
void hashDestroy(HashTable hash_table){
   
    for(int i = 0; i < hash_table->size_of_array; i++) {
        
        if(hash_table->array[i] != NULL) {
            List list = hash_table->array[i]->list;

            if (listSize(list) != 0){

            ListNode node = listFirst(list);

                while(node != NULL) {
                    ListNode next = listGetNext(node);
                    free(node);
                    node = next;
                }
            }
            free(list);
            free(hash_table->array[i]);
        }
    }
    free(hash_table->array);
    free(hash_table);
}




Pointer hashFindListNodeWithValue(HashTable hash_table, Pointer value){
    
    int user = graphGetUser(value);
    ListNode node = hashFindListNodeWithKey(hash_table, user);
    return node;
}











