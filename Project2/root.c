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
#include <ctype.h>
#include <root.h>
#include <signal.h>

int num_of_usr1 = 0; //μετρητης για το ποσες φορες εχει ληφθει το σημα USR1
int num_of_usr2 = 0; //μετρητης για το ποσες φορες εχει ληφθει το σημα USR2


//handler σηματος USR1 οταν ενας splitter τελειωσει τη δουλεια του
void splitterIsDone(int signum){
        num_of_usr1++;
        signal(SIGUSR1, splitterIsDone); 
}


//handler σηματος USR2 οταν ενας builder τελειωσει τη δουλεια του
void builderIsDone(int signum){
        num_of_usr2++;
        signal(SIGUSR2, builderIsDone);
}

struct word_with_count{
    char* word;
    int* count;
};



int main(int argc, char* argv[]){  

    //struct sigaction: περιγραφει ενα action που θα πραγματοποιηθει για ενα συγκεκριμενο σημα
    //sigaction(): συσχετιζει το action(handler) με το σημα

    struct sigaction sa1;
    sa1.sa_handler = splitterIsDone;  // ορισμος signal handler
    sa1.sa_flags = SA_RESTART;       // SA_RESTART για να συνεχισουν πιθανον μπλοκαρισμενα sys calls 
    sigemptyset(&sa1.sa_mask);       //να μην μπλοκαρει κανενα σημα κατα τη διαρκει της εκτελεσης του κωδικα του handler


    if (sigaction(SIGUSR1, &sa1, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    struct sigaction sa2;
    sa2.sa_handler = builderIsDone;  // ορισμος signal handler
    sa2.sa_flags = SA_RESTART;       // SA_RESTART για να συνεχισουν πιθανον μπλοκαρισμενα sys calls 
    sigemptyset(&sa2.sa_mask);     

    if (sigaction(SIGUSR2, &sa2, NULL) == -1) {
        perror("sigaction");
        return 1;
    }


    
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
        exit(1);
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
        exit(1);
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
            exit(1);
        }
    }

    //Δημιουργια ενος pipe για επικοινωνια builders με ριζα
    int fd_root[2]; //fd_pipe[0] read end, fd_pipe[1] write end
    if(pipe(fd_root) == -1){
        perror("error with creation of pipe builder\n");
        exit(1);
    }


    int input_of_splitter = lines / num_of_splitter; //γραμμες ανα splitter
    pid_t splitter[num_of_splitter]; //πινακας με τα pid του καθε splitter

    
    //########################################## ΔΗΜΙΟΥΡΓΙΑ SPLITTERS ############################################//
      
    for(int i = 0; i < num_of_splitter; i++){
    
        pid_t splitter_pid = fork(); //δημιουργια splitter processes
        
        if(splitter_pid == -1){
            perror("error with fork splitter\n");
            exit(1);
        }
        
        if(splitter_pid != 0){ //εντος parent process
            
            splitter[i] = splitter_pid; //αποθηκευση του pid του splitter i   
          
        }

        if(splitter_pid == 0){ //εντος child process

            //οι splitters πρεπει μονο να γραφουν στα pipes
            for(int j = 0; j < num_of_builders; j++){

                close(pipes_builder[j][0]); //κλεισιμο του read end
               
               if(pipes_builder[j][1] < 0){
                perror("write end of pipe of builder is closed\n");
               }
                if( dup2(pipes_builder[j][1], j + 300) == - 1){
                    perror("dup2 failed\n");
                } //ανακατευθυνση του write end, ωστε να εχουν προσβαση σε αυτο μετα την exec
               
                close(pipes_builder[j][1]); //κλεινουμε το original write end

                //κλεισιμο των fd_root αφου δεν απασχολουν τον splitter
                close(fd_root[0]); 
                close(fd_root[1]);
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

    pid_t builder[num_of_builders];
    
    for(int i = 0; i < num_of_builders; i++){

            pid_t builder_pid = fork(); //δημιουργια builder processes

            if(builder_pid == -1){
                perror("error with fork builder\n");
                return -1;
            }
            if(builder_pid != 0){ //εντος parent process

                builder[i] = builder_pid; //αποθηκευση του pid του παιδιου i
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


    //############################## ΣΥΓΧΡΟΝΙΣΜΟΣ #############################################

    close(fd_root[1]); //ο root πρεπει μονο να διαβαζει απο το pipe root - builders

    //κλεισιμο ολων των ακρων των pipes builders - splitters, αφου δεν αφορουν τον root
    for(int i = 0; i < num_of_builders; i ++){
        close(pipes_builder[i][0]);
        close(pipes_builder[i][1]);
    }

    //ο parent πρεπει να περιμενει αρχικα τον splitter προκειμενου να εχουν σταλθει ολες οι λεξεις στους builders
    for(int i = 0; i < num_of_splitter; i++){
        int status;
        if (waitpid(splitter[i], &status, 0) == -1) {
            perror("error with waitpid splitter\n");
            exit(1);
        }
    }


  
    //############# ΔΙΑΒΑΣΜΑ ΑΠΟΤΕΛΕΣΜΑΤΩΝ ΑΠΟ ROOT #################//
    
    //δημιουργια ενος απλου πινακα που εχει δεικτες σε struct word_with_count με τις λεξεις και τις συχνοτητες τους
    int* array_size = malloc(sizeof(int)); 
    *array_size = 1000; //αρχικο μεγεθος πινακα

    //επιστρεφει τον πινακα με τις λεξεις και τις συχνοτητες τους
    WordWithCount* words = rootReadFromBuilder(fd_root[0], array_size); //διαβασμα απο το pipe root - builders

    //προκειμενου να διαβαζει ο root ταυτοχρονα οσο γραφει ο builder, το wait πρεπει να γινει μετα τη συναρτηση
    for (int i = 0; i < num_of_builders; i++) {
        int status;
        if (waitpid(builder[i], &status, 0) == -1) {
            perror("error with waitpid builder\n");
        }
    }


    rootPrintToOutputFile(output_file, words, num_of_top_popular); //εκτυπωση των πιο δημοφιλων λεξεων στο αρχειο εξοδου
   


    printf("root received %d USR1 signal(s)\n", num_of_usr1);
    printf("root received %d USR2 signal(s)\n", num_of_usr2);
   
    close(fd_root[0]); //κλεισιμο του read end του pipe root - builders

    //απελευθερωση μνημης               
    free(offset_of_line);
    
    for(int i = 0; i < *array_size; i++){
        free(words[i]->word);
        free(words[i]->count);
        free(words[i]);
    }
    
    free(array_size);

    exit(0);
}






WordWithCount* rootReadFromBuilder(int fd_read, int* size){

    int array_size = *size;

    WordWithCount* words = malloc(array_size * sizeof(WordWithCount)); //δεσμευση χωρου για size_of_array δεικτες σε struct word_with_count
    //η malloc επιστρεφει δεικτη στην πρωτη θεση του πινακα

    int buffer_size = 4096;
    int size_word = 0;
    int capacity = 20;
    char* word = malloc(capacity);
    int frequency = 0;
    char* buffer = malloc(buffer_size);

    int array_index = 0;
    int bytes_to_read = 0;

    bool waiting_for_frequency = false;

    //διαβαζει δεδομενα της μορφης word:count word:count ...
    while(( bytes_to_read = read(fd_read, buffer, buffer_size)) > 0){

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
                waiting_for_frequency = true;  
            } 
            
            else if( isdigit(buffer[i]) ){

                if(waiting_for_frequency == true){
                    //καθε φορα που διαβαζουμε νεο ψηφιο η θεση του προηγουμενο 
                    frequency = frequency * 10 + (buffer[i] - '0'); //μετατροπη απο char σε int αφαιρωντας τον ασκι κωδικο του 0 που ειναι 48
                    //και απεχει οσο τον αριθμο απο την ασκι μορφη του
                }
            }

            //μεταβαση στην επομενη λεξη
            else if(buffer[i] == '-'){

                if(array_index >= array_size){
                    array_size *= 2;
                    words = realloc(words, array_size * sizeof(WordWithCount));
                    
                }

                waiting_for_frequency = false;

                words[array_index] = malloc(sizeof(struct word_with_count)); //δεσμευση χωρου για το ιδιο το struct
                //η malloc επιστρεφει δεικτη σε αυτο

                words[array_index]->word = malloc(strlen(word) + 1);
                strcpy(words[array_index]->word, word);

                words[array_index]->count = malloc(sizeof(int));
                *(words[array_index]->count) = frequency; 
                
                array_index++;

                //επαναφορα σε κενη λεξη
                memset(word, '\0', strlen(word));
                
                //επαναφορα μεταβλητων
                frequency = 0;
                size_word = 0;
            }
                    
        }
      
    memset(buffer, '\0', buffer_size); //καθαρισμος buffer
    }

    if (bytes_to_read == 0) {
        // printf("End of input in root\n");
    } 
    else if (bytes_to_read < 0) {
        perror("Error reading from pipe");
    }

    

    for(int i = array_index; i < array_size; i++){
        words[i] = malloc(sizeof(struct word_with_count));
        words[i]->word = NULL;
        words[i]->count = NULL;

    }

    *size = array_size;
    free(word);
    free(buffer);

    qsort(words, array_index, sizeof(WordWithCount), compareWordStructs); //ταξινομηση των λεξεων με βαση τη συχνοτητα τους

    return words;
}



//δεχεται δυο δεικτες σε WordWithCount και συγκρινει τις συχνοτοτητες των λεξεων αφου
//η qsort καλειται με δυο δεικτες στα στοιχεια του πινακα και οχι με τα ιδια τα στοιχεια
int compareWordStructs(const void* a, const void* b) {
    const WordWithCount* p1 = (const WordWithCount*)a;
    const WordWithCount* p2 = (const WordWithCount*)b;

    WordWithCount w1 = *p1;
    WordWithCount w2 = *p2;

    int* count1 = w1->count;
    int* count2 = w2->count;

    if(count1 == NULL || count2 == NULL){
        return 0;
    }
    else if(count1 == NULL){
        return 1;
    }
    else if(count2 == NULL){
        return -1;
    }

    if ((*count1) < *(count2)) {
        return 1;
    }
    else if (*(count1) > *(count2)) {
        return -1;
    }
    
    return strcmp(w1->word, w2->word);
}


//εκτυπωση των num_of_top_popular πιο δημοφιλων λεξεων στο output file
void rootPrintToOutputFile(char* output, WordWithCount* words,  int num_of_top_popular){

    //ανοιγμα αρχειου εξοδου
                    //εγγραφη μονο, δημιουργια αν δεν υπαρχει, διαγραφη περιεχομενων αν δεν ειναι αδειο
    int fd = open(output, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(fd < 0){
        char* message = "Failure opening output file\n";
        write(STDERR_FILENO, message, strlen(message));
        return;
    }

    printf("The %d most popular words are:\n", num_of_top_popular);

    //εκτυπωση των num_of_top_popular πιο δημοφιλων λεξεων


    for(int i = 0; i < num_of_top_popular; i++){
  
        char* word = words[i]->word;
        int count = *(words[i]->count);
        char count_str[10];
        snprintf(count_str, sizeof(count_str), "%d", count); //μετατροπη του count σε string
  
        printf("%s: %d\n", word, count);

        write(fd, word, strlen(word));
        write(fd, ":", 1);
        write(fd, count_str, strlen(count_str));
        write(fd, "\n", 1);

    }
  


    close(fd);

}