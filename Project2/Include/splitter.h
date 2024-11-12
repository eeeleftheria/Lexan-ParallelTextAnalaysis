#include "list.h"


//συγκρινει δυο λεξεις και επιστρεφει 0 αν ειναι ιδιες
int compareWords(Pointer a, Pointer b);

//διαβαζει το αρχειο και το χωριζει σε λεξεις που δεν περιεχουν συμβολα, σημεια στιξης κλπ
void splitterCreateWords(int fd, int end_line, int start_line, List exclusion_list, int num_of_builders);

//δημιουργει μια λιστα με τις λεξεις που πρεπει να αγνοηθουν
List splitterCreateExclusionList(char* exclusion_list);

//υπολογιζει τον builder στον οποιο πρεπει να σταλθει η λεξη
int splitterHashFunc(char* word, int num_of_builders);

//υπολογιζει τον builder που πρεπει να σταλθει η λεξη και την κανει write στο pipe
void splitterSendToBuilder(char* word, int num_of_builders);