/////////////////////////////////////////////////////////////////////
// A LIST HAS THE PROPERTIES:                                      // 
// 1) INSERTION/DELETION ANYWHERE                                  //
// 2) SEQUENTIAL SEARCH                                            //
//                                                                 //
// for the implementation of the graph we are only interested in   //
// insertion at the END of the list and deletion anywhere          //
/////////////////////////////////////////////////////////////////////

#pragma once

#include <stdio.h>
#include <stdbool.h>

typedef void* Pointer; // we can store any type we want but NOT dereference it

typedef struct list* List; // List pointer to struct list
typedef struct list_node* ListNode;

// DestroyFunc is a pointer to a function that destroys the value
typedef void (*DestroyValueFunc)(Pointer value); 



List listCreate(); // create empty list
void listInsert(List list, Pointer value); // create and add a node to a list with value
void listRemove(List list, ListNode node, DestroyValueFunc func); // remove a node from a list
int listSize(List list); // returns the size of the list
Pointer listNodeValue(List list, ListNode node); // returns the value of the node
ListNode listFirst(List list); // returns the first element of the list
ListNode listGetNext(ListNode node); // returns node's next list node
ListNode findNodeWithValue(List list, Pointer value); // returns the node with value if found


void listDestroy(List list, DestroyValueFunc func);
void listDestroyValue(List list, ListNode node, DestroyValueFunc func);


