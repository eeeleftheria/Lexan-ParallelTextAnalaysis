//############################# SYSTEM CALLS ########################//
// exec*(), mkfifo(), pipe(), open(), close(), read(), write(),      //
// wait(), waitpid(), poll(), select(), dup(), dup2(), kill(), exit()//
//###################################################################//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>
#include <root.h>


//handler of signal USR1 when a splitter finishes its job.
void splitterIsDone(int signum){
        signal(SIGUSR1, splitterIsDone);
        // printf("splitter is done\n");
}

struct word_with_count{
    char* word;
    int count;
};


int main(int argc, char* argv[]){  
    
    char* input_file = NULL;
    char* output_file = NULL;
    char* exclusion_list_file = NULL;
    int num_of_splitter, num_of_builders, num_of_top_popular;
    
    if(argc != 13){
        char* message = "Error\nUsage is: ./lexan -i TextFile -l numOfSplitter -m numOfBuilders -t TopPopular -e ExclusionList -o OutputFile\n";
        write(STDOUT_FILENO, message, strlen(message));
        exit(1);
    }

    for(int i = 0; i < argc; i++){
        
        if(strcmp(argv[i], "-i") == 0){
            input_file = argv[i+1];
        }
        if(strcmp(argv[i], "-o") == 0){
            output_file = argv[i+1];
        }
        if(strcmp(argv[i], "-l") == 0){
            num_of_splitter = atoi(argv[i+1]);
        }
        if(strcmp(argv[i], "-m") == 0){
            num_of_builders = atoi(argv[i+1]);
        }
        if(strcmp(argv[i], "-t") == 0){
            num_of_top_popular = atoi(argv[i+1]);
        }
        if(strcmp(argv[i], "-e") == 0){
            exclusion_list_file = argv[i+1];
        }
    }
    
    //ανοιγμα αρχειου για να μετρησουμε τις γραμμες
    int fd = open(input_file, O_RDONLY);
    if(fd < 0){
        char* message = "Failure opening input file\n";
        write(STDERR_FILENO, message, strlen(message));
    }

    //################################################ ΥΠΟΛΟΓΙΣΜΟΣ ΓΡΑΜΜΩΝ ΑΡΧΕΙΟΥ #################################################//
    int lines = 1;//κραταμε τον αριθμο γραμμων του αρχειου
    char c = 0;

    //διαβασμα αρχειου ανα χαρακτηρα
    int bytes_to_read;
    while((bytes_to_read =  read(fd, &c, sizeof(c))) > 0) {  //για EOF επιστρεφει 0
        if(c == '\n'){
            lines++;
        }
    }
    if (bytes_to_read == 0) {
        perror("End of input in root\n");
    } 
    else if (bytes_to_read < 0) {
        perror("Error reading from input file\n");
    }

    lseek(fd, 0, SEEK_SET); //επαναφορα δεικτη στην αρχη του αρχειου
    

    //########################################## ΥΠΟΛΟΓΙΣΜΟΣ OFFSET ΤΗΣ ΚΑΘΕ ΓΡΑΜΜΗΣ ###############################################//
    
    //πινακας με τα offset των γραμμων
    long int* offset_of_line = malloc((lines + 1) * sizeof(long int)); //+1 γιατι δεν μετραμε την θεση 0 ως γραμμη
    int count = 1; //μετρητης γραμμων
    int bytes = 0; //ποσα bytes εχουμε διαβασει μεχρι ενα σημειο
    offset_of_line[0] = 0;
    
    while(read(fd, &c, sizeof(c)) > 0) {  
        bytes += sizeof(c);
        
        if(c == '\n'){  //νεα γραμμη
            offset_of_line[count] = offset_of_line[count-1] + bytes;
            count++;
            bytes = 0;
        }
        //αν ειναι η τελευταια γραμμη μπορει να μην τελειωνει με \n
        if(count == lines){
            offset_of_line[count] = offset_of_line[count-1] + bytes;
        }
            
    }

    close(fd); //κλεισιμο του αρχειου, θα το ξαναανοιξουμε μεσω των pipes


    //################################# ΔΗΜΙΟΥΡΓΙΑ PIPES ΓΙΑ ΕΠΙΚΟΙΝΩΝΙΑ SPLITTERS - BUILDERS #############################################//

    //θελουμε ενα pipe για καθε builder για επικοινωνια splitters-builders, 
    //δηλ num_of_builder pipes με 2 θεσεις το καθενα για τον pipefd
    int pipes_builder[num_of_builders][2]; //pipe[i][0] = read end of pipe i & pipe[i][1] = write end of pipe i

    for(int i = 0; i < num_of_builders; i++){
            
        if(pipe(pipes_builder[i]) == -1){
            perror("error with creation of pipe builder\n");
            return -1;
        }
    }


    int input_of_splitter = lines / num_of_splitter; //γραμμες ανα splitter
    pid_t splitter[num_of_splitter]; //πινακας με τα pid του καθε splitter

    
    //########################################## ΔΗΜΙΟΥΡΓΙΑ SPLITTERS ############################################//
      
    signal(SIGUSR1, splitterIsDone);


    for(int i = 0; i < num_of_splitter; i++){
    
        pid_t splitter_pid = fork(); //δημιουργια splitter processes
        
        if(splitter_pid == -1){
            perror("error with fork splitter\n");
            return -1;
        }
        
        if(splitter_pid != 0){ //εντος parent process
            
            splitter[i] = splitter_pid; //αποθηκευση του pid του splitter i   
          
        }

        if(splitter_pid == 0){ //εντος child process

            //οι splitters πρεπει μονο να γραφουν στα pipes
            for(int j = 0; j < num_of_builders; j++){
                close(pipes_builder[j][0]); //κλεισιμο του read end
                dup2(pipes_builder[j][1], j + 2000); //ανακατευθυνση του write end, ωστε να εχουν προσβαση σε αυτο μετα την exec
                close(pipes_builder[j][1]); //κλεινουμε το original write end
            }
            

            //για τον υπολογισμο του ευρους γραμμων καθε splitter
            int start_line = 0;
            int end_line = 0;

            //αν ειναι ο 1ος splitter διαβαζει απο την αρχη του αρχειου μεχρι το input_of_splitter
            if(i == 0){
                start_line = 1;
                end_line = input_of_splitter;
            }
            //διαβαζει απο εκει που σταματησε ο προηγουμενος
            else if((i != 0) && (i != num_of_splitter - 1)){
                start_line = i * input_of_splitter + 1;
                end_line = start_line + input_of_splitter - 1; //αν πχ start_line = 5 και input = 5, πρεπει να διαβασει μεχρι και τη γραμμη 9
            }
            //αν ειναι ο τελευταιος splitter διαβαζει μεχρι το τελος του αρχειου
            //γιατι αν δεν διαιρειται ακριβως, θα περισσεψουν γραμμες. 
            else if(i == num_of_splitter - 1){
                start_line = i * input_of_splitter + 1;
                end_line = lines;
            }   


            long int offset_start_line = offset_of_line[start_line - 1]; //τα Bytes μεχρι και πριν τη γραμμη start_line

            char offset_start_line_str[10]; 
            char start_line_str[10];
            char end_line_str[10]; 
            char num_of_builders_str[10];
            
            //μετατροπη των ακεραιων σε string για την exec
            snprintf(start_line_str, sizeof(start_line_str), "%d", start_line);
            snprintf(end_line_str, sizeof(end_line_str), "%d", end_line);  
            snprintf(offset_start_line_str, sizeof(offset_start_line_str), "%ld", offset_start_line);
            snprintf(num_of_builders_str, sizeof(num_of_builders_str), "%d", num_of_builders);

            //εκτελεση του splitter που βρισκεται στο ιδιο directory
            execlp("./splitter", "splitter", input_file, start_line_str, end_line_str, 
                offset_start_line_str, exclusion_list_file, num_of_builders_str, NULL); 
		    perror("exec failure\n");  //δεν θα φτασει ποτε εδω εαν ειναι επιτυχης η exec     
            exit(1);         

        }
    }   

    



    //#################################################### ΔΗΜΙΟΥΡΓΙΑ BUILDERS ######################################################//

    //Δημιουργια ενος pipe για επικοινωνια builders με ριζα
    int fd_root[2]; //fd_pipe[0] read end, fd_pipe[1] write end
    if(pipe(fd_root) == -1){
        perror("error with creation of pipe builder\n");
        return -1;
    }

    pid_t builder[num_of_builders];
    
    for(int i = 0; i < num_of_builders; i++){

            pid_t builder_pid = fork(); //δημιουργια builder processes

            if(builder_pid == -1){
                perror("error with fork builder\n");
                return -1;
            }
            if(builder_pid != 0){ //εντος parent process

                builder[i] = builder_pid; //αποθηκευση του pid του παιδιου i
                close(fd_root[1]); //o root πρεπει μονο να διαβαζει απο τους builders
            }

            if(builder_pid == 0){ //εντος child process

                close(fd_root[0]); //μονο γραψιμο στον root
                
                //θελουμε να κλεισουμε ολα τα read end των pipes 
                //που δεν απασχολουν τον συγκεκριμενο builder καθως και ολα τα write end
                for(int j = 0; j < num_of_builders; j++){
                    if(j != i){
                        close(pipes_builder[j][0]);
                    }
                    close(pipes_builder[j][1]);
                }

                int fd_read = pipes_builder[i][0]; //το read end fd του pipe
                int fd_root_write_end = fd_root[1]; 

                char fd_read_str[10];
                char num_of_builder_str[10];
                char fd_root_write_str[10];

                snprintf(fd_read_str, sizeof(fd_read_str), "%d", fd_read);
                snprintf(num_of_builder_str, sizeof(num_of_builder_str), "%d", i);
                snprintf(fd_root_write_str, sizeof(fd_root_write_str), "%d", fd_root_write_end);


                //εκτελεση του builder που βρισκεται στο ιδιο directory
                execlp("./builder", "builder", num_of_builder_str, fd_read_str, fd_root_write_str, NULL);
                perror("exec failure\n"); //εαν αποτυχει
                exit(1);
            } 
  
    }  


    //######################################################################################################################


    //κλεισιμο ολων των ακρων των pipes builders - splitters
    for(int i = 0; i < num_of_builders; i ++){
        close(pipes_builder[i][0]);
        close(pipes_builder[i][1]);
    }


    //ο parent πρεπει να περιμενει 
    for(int i = 0; i < num_of_splitter; i++){
        int status;
        if (waitpid(splitter[i], &status, 0) == -1) {
            perror("error with waitpid splitter\n");
        }
    }


  
    //########################################## ΔΙΑΒΑΣΜΑ ΑΠΟΤΕΛΕΣΜΑΤΩΝ ΑΠΟ BUILDERS ##########################################################//

    close(fd_root[1]); //ο root πρεπει μονο να διαβαζει απο το pipe root - builders
    
    //δημιουργια ενος απλου πινακα που εχει δεικτες σε struct word_with_count
    int size_of_array = 1000; //αρχικο μεγεθος πινακα
    WordWithCount* words = malloc(size_of_array * sizeof(struct word_with_count)); //πινακας με τις λεξεις και τις συχνοτητες τους

    //εαν αλλαξει εσωτερικα στη συναρτηση το μεγεθος του πινακα, τοτε ανανεωνεται καθως το επιστρεφει η συναρτηση
    size_of_array = rootReadFromBuilder(fd_root[0], words, size_of_array); //διαβασμα απο το pipe root - builders


    for (int i = 0; i < num_of_builders; i++) {
        int status;
        if (waitpid(builder[i], &status, 0) == -1) {
            perror("error with waitpid builder\n");
        }
    }


    qsort(words, size_of_array, sizeof(WordWithCount), compareWordStructs); //ταξινομηση του πινακα με τις λεξεις

    for(int i = 0; i < size_of_array; i++){
        printf("word: %s with count: %d\n", words[i]->word, words[i]->count);
    }

    close(fd_root[0]);

               
    free(offset_of_line);
    exit(1);
}




