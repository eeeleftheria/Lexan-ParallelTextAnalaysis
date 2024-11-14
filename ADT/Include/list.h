#pragma once

#include <stdio.h>
#include <stdbool.h>

typedef void* Pointer; //μπορουμε να αποθηκευσουμε οτι τυπο θελουμε αλλα ΟΧΙ να τον χρησιμοποιησουμε(να ορισουμε περιεχομενο)

typedef struct list* List; //List δεικτης σε struct list
typedef struct list_node* ListNode;

//η DestroyFunc ειναι δεικτης σε συναρτηση που καταστρεφει το value
typedef void (*DestroyValueFunc)(Pointer value); 

typedef int (*CompareFunc)(Pointer a, Pointer b);


List listCreate(); //δημιουργια κενης λιστας
void listInsert(List list, Pointer value); //δημιουργια και προσθηκη κομβου σε μια λιστα με τιμη value
void listRemove(List list, ListNode node, DestroyValueFunc func); //αφαιρεση κομβου απο μια λιστα 
int listSize(List list); //επιστρεφει το μεγεθος της λιστας
Pointer listNodeValue(List list, ListNode node); //επιστρεφει το value του κομβου
ListNode listFirst(List list); //επιστρεφει το πρωτο στοιχειο της λιστας
ListNode listGetNext(ListNode node);
ListNode listfindNodeWithValue(List list, Pointer value, CompareFunc compare); //επιστρεφει αν βρεθει τον κομβο με τιμη value


void listDestroy(List list, DestroyValueFunc func);
void listDestroyValue(List list, ListNode node, DestroyValueFunc func);