#include "list.h"


//συγκρινει δυο λεξεις και επιστρεφει 0 αν ειναι ιδιες
int compareWords(Pointer a, Pointer b);

//διαβαζει το αρχειο και το χωριζει σε λεξεις που δεν περιεχουν συμβολα, σημεια στιξης κλπ
void splitterCreateWords(int fd, int end_line, int start_line, List exclusion_list, int num_of_builders);

//δημιουργει μια λιστα με τις λεξεις που πρεπει να αγνοηθουν
List splitterCreateExclusionList(char* exclusion_list);

int splitterHashFunc(char* word, int num_of_builders);

void splitterSendToBuilder(char* word, int num_of_builders);