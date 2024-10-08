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

    list->size = 0;

    return list;

}

ListNode listCreateNode(Pointer value) {
    ListNode new_node = malloc(sizeof(struct list_node));

    new_node->value = value;

    return new_node;
}

//προσθετουμε εναν κομβο στο τελος της λιστας, δεν μας ενδιαφερει να τον προσθεσουμε σε συγκεκριμενο σημειο
void listInsert(List list, ListNode node) {

    if(node == NULL) {
        return;
    }
    
    if(list->size == 0) { //κενη λιστα
        list->first = node;
        list->last = node;
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

// void listRemove(List list, ListNode node){

//     ListNode prev = node->prev;
//     ListNode next = node->next;

//     //αν ο node ειναι ο πρωτος της λιστας πρεπει ο επομενος του να γινει πρωτος
//     if (node == list->first) {
//         list->first = next;
//     }
//     //αν ο node ειναι ο τελευταιος της λιστας πρεπει ο προηγουμενος του να γινει τελευταιος
//     else if (node == list->last){
//         list->last = prev;
//     }

//     //αν ο node βρισκεται ενδιαμεσα, πρεπει να συνδεσουμε τον προηγουμενο του με τον επομενο του
//     else{
//         prev->next = next;
//         next->prev = prev;
//     }

//     free(node);
//     list->size--;

// }





// ListNode findNode(List list, Pointer value){

// }

int listSize(List list){
    return list->size;
}


Pointer listNodeValue(List list, ListNode node){
    return node->value;
}


void listDestroyValue(List list, ListNode node){
    free(node->value);
}

void listDestroy(List list) {
      
    if (list->size != 0){   
        ListNode node = list->first;

        while(node != NULL) {
            ListNode next = node->next;
            listDestroyValue(list, node);
            free(node);
            node = next;
        }
    }

    free(list);
}

ListNode listFirst(List list){
    return list->first;
}