int rootReadFromBuilder(int fd_read, WordWithCount* words, int array_size){

    int bytes_to_read = 0;
    int buffer_size = 4096;
    
    int size_word = 0;
    int capacity = 20;
    char* word = malloc(capacity);
    
    int size_count = 0;
    int frequency = 0;
    
    char* buffer = malloc(buffer_size);

    int array_index = 0;

    //διαβαζει δεδομενα της μορφης word:count word:count ...
    while((bytes_to_read = read(fd_read, buffer, buffer_size)) > 0){

       for(int i = 0; i < bytes_to_read; i++){
          
            if(isalpha( buffer[i] )){
                
                if(size_word >= capacity){
                    capacity *= 2;
                    word = realloc(word, capacity);
                }
                
                word[size_word] = buffer[i];
                
                
                size_word++;
            }
            
            //σχηματισμος λεξης
            else if(buffer[i] == ':'){

                word[size_word] = '\0';

                //επαναφορα σε κενη λεξη
                size_word = 0;
                                          
            } 
            
            else if( isdigit(buffer[i]) ){
                size_count++;

                //καθε φορα που διαβαζουμε νεο ψηφιο η θεση του προηγουμενο 
                frequency = frequency * 10 + (buffer[i] - '0'); //μετατροπος απο char σε int αφαιρωντας τον ασκι κωδικο του 0 που ειναι 48
                //και απεχει οσο τον αριθμο απο την ασκι μορφη του
            }

            //μεταβαση στην επομενη λεξη
            else if(buffer[i] == '-'){

                if(array_index >= array_size){
                    array_size *= 2;
                    words = realloc(words, array_size);
                    printf("reallocating array\n");
                }
           
                words[array_index] = malloc(sizeof(struct word_with_count));

                words[array_index]->word = malloc(strlen(word) + 1);
                strcpy(words[array_index]->word, word);
                words[array_index]->count = frequency;

                // printf("word %s with count %d\n", words[array_index]->word, words[array_index]->count);
                
                array_index++;

                // printf("root received word: %s with count: %d\n ", word, frequency);
                

                //επαναφορα σε κενη λεξη
                memset(word, '\0', strlen(word));
                
                //επαναφορα μεταβλητων
                size_count = 0; 
                frequency = 0;
            }
           
        }
      
    }

    memset(buffer, '\0', sizeof(buffer)); //καθαρισμος buffer

    if (bytes_to_read == 0) {
        // printf("End of input in root\n");
    } 
    else if (bytes_to_read < 0) {
        perror("Error reading from pipe");
    }

    free(word);
   
    return array_index;
 
}


int compareWordStructs(const void* a, const void* b){

    const WordWithCount w1 = (const WordWithCount)a;
    const WordWithCount w2 = (const WordWithCount)b;

    if(w1->count < w2->count){
        return 1;
    }
    else if(w1->count > w2->count){
        return -1;
    }
    
    return strcmp(w1->word, w2->word);
}

