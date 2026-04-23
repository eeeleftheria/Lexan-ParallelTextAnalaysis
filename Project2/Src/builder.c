#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/types.h>
#include "splitter.h"
#include "hash.h"
#include "builder.h"
#include <signal.h>

int main(int argc, char* argv[]){

    if(argc != 4){
        char* message = "Error\nUsage is: ./builder numOfBuilder fdReadEnd(pipe splitter-builder) fdWriteEnd(pipe builder-root)\n";
        write(STDOUT_FILENO, message, strlen(message));
        exit(1);
    }

    int fd_read_end = atoi(argv[2]);
    int fd_write_end_root = atoi(argv[3]);
 
    int size_buffer = 1024;
    char* buffer = malloc(size_buffer);
    int bytes_to_read = 0;
    int size = 0;

    // creates a data structure for storing words received from splitters
    HashTable table = hashCreate(1000, compareWords);

 
    char* word;

    while((bytes_to_read = read(fd_read_end, buffer, size_buffer)) > 0){
        
       for(int i = 0; i < bytes_to_read; i++){
          
            if(isalpha( buffer[i] )){
                size++;
            }
          
            else if(buffer[i] == ' '){
                word = malloc(size + 1); //+1 for \0
                memcpy(word, buffer + i - 1 - size, size); // i - 1 because we are at the space position
                word[size] = '\0';

                builderStoreInTable(table, word); // add word to hash table
               
              
                size = 0;
                free(word);
            } 
           
        }
      
    }
    if (bytes_to_read == 0) {

    } 

    else if (bytes_to_read < 0) {
        perror("Error reading from pipe");
    }

    free(buffer);
    
    close(fd_read_end);

    builderSendToRoot(table, compareHashNodes, fd_write_end_root); // write words to root

    hashDestroy(table);

    close(fd_write_end_root);
 
  
    pid_t root_pid = getppid(); // the process id of root
    kill(root_pid, SIGUSR2); // builder sends signal to root that it has finished its work


    exit(0);

}


int compareWords(Pointer a, Pointer b){
    int res = strcmp((char*)a, (char*)b);
    return res;
}


//συγκρινει δυο κομβους του HashTable με βαση το κλειδι τους
int compareHashNodes(Pointer a, Pointer b){
    HashNode node1 = (HashNode)a;
    HashNode node2 = (HashNode)b;

    Pointer key1 = hashGetKey(node1);
    Pointer key2 = hashGetKey(node2);

    return compareWords(key1, key2);

}


//αποθηκευει μια λεξη που ελαβε απο splitter σε μια δομη HashTable
void builderStoreInTable(HashTable table, char* word){
        
    //αν υπαρχει ηδη η λεξη αποθηκευμενη, αρκει να αυξησουμε τον μετρητη της
    if(hashFindListNodeWithKey(table, word) != NULL){
        
        int* value = hashFindValue(table, word);
        
        if(value != NULL){
            (*value)++; // increment the occurrences of the word, since it already exists in the structure
        }

        else{
            printf("count is null\n");
        }

    }
    // if the word does not exist, we add it
    else{

        char* key = malloc(strlen(word) + 1); // allocate space for the word
        strcpy(key, word);
        
        int* count = malloc(sizeof(int)); // allocate space for the counter of word occurrences
        *count = 1;

     
        hashAdd(table, key, count); // add to structure with counter value
    }
}


// after storing all words in the structure, sends them to root
void builderSendToRoot(HashTable table, CompareFunc compare, int fd_root_write){

    int size_of_table = hashGetSizeOfArray(table);
    
    for(int i = 0; i < size_of_table; i++){
        
        // if some list has no nodes, move to the next one
        if(hashGetSizeOfList(table, i) == 0){
            continue;
        }

        // traverse the hash table to write each word it contains to the pipe
        for(HashNode node = hashGetFirst(table, i); node != NULL; node = hashGetNext(table, i, node, compare)){
       
            char word[strlen(hashGetKey(node))]; // allocate space for the word
            strcpy(word, hashGetKey(node));

            char count[10];  // allocate space for the count
            snprintf(count, sizeof(count), "%d", *(int*)(hashGetValue(node))); // convert to string and store in count variable
                    
            int size = strlen(word) + 1 + strlen(count) + 1 + 1;
            char buffer[size];
           
            memcpy(buffer, word, strlen(word));
            memcpy(buffer + strlen(word), ":", 1);
            memcpy(buffer + strlen(word) + 1, count, strlen(count));
            memcpy(buffer + strlen(word) + strlen(count) + 1, "-", 1); 

            buffer[size - 1] = '\0';

            int bytes_written = 0;
            // write in the form word:count-word:count-word:count ....
            bytes_written = write(fd_root_write, buffer, sizeof(buffer));
            if(bytes_written == -1){
                 perror("Write from builder to root failed");
            }         
        }
    }
}