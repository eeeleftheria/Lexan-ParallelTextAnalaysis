#include "list.h"

typedef struct word_with_count* WordWithCount;


//διαβαζει απο το write end του pipe builder-root στο οποιο γραφει τα δεδομενα του ο καθε builder
//και αποθηκευει τα δεδομενα σε εναν πινακα τον οποιο και επιστρεφει
WordWithCount* rootReadFromBuilder(int fd_read, int* array_size);

//συγκρινει δυο struct τυπου  word_with_count με την συχνοτητα των λεξεων
int compareWordStructs( const void* a, const void* b);

//εκτυπωνει τις πιο δημοφιλεις λεξεις στο αρχειο output
void rootPrintToOutputFile(char* output, WordWithCount* words, int num_of_top_popular);
