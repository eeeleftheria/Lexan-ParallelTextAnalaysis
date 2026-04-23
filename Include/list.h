#pragma once

#include <stdio.h>
#include <stdbool.h>

typedef void* Pointer; // we can store any type we want but NOT use it (define content)

typedef struct list* List; // List pointer to struct list
typedef struct list_node* ListNode;

// DestroyFunc is a pointer to a function that destroys the value
typedef void (*DestroyValueFunc)(Pointer value); 

typedef int (*CompareFunc)(Pointer a, Pointer b);


List listCreate(); // creates an empty list
void listInsert(List list, Pointer value); // creates and adds a node to a list with value
void listRemove(List list, ListNode node, DestroyValueFunc func); // removes a node from a list
int listSize(List list); // returns the size of the list
Pointer listNodeValue(List list, ListNode node); // returns the value of the node
ListNode listFirst(List list); // returns the first element of the list
ListNode listGetNext(ListNode node);
ListNode listfindNodeWithValue(List list, Pointer value, CompareFunc compare); // returns the node with value if found


void listDestroy(List list, DestroyValueFunc func);
void listDestroyValue(List list, ListNode node, DestroyValueFunc func);