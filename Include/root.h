#include "list.h"

typedef struct word_with_count* WordWithCount;


// reads from the write end of the builder-root pipe on which each builder writes its data
// and stores the data in an array which is returned
WordWithCount* rootReadFromBuilder(int fd_read, int* array_size);

// compares two word_with_count structs by the frequency of words
int compareWordStructs( const void* a, const void* b);

// prints the most popular words to the output file
void rootPrintToOutputFile(char* output, WordWithCount* words, int num_of_top_popular);
