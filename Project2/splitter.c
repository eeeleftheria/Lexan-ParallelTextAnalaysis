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
        char* message = "Failure opening input file in splitter\n";
        write(STDOUT_FILENO, message, strlen(message));
        exit(1);
    }

    // printf("\nIN SPLITTER %d %d\n", start_line, end_line);
    
    //πηγαινουμε τον δεικτη διαβασματος στη γραμμη start_line
    //απο την οποια πρεπει να διαβασει ο splitter
    lseek(fd, offset__of_start_line, SEEK_SET);
  
    //παραγωγη λεξεων, αγνοωντας σημεια στιξης, συμβολα κλπ
    splitterCreateWords(fd, end_line, start_line, exclusion_list, num_of_builders);


    //αφου τελειωσαμε με το γραψιμο στους builder, κλεινουμε ολα τα write ends
    for(int i = 0; i < num_of_builders; i++){
        close(i + 2000);
    }


    // printf("\nI AM SPLITTER %d %d AND FINISHED\n", start_line, end_line);

    pid_t root_pid = getppid(); //το process id του root
    // kill(root_pid, SIGUSR1); //ο splitter στελνει το σημα στον root οτι εχει τελειωσει με τη δουλεια του

    hashDestroy(exclusion_list); //καταστροφη του exclusion list
    close(fd);
    
    exit(1);
    
}

//############## ΣΥΝΑΡΤΗΣΕΙΣ ###############//

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



void splitterCreateWords(int fd, int end_line, int start_line, HashTable exclusion_list, int num_of_builders){

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
   
        //αν δεν χωραει αλλος χαρακτηρας στο word δεσμευουμε τον διπλασιο χωρο
        if(w_index >= word_size - 1){ //-1 γιατι ξεκιναει απο το 0
            word_size = 2 * word_size;
            word = realloc(word, word_size);
            
        }

        if(b_index >= buffer_size - 1){ //-1 γιατι ξεκιναει απο το 0
            buffer_size = 2 * buffer_size;
            buffer = realloc(buffer, buffer_size);
        }

        //αποθηκευση καθε χαρακτηρα στον buffer
        buffer[b_index] = c;

        if(isalpha(c)){ //αν ειναι αλφαβητικος χαρακτηρας μπορει να αποτελει μερος λεξης

            //μετατροπη των κεφαλαιων σε μικρα, ωστε οι λεξεις Hello και hello να ειναι ισοδυναμες
            if(c >= 'A' && c <= 'Z'){
                c = tolower(c);
            }

            word[w_index] = c;
            w_index++;
        }
        else if(c == '\n' || c == ' ' || c == '!' || c == ',' || c == '.' || c == '\t' || c == '"'){
            
            if(c == '\n'){
                lines++; //νεα γραμμη
            }
            
            //αν ο προηγουμενος ειναι αλφαβητικος, εχουμε λεξη
            if(isalpha( buffer[b_index - 1] )){
                word[w_index] = '\0';
                
                //αν δεν ανηκει στο exclusion list την στελνουμε στον builder
                if(hashFindListNodeWithKey(exclusion_list, word) == NULL){  
                    splitterSendToBuilder(word, num_of_builders);
                }
                else{
                    // printf("Excluded: %s\n", word);
                }

                //επαναφορα του word σε κενη λεξη
                for(int i = 0; i < w_index; i++){
                    word[i] = '\0';
                }
                w_index = 0; //επαναφορα του index στο 0, για ευρεση νεας λεξης
            }

            //αν διαβασαμε και την τελευταια γραμμη που πρεπει να διαβασει ο splitter
            if(lines > end_line){
                    break;
            }

        }
        //αν δεν ειναι αλφαβητικος χαρακτηρας ή αλλαγη γραμμης ή κενο
        else{
            //καταργουμε τη λεξη
            word[w_index] = '\0';
            w_index = 0;
        }
        b_index++;  //αυξηση index του buffer
    }

    if(bytes_to_read < 0) {
        perror("error reading from pipe in splitter\n");
    }
    
    //αν φτασαμε στο EOF
    if(bytes_to_read == 0){
       
        word[w_index] = '\0';

        if(hashFindListNodeWithKey(exclusion_list, word) == NULL){
        
            splitterSendToBuilder(word, num_of_builders);
        }
        else{
            // printf("Excluded: %s\n", word);
        }

    }
    free(word);
    free(buffer);
}


//συγκρινει δυο λεξεις
int compareWords(Pointer a, Pointer b){
    int res = strcmp((char*)a, (char*)b);
    return res;
}

//υπολογιζει τον builder που πρεπει να σταλθει η λεξη και την κανει write
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

    //ελεγχος αν ειναι ανοιχτος ο fd
    int flags = fcntl(builder + 2000, F_GETFD);
    if (flags == -1) {
        perror("Invalid file descriptor in splitter to write");
    }

    // printf("splitter sent to builder %d word: %s\n", builder, word);
    int bytes_written = write(builder + 2000, buffer, buffer_size);
    
    if (bytes_written == -1) {
        perror("Write to builder failed");
        exit(1);
    }

    // sleep(1);

    free(buffer);

}


//Δημιουργια μιας λιστας με τις λεξεις που περιεχονται στο αρχειο Exclusion List
//το αρχειο ειναι της μορφης: μια λεξη ανα γραμμη
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
        
        if(count >= word_size - 1){ //αν χρειαστει παραπανω χωρος για τη λεξη, δεσμευουμε τον διπλασιο
            word_size = 2 * word_size;
            word = realloc(word, word_size);
        }

        if(c != '\n' && (isalpha(c))){

            word[count] = c;
            count++;
        }
        
        else if(c == '\n' || c == EOF){  //αρα εχουμε λεξη
            word[count] = '\0';
            char* value = malloc(strlen(word) + 1); //δεσμευση χωρου για την τιμη που ισοδυναμει με τη λεξη
            char* key = malloc(strlen(word) + 1); //δεσμευση χωρου για τη λεξη
            
            strcpy(value, word);
            strcpy(key, word);


            hashAdd(table, key, value);

            for(int i = 0; i < count; i++){
                word[i] = '\0';
            }

            count = 0; //επαναφορα word index
        }

    }

    if(bytes_to_read < 0) {
            perror("error reading from exclusion list\n");
            close(fd);
            return NULL;
    }

    if(bytes_to_read == 0){
        word[count] = '\0';
        char* value = malloc(strlen(word) + 1);
        strcpy(value, word);

        hashAdd(table, value, value);
    }

    close(fd); //δεν χρειαζομαστε αλλο το αρχειο
    free(word);
    return table;

}

