#include "list.h"

typedef struct word_with_count* WordWithCount;


//διαβαζει απο το write end του pipe builder-root στο οποιο γραφει τα δεδομενα του ο καθε builder
WordWithCount* rootReadFromBuilder(int fd_read, int* array_size);

int compareWordStructs( const void* a, const void* b);

void rootPrintToOutputFile(char* output, WordWithCount* words, int size_of_array, int num_of_top_popular);
