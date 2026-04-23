#include <stdio.h>
#include <stdlib.h>
#include "list.h"


struct list{
    ListNode first;
    ListNode last;
    int size;
};

struct list_node{
    Pointer value;
    ListNode next;
    ListNode prev;
};


List listCreate() {
    List list = malloc(sizeof(struct list)); // or malloc(sizeof(*list))

    list->first = NULL;
    list->last = NULL;

    list->size = 0;

    return list;

}


// we add a node to the end of the list, we don't care to add it at a specific point
void listInsert(List list, Pointer value) {

    ListNode new_node = malloc(sizeof(struct list_node));
    new_node->value = value;

    if(new_node == NULL) {
        return;
    }
    
    if(list->size == 0) { // empty list-> first and last are the same
        list->first = new_node;
        list->last = new_node;
        new_node->next = NULL;
        new_node->prev = NULL;
    }

    else { // non-empty list: we add to the end and connect with the previous node
    // the prev of the node will be the old last
        list->last->next = new_node;
        new_node->prev = list->last;
        list->last = new_node;
        new_node->next = NULL;
    }

    list->size++;
}


void listRemove(List list, ListNode node, DestroyValueFunc func){

    if(node == NULL) {
        return;
    }

    ListNode prev = node->prev;
    ListNode next = node->next;

    // if only one node remains, then the list will remain empty
    if(list->size == 1) {
        list->first = NULL;
        list->last = NULL;
    }
    // if the node is the first of the list the next one should become first
    else if (node == list->first) {
        list->first = next;
        next->prev = NULL;
    }
    // if the node is the last of the list the previous one should become last
    else if (node == list->last){
        list->last = prev;
        prev->next = NULL;
    }

    // if the node is in the middle, we need to connect the previous with the next

    else{
        prev->next = next;
        next->prev = prev;
    }
    
    // we call destroy value -> different for each type of value
    func(node->value);  

    // freeing node memory
    free(node);
    node = NULL;

    // update size
    list->size--;

}


int listSize(List list){
    return list->size;
}


Pointer listNodeValue(List list, ListNode node){
    return node->value;
}


// of type DestroyFunc
void listDestroyValue(List list, ListNode node, DestroyValueFunc func){
    
    func(node->value); // destroys each value in an appropriate way
    // for example if we have graph nodes as value we need to free whatever element has inside
    // the graph node that takes up space in memory and then free it
   
    }


// traverses the entire list and destroys each element one by one
void listDestroy(List list, DestroyValueFunc func) {
      
    if (list->size != 0){   
        ListNode node = list->first;

        while(node != NULL) {
            ListNode next = node->next;
            listDestroyValue(list, node, func);
            free(node);
            node = next;
        }
    }

    free(list); // freeing memory from list
}

void listDestroyNode(Pointer node){
    free(node);
}

ListNode listFirst(List list){
    return list->first;
}


ListNode listGetNext(ListNode node){
    return node->next;
}


// finding node with value
ListNode findNodeWithValue(List list, Pointer value){
    ListNode node;

    // we traverse the entire list until we find the node with this value    
    for(node = listFirst(list); node != NULL; node = listGetNext(node)){
        
        if (listNodeValue(list, node) == value) {
            return node;
        }    
    }
    return NULL;

}






