////////////////////////////////////////////
// ΜΙΑ ΛΙΣΤΑ ΕΧΕΙ ΤΙΣ ΙΔΙΟΤΗΤΕΣ:
// 1) ΕΙΣΑΓΩΓΗ/ΔΙΑΓΡΑΦΗ ΟΠΟΥΔΗΠΟΤΕ
// 2) ΣΕΙΡΙΑΚΗ ΑΝΑΖΗΤΗΣΗ 
//
// για την υλοποιηση του γραφου εμας μας ενδιαφερει μονο η εισαγωγη στο ΤΕΛΟΣ της λιστας και η διαγραφη οπουδηποτε
//////////////////////////////////////////

#include <stdio.h>

typedef void* Pointer; //μπορουμε να αποθηκευσουμε οτι τυπο θελουμε αλλα ΟΧΙ να τον χρησιμοποιησουμε(να ορισουμε περιεχομενο)


typedef struct list* List; //List δεικτης σε struct list
typedef struct list_node* ListNode;



List listCreate(); //δημιουργια κενης λιστας
void listInsert(List list, ListNode node); //προσθηκη κομβου σε μια λιστα
void listRemove(List list, ListNode node); //αφαιρεση κομβου απο μια λιστα 
ListNode listCreateNode(Pointer value);
int listSize(List list);
Pointer listNodeValue(List list, ListNode node);
ListNode listFirst(List list);


/////////////
void listDestroy(List list);
void listDestroyValue(List list, ListNode node);
ListNode findNode(List list, Pointer value);


