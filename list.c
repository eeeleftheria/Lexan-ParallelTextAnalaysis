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
    List list = malloc(sizeof(struct list)); // ή malloc(sizeof(*list))

    list->first = NULL;
    list->last = NULL;
    list->first->prev = NULL;
    list->last->next = NULL;

    list->size = 0;

    return list;

}

//προσθετουμε εναν κομβο στο τελος της λιστας, δεν μας ενδιαφερει να τον προσθεσουμε σε συγκεκριμενο σημειο
void listInsert(List list, ListNode node, Pointer value) {
    
    if(list->size == 0) { //κενη λιστα
        list->first = node;
        node->next = NULL;
        node->prev = NULL;
    }

    else { //μη κενη λιστα: προσθετουμε στο τελος και συνδεουμε με τον προηγουμενο κομβο
    //ο prev του node θα ειναι ο παλιος last
        list->last->next = node;
        node->prev = list->last;
        list->last = node;
        node->next = NULL;
    }

    list->size++;

}

void listDelete(List list, ListNode node){

    ListNode prev = node->prev;
    ListNode next = node->next;

    //αν ο node ειναι ο πρωτος της λιστας πρεπει ο επομενος του να γινει πρωτος
    if (node == list->first) {
        list->first = next;
    }
    //αν ο node ειναι ο τελευταιος της λιστας πρεπει ο προηγουμενος του να γινει τελευταιος
    else if (node == list->last){
        list->last = prev;
    }

    //αν ο node βρισκεται ενδιαμεσα, πρεπει να συνδεσουμε τον προηγουμενο του με τον επομενο του
    else{
        prev->next = next;
        next->prev = prev;
    }

    free(node);
    list->size--;

}



