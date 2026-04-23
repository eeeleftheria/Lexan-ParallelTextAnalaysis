#include "list.h"
#include "hash.h"


// compares two words and returns 0 if they are identical
int compareWords(Pointer a, Pointer b);

// reads the file and splits it into words that do not contain symbols, punctuation, etc
void splitterCreateWords(int fd, int end_line, int start_line, HashTable exclusion_list, int num_of_builders);

// creates a list with words that should be ignored
HashTable splitterCreateExclusionList(char* exclusion_list);

// calculates the builder to which the word should be sent
int splitterHashFunc(char* word, int num_of_builders);

// calculates the builder to which the word should be sent and writes it to the pipe
void splitterSendToBuilder(char* word, int num_of_builders);