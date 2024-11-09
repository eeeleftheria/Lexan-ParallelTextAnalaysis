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
    int num_of_builders = 0;
    long int offset__of_start_line = 0;

    if(argc != 7){
        char* message = "Error\nUsage is: ./splitter inputFile startLine endLine offset exclusionList numOfBuilders\n";
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
       if(i == 6){
           num_of_builders = atoi(argv[i]);
       }
    }

    //Ανοιγμα input file
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
    splitterCreateWords(fd, end_line, start_line, exclusion_list, num_of_builders);


    for(int i = 0; i < num_of_builders; i++){
        close(i + 25);
    }

    printf("\nI AM SPLITTER %d %d AND FINISHED\n", start_line, end_line);

    close(fd);
    
    exit(1);
    
}


int splitterHashFunc(char* word, int num_of_builders){
    int key = 0;
    //καθως οι λεξεις μας δεν περιλαμβανουν αριθμους, δεν μπορουμε να κανουμε atoi
    //οποτε προσθετουμε τους αριθμους ascii του καθε χαρακτηρα
    for(int i = 0; i < strlen(word); i++){
        key += word[i];
    }

    int pos = key % num_of_builders;
    return pos;
}


void splitterCreateWords(int fd, int end_line, int start_line, List exclusion_list, int num_of_builders){

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

        //αποθηκευση καθε χαρακτρα στον buffer
        buffer[b_index] = c;

        if(isalpha(c)){ //αν ειναι αλφαβητικος χαρακτηρας μπορει να αποτελει μερος λεξης
            word[w_index] = c;
            w_index++;
        }
        else if(c == '\n' || c == ' '){
            
            if(c == '\n'){
                lines++;
            }
            
            //αν ο προηγουμενος ειναι αλφαβητικος, εχουμε λεξη
            if(isalpha( buffer[b_index - 1] )){
                word[w_index] = '\0';
                w_index = 0;
                
                //ελεγχος αν ανηκει στο exclusion list
                if(listfindNodeWithValue(exclusion_list, word, compareWords) == NULL){  
                    splitterSendToBuilder(word, num_of_builders);
                }
                else{
                    // printf("Excluded: %s\n", word);
                }

                //επαναφορα του word σε κενη λεξη
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
        
            splitterSendToBuilder(word, num_of_builders);
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


void splitterSendToBuilder(char* word, int num_of_builders){
    int builder = splitterHashFunc(word, num_of_builders);
    int size = strlen(word) + 1; //for \0
    int buffer_size = sizeof(int) + size;
    
    char* buffer = malloc(buffer_size);
    if (buffer == NULL) {
        perror("Memory allocation failed");
        exit(1);
    }

    memcpy(buffer, &size, sizeof(int));  //σαν header για να ξερει ο builder ποσο πρεπει να διαβασει
    memcpy(buffer + sizeof(int), word, size); //αντιγραφεται στη θεση μνημης μετα το header
    
    write(4, buffer, buffer_size);
    printf("splitter sent to builder %d word: %s\n", builder, buffer + sizeof(int));
    // sleep(1);

    free(buffer);

}
