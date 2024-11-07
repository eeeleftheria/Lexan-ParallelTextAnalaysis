#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include "splitter.h"


int main(int argc, char* argv[]){

    char* input_file = NULL;
    List exclusion_list = NULL;
    int start_line = 0;
    int end_line = 0;
    long int offset__of_start_line = 0;

    if(argc != 6){
        char* message = "Error\nUsage is: ./splitter inputFile startLine endLine offset exclusionList\n";
        write(STDOUT_FILENO, message, strlen(message));
        exit(1);
    }
    
    for(int i = 0; i < argc; i++){
        
       if(i == 1){
           input_file = argv[i];
       }
       if(i == 2){
           start_line = atoi(argv[i]);
       }
       if(i == 3){
           end_line = atoi(argv[i]);
       }
       if(i == 4){
            offset__of_start_line = atol(argv[i]); //atol: string to long int
       }
       if(i == 5){
           exclusion_list = splitterCreateExclusionList(argv[i]);
       }
    }


    int fd = open(input_file, O_RDONLY);
    if(fd < 0){
        char* message = "Failure opening input file\n";
        write(STDOUT_FILENO, message, strlen(message));
        exit(1);
    }

    printf("\nIN SPLITTER %d %d\n", start_line, end_line);
    
    //πηγαινουμε τον δεικτη διαβασματος στη γραμμη start_line
    //απο την οποια πρεπει να διαβασει ο splitter
    lseek(fd, offset__of_start_line, SEEK_SET);

    //παραγωγη λεξεων, αγνοωντας σημεια στιξης, συμβολα κλπ
    splitterCreateWords(fd, end_line, start_line, exclusion_list);

    printf("\nI AM SPLITTER %d %d AND FINISHED\n", start_line, end_line);

    close(fd);
    
    exit(1);
    
}


void splitterCreateWords(int fd, int end_line, int start_line, List exclusion_list){

    int bytes_to_read;
    char c;
    int w_index = 0;
    int b_index = 0;
    int word_size = 10;
    int buffer_size = 50;
    char* buffer = malloc(buffer_size); //αρχικα δεσμεουμε 50 Bytes για το buffer
    char* word = malloc(word_size); //αρχικα δεσμεουμε 10 Bytes για μια λεξη
    int lines = start_line;

    while((bytes_to_read = read(fd, &c, sizeof(c))) > 0){

        if(bytes_to_read < 0) {
            perror("error reading from pipe in splitter\n");
        }

        //αν δεν χωραει αλλος χαρακτηρας στο word δεσμευουμε τον διπλασιο χωρο
        if(w_index >= word_size - 1){ //-1 γιατι ξεκιναει απο το 0
            word_size = 2 * word_size;
            word = realloc(word, word_size);
        }

        if(b_index >= buffer_size - 1){ //-1 γιατι ξεκιναει απο το 0
            buffer_size = 2 * buffer_size;
            buffer = realloc(buffer, buffer_size);
        }

        buffer[b_index] = c;

        if(isalpha(c)){
            word[w_index] = c;
            w_index++;
        }
        else if(c == '\n' || c == ' '){
            
            if(c == '\n'){
                lines++;
            }
            
            if(isalpha( buffer[b_index - 1] )){
                word[w_index] = '\0';
                w_index = 0;
                
                if(listfindNodeWithValue(exclusion_list, word, compareWords) == NULL){  
                    printf("%s\n", word);
                }
                else{
                    // printf("Excluded: %s\n", word);
                }

                for(int i = 0; i < w_index; i++){
                    word[i] = '\0';
                }
            }

            if(lines == end_line + 1){
                    break;
            }

        }
   
        else{
            word[w_index] = '\0';
            w_index = 0;
        }
        b_index++;       
    }

    if(bytes_to_read == 0){
        word[w_index] = '\0';

        if(listfindNodeWithValue(exclusion_list, word, compareWords) == NULL){
            printf("%s\n", word);
        }
        else{
            // printf("Excluded: %s\n", word);
        }

    }
}



List splitterCreateExclusionList(char* exclusion_list){
    int fd = open(exclusion_list, O_RDONLY);
    if(fd < 0){
        char* message = "Failure opening exclusion list\n";
        write(STDOUT_FILENO, message, strlen(message));
        exit(1);
    }

    List list = listCreate();

    int bytes_to_read;
    char c;
    int word_size = 20;
    char* word = malloc(word_size);
    int count = 0;

    while((bytes_to_read = read(fd, &c, sizeof(c))) > 0){
        if(bytes_to_read < 0) {
            perror("error reading from exclusion list\n");
            close(fd);
            return NULL;
        }

        if(count >= word_size - 1){
            word_size = 2 * word_size;
            word = realloc(word, word_size);
        }

        if(c != '\n'){
            word[count] = c;
            count++;
        }
        
        else if(c == '\n'){
            word[count] = '\0';
            char* value = malloc(strlen(word) + 1);
            strcpy(value, word);
            listInsert(list, value);
            count = 0;
        }

    }
    if(bytes_to_read == 0){
        word[count] = '\0';
        char* value = malloc(strlen(word) + 1);
        strcpy(value, word);
        listInsert(list, value);
    }

    close(fd); //δεν χρειαζομαστε αλλο το αρχειο
    free(word);
    return list;

}

int compareWords(Pointer a, Pointer b){
    return strcmp((char*)a, (char*)b);
}