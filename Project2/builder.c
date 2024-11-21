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

    // printf("\n\nread fd of builder: %d\n", fd_read_end);
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
        // printf("End of input in builder\n");
    } 
    else if (bytes_to_read < 0) {
        perror("Error reading from pipe");
    }

    // hashDisplay(table);

    builderSendToRoot(table, fd_write_end_root, compareHashNodes, fd_write_end_root);

    hashDestroy(table);

    printf("\nBUILDER FINISHED\n");

    close(fd_read_end);
    close(fd_write_end_root);

    exit(1);

}


int compareWords(Pointer a, Pointer b){
    int res = strcmp((char*)a, (char*)b);
    return res;
}

int compareHashNodes(Pointer a, Pointer b){
    HashNode node1 = (HashNode)a;
    HashNode node2 = (HashNode)b;

    Pointer key1 = hashGetKey(node1);
    Pointer key2 = hashGetKey(node2);

    return compareWords(key1, key2);

}


void builderStoreInTable(HashTable table, char* word){
        
    //αν υπαρχει ηδη η λεξη αποθηκευμενη, αρκει να αυξησουμε τον μετρητη της
    if(hashFindListNodeWithKey(table, word) != NULL){
        
        int* value = hashFindValue(table, word);
        
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

        char* key = malloc(strlen(word) + 1); //δεσμευση χωρου για τη λεξη
        strcpy(key, word);
        
        int* count = malloc(sizeof(int)); // δεσμευση χωρου για τον μετρητη εμφανισεων της λεξης
        *count = 1;

     
        hashAdd(table, key, count); //προσθηκη στη δομη με τιμη τον μετρητη
     
        // printf("ADDED word %s with count %d\n", key, count);

    }

}


void builderSendToRoot(HashTable table, int fd, CompareFunc compare, int fd_root_write){
    int size_of_table = hashGetSizeOfArray(table);
    
    for(int i = 0; i < size_of_table; i++){
        
        // printf("\nbucket %d\n", i);
        if(hashGetSizeOfList(table, i) == 0){
            continue;
        }


        for(HashNode node = hashGetFirst(table, i); node != NULL; node = hashGetNext(table, i, node, compare)){
            // printf("node %s: %d times ", (char*)hashGetKey(node), *(int*)(hashGetValue(node)));
         

            char word[strlen(hashGetKey(node))]; //δεσμευση χωρου για τη λεξη
            strcpy(word, hashGetKey(node));

            char count[10];  //δεσμευση χωρου για το count
            snprintf(count, sizeof(count), "%d", *(int*)(hashGetValue(node))); //μετατροπη σε string κ αποθηκευση στη μεταβητη count
            
        
            int size = strlen(word) + 1 + strlen(count) + 1 + 1;
            char buffer[size];
           
            memcpy(buffer, word, strlen(word) + 1);
            memcpy(buffer + strlen(word), ":", 1);
            memcpy(buffer + strlen(word) + 1, count, sizeof(count));
            memcpy(buffer + strlen(word) + sizeof(count) + 1, " ", 1); 

            //ενα write της μορφης word:count word:count word:count ....
            write(fd_root_write, buffer, sizeof(buffer));
            // printf("builder sent to root: %s\n", buffer);

          
        }

    }
}