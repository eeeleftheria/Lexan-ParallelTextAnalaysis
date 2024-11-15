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

int main(int argc, char* argv[]){

    if(argc != 4){
        char* message = "Error\nUsage is: ./builder numOfBuilder fdReadEnd(pipe splitter-builder) fdWriteEnd(pipe builder-root)\n";
        write(STDOUT_FILENO, message, strlen(message));
        exit(1);
    }

    int num_of_builder = atoi(argv[1]);
    int fd_read_end = atoi(argv[2]);
    int fd_write_end_root = atoi(argv[3]);
 

    int size_buffer = 1024;
    char* buffer = malloc(size_buffer);
    int bytes_to_read;
    int size = 0;

    HashTable table = hashCreate(101, compareWords);

    printf("\n\nread fd of builder: %d\n", fd_read_end);
    char* word;

    while((bytes_to_read = read(fd_read_end, buffer, size_buffer)) > 0){
       
    //    printf("builder received bytes: %d\n", bytes_to_read);
       
       for(int i = 0; i < bytes_to_read; i++){
          

            if(isalpha( buffer[i] )){
                size++;
            }
            else if(buffer[i] == ' '){
                word = malloc(size + 1); //+1 for \0
                memcpy(word, buffer + i - 1 - size, size);
                word[size] = '\0';

                // printf("builder received word: %s\n", word);
                builderStoreInTable(table, word); //προσθηκη λεξης στο hash table
               
              
                size = 0;
                free(word);
            } 
           
        }
      
    }
    if (bytes_to_read == 0) {
    printf("End of input in builder\n");
    } 
    else if (bytes_to_read < 0) {
        perror("Error reading from pipe");
    }

    // hashDisplay(table);




    close(fd_read_end);

    exit(1);

}


int compareWords(Pointer a, Pointer b){
    int res = strcmp((char*)a, (char*)b);
    return res;
}


void builderStoreInTable(HashTable table, char* word){
    
    char* key = malloc(strlen(word) + 1); //δεσμευση χωρου για τη λεξη
    strcpy(key, word);
    
    
    //αν υπαρχει ηδη η λεξη αποθηκευμενη, αρκει να αυξησουμε τον μετρητη της
    if(hashFindListNodeWithKey(table, word) != NULL){
        
        int* value = hashFindValue(table, key);
        
        if(value != NULL){
            (*value)++; //αυξηση των εμφανισεων της λεξης, αφου υπαρχει ηδη στη δομη
        }

        else{
            printf("count is null\n");
        }
        
        // printf("found word %s with count %d\n", word, *value);

    }
    //αν δεν υπαρχει η λεξη, τη προσθετουμε
    else{

        int* count = malloc(sizeof(int)); // δεσμευση χωρου για τον μετρητη εμφανισεων της λεξης
        *count = 1;
     
        hashAdd(table, key, count); //προσθηκη στη δομη με τιμη τον μετρητη
     
        // printf("ADDED word %s with count %d\n", key, count);

    }

}


void builderSendToRoot(HashTable table, int fd){
    
}