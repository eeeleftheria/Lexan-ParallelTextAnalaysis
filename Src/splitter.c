#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include "splitter.h"
#include "hash.h"
#include <errno.h>
#include <signal.h>


int main(int argc, char* argv[]){

    char* input_file = NULL;
    HashTable exclusion_list = NULL;
    int start_line = 0;
    int end_line = 0;
    int num_of_builders = 0;
    long int offset__of_start_line = 0;

    if(argc != 7){
        char* message = "Error\nUsage is: ./splitter inputFile startLine endLine offset exclusionList numOfBuilders\n";
        write(STDOUT_FILENO, message, strlen(message));
        exit(1);
    }
    
    input_file = argv[1];
    start_line = atoi(argv[2]);
    end_line = atoi(argv[3]);
    offset__of_start_line = atol(argv[4]); // atol: string to long int
    exclusion_list = splitterCreateExclusionList(argv[5]);
    num_of_builders = atoi(argv[6]);

    // Open input file
    int fd = open(input_file, O_RDONLY);
    if(fd < 0){
        char* message = "Failure opening input file in splitter\n";
        write(STDOUT_FILENO, message, strlen(message));
        exit(1);
    }

    
    // move the read pointer to the start_line from which the splitter should read
    lseek(fd, offset__of_start_line, SEEK_SET);
  
    // generate words, ignoring punctuation, symbols, etc
    splitterCreateWords(fd, end_line, start_line, exclusion_list, num_of_builders);

    // after we finish writing to the builders, close all write ends
    for(int i = 0; i < num_of_builders; i++){
        close(i + 500);
    }


    pid_t root_pid = getppid(); // the process id of root
    kill(root_pid, SIGUSR1); // splitter sends signal to root that it has finished its work

    hashDestroy(exclusion_list); // destroy the exclusion list
    close(fd);
    
    exit(1);
     
}

// ############## FUNCTIONS ###############//

int splitterHashFunc(char* word, int num_of_builders){
    int key = 0;
    // since our words do not include numbers, we cannot do atoi
    // so we add the ASCII numbers of each character
    for(int i = 0; i < strlen(word); i++){
        key += word[i];
    }

    int pos = key % num_of_builders;
    return pos;
}



void splitterCreateWords(int fd, int end_line, int start_line, HashTable exclusion_list, int num_of_builders){

    int bytes_to_read;
    char c;
    int w_index = 0;
    int b_index = 0;
    int word_size = 10;
    int buffer_size = 50;
    char* buffer = malloc(buffer_size); // initially allocate 50 Bytes for the buffer
    char* word = malloc(word_size); // initially allocate 10 Bytes for a word
    int lines = start_line;

    while((bytes_to_read = read(fd, &c, sizeof(c))) > 0){
   
        // if another character doesn't fit in the word, allocate double the space
        if(w_index >= word_size - 1){ // -1 because it starts from 0
            word_size = 2 * word_size;
            word = realloc(word, word_size);
            
        }

        if(b_index >= buffer_size - 1){ // -1 because it starts from 0
            buffer_size = 2 * buffer_size;
            buffer = realloc(buffer, buffer_size);
        }

        // store each character in the buffer
        buffer[b_index] = c;

        if(isalpha(c)){ // if it is an alphabetic character, it can be part of a word

            // convert uppercase to lowercase, so words Hello and hello are equivalent
            if(c >= 'A' && c <= 'Z'){
                c = tolower(c);
            }

            word[w_index] = c;
            w_index++;
        }
        else if(c == '\n' || c == ' ' || c == '!' || c == ',' || c == '.' || c == '\t' || c == '"'){
            
            if(c == '\n'){
                lines++; // new line
            }
            
            // if the previous character is alphabetic, we have a word
            if(isalpha( buffer[b_index - 1] )){
                word[w_index] = '\0';
                
                // if it does not belong to the exclusion list, send it to the builder
                if(hashFindListNodeWithKey(exclusion_list, word) == NULL){
                    splitterSendToBuilder(word, num_of_builders);
                }
                else{
                    // printf("Excluded: %s\n", word);
                }

                // restore word to empty
                for(int i = 0; i < w_index; i++){
                    word[i] = '\0';
                }

                w_index = 0; // restore index to 0, to find next word
            }

            // if we read the last line that the splitter should read
            if(lines > end_line){
                    break;
            }

        }
        // if it is not an alphabetic character or line break or space
        else{
            // discard the word
            word[w_index] = '\0';
            w_index = 0;
        }

        b_index++;  // increment buffer index
    }

    if(bytes_to_read < 0) {
        perror("error reading from pipe in splitter\n");
    }
    
    // EOF
    if(bytes_to_read == 0){
       
        word[w_index] = '\0';

        if(hashFindListNodeWithKey(exclusion_list, word) == NULL){
        
            splitterSendToBuilder(word, num_of_builders);
        }
        else{
            
        }

    }
    free(word);
    free(buffer);
}


// compares two words
int compareWords(Pointer a, Pointer b){
    int res = strcmp((char*)a, (char*)b);
    return res;
}

// calculates the builder that the word should be sent to and writes it 
void splitterSendToBuilder(char* word, int num_of_builders){
    int builder = splitterHashFunc(word, num_of_builders);

    int size = strlen(word) + 1; //for \0
    int buffer_size = size + 1; //for " "
     
    char* buffer = malloc(buffer_size);
    if (buffer == NULL) {
        perror("Memory allocation failed");
        exit(1);
    }

    memcpy(buffer, word, size); 
    memcpy(buffer + size, " ", 1);

    // writes the words in format: word word word ...
    int bytes_written = write(builder + 500, buffer, buffer_size);
    
    if (bytes_written == -1) {
        perror("Write to builder failed");
        exit(1);
    }

    free(buffer);

}


// Creation of a list containing the words of Exclusion List
// the file is of format: one word per line
HashTable splitterCreateExclusionList(char* exclusion_list){
    
    int fd = open(exclusion_list, O_RDONLY);
    if(fd < 0){
        char* message = "Failure opening exclusion list\n";
        write(STDOUT_FILENO, message, strlen(message));
        exit(1);
    }

    HashTable table = hashCreate(51, compareWords);

    int bytes_to_read;
    char c;
    int word_size = 20;
    char* word = malloc(word_size);
    int count = 0; //index λεξης

    while((bytes_to_read = read(fd, &c, sizeof(c))) > 0){
        
        if(count >= word_size - 1){ // reserve double the space
            word_size = 2 * word_size;
            word = realloc(word, word_size);
        }

        if(c != '\n' && (isalpha(c))){

            word[count] = c;
            count++;
        }
        
        else if(c == '\n' || c == EOF){  // we have formed a word
            word[count] = '\0';
            char* value = malloc(strlen(word) + 1);
            char* key = malloc(strlen(word) + 1);
            
            strcpy(value, word);
            strcpy(key, word);

            hashAdd(table, key, value); // add word to hash table with same key and value

            for(int i = 0; i < count; i++){ // restore to empty word
                word[i] = '\0';
            }

            count = 0; // restore word index
        }

    }

    if(bytes_to_read < 0) {
            perror("error reading from exclusion list\n");
            close(fd);
            exit(1);
    }

    if(bytes_to_read == 0){
        word[count] = '\0';
        char* value = malloc(strlen(word) + 1);
        strcpy(value, word);

        hashAdd(table, value, value);
    }

    close(fd); // we don't need the file anymore
    free(word);
    return table;

}

