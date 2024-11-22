#include "list.h"

typedef struct word_with_count* WordWithCount;


//διαβαζει απο το write end του pipe builder-root στο οποιο γραφει τα δεδομενα του ο καθε builder
int rootReadFromBuilder(int fd_read, WordWithCount* words, int size_of_array);

int compareWordStructs( const void* a, const void* b);
