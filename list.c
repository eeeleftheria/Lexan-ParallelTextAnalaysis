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


//προσθετουμε εναν κομβο στο τελος της λιστας, δεν μας ενδιαφερει να τον προσθεσουμε σε συγκεκριμενο σημειο
void listInsert(List list, Pointer value) {

    ListNode new_node = malloc(sizeof(struct list_node));
    new_node->value = value;

    if(new_node == NULL) {
        return;
    }
    
    if(list->size == 0) { //κενη λιστα
        list->first = new_node;
        list->last = new_node;
        new_node->next = NULL;
        new_node->prev = NULL;
    }

    else { //μη κενη λιστα: προσθετουμε στο τελος και συνδεουμε με τον προηγουμενο κομβο
    //ο prev του node θα ειναι ο παλιος last
        list->last->next = new_node;
        new_node->prev = list->last;
        list->last = new_node;
        new_node->next = NULL;
    }


    list->size++;

}

void listRemove(List list, ListNode node, DestroyValueFunc func){

    ListNode prev = node->prev;
    ListNode next = node->next;

    //αν ο node ειναι ο πρωτος της λιστας πρεπει ο επομενος του να γινει πρωτος
    if (node == list->first) {
        list->first = next;
        next->prev = NULL;
    }
    //αν ο node ειναι ο τελευταιος της λιστας πρεπει ο προηγουμενος του να γινει τελευταιος
    else if (node == list->last){
        list->last = prev;
        prev->next = NULL;
    }

    //αν ο node βρισκεται ενδιαμεσα, πρεπει να συνδεσουμε τον προηγουμενο του με τον επομενο του
    else{
        prev->next = next;
        next->prev = prev;
    }

    func(node->value);

    free(node);
    node = NULL;
    list->size--;

}






int listSize(List list){
    return list->size;
}


Pointer listNodeValue(List list, ListNode node){
    return node->value;
}


//τυπου DestroyFunc
void listDestroyValue(List list, ListNode node, DestroyValueFunc func){
    
    func(node->value); //καταστρεφει μεκαταλληλο τροπο το καθε value
    //για παραδειγμα αν εχουμε ως value graph nodes πρεπει να αποδεσμευει οτι στοιχειο εχει μεσα
    //το graph node που πιανει χωρο στη μνημη και μετα να κανει free
   
    }

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

    free(list);
}

ListNode listFirst(List list){
    return list->first;
}


ListNode listGetNext(ListNode node){
    return node->next;

}





ListNode findNodeWithValue(List list, Pointer value){
    ListNode node;
    
    for(node = listFirst(list); node != NULL; node = listGetNext(node)){
        
        if (listNodeValue(list, node) == value) {
            return node;
        }    
    }

}









