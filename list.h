////////////////////////////////////////////
// ΜΙΑ ΛΙΣΤΑ ΕΧΕΙ ΤΙΣ ΙΔΙΟΤΗΤΕΣ:
// 1) ΕΙΣΑΓΩΓΗ/ΔΙΑΓΡΑΦΗ ΟΠΟΥΔΗΠΟΤΕ
// 2) ΣΕΙΡΙΑΚΗ ΑΝΑΖΗΤΗΣΗ 
//
// για την υλοποιηση του γραφου εμας μας ενδιαφερει μονο η εισαγωγη στο ΤΕΛΟΣ της λιστας και η διαγραφη οπουδηποτε
//////////////////////////////////////////

#include <stdio.h>
#include <stdbool.h>

typedef void* Pointer; //μπορουμε να αποθηκευσουμε οτι τυπο θελουμε αλλα ΟΧΙ να τον χρησιμοποιησουμε(να ορισουμε περιεχομενο)


typedef struct list* List; //List δεικτης σε struct list
typedef struct list_node* ListNode;



List listCreate(); //δημιουργια κενης λιστας
void listInsert(List list, Pointer value); //δημιουργια και προσθηκη κομβου σε μια λιστα με τιμη value
void listRemove(List list, ListNode node); //αφαιρεση κομβου απο μια λιστα 
int listSize(List list); //επιστρεφει το μεγεθος της λιστας
Pointer listNodeValue(List list, ListNode node); //επιστρεφει το value του κομβου
ListNode listFirst(List list); //επιστρεφει το πρωτο στοιχειο της λιστας
bool listContainsValue(List list, Pointer value); //επιστρεφει true αν υπαρχει κομβος με αυτο το value 
ListNode listGetNext(ListNode node);

/////////////
void listDestroy(List list);
void listDestroyValue(List list, ListNode node);
ListNode findNode(List list, Pointer value); //επιστρεφει αν βρεθει τον κομβο με τιμη value


