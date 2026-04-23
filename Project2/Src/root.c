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

int num_of_usr1 = 0; // counter for how many times USR1 signal has been received
int num_of_usr2 = 0; // counter for how many times USR2 signal has been received


// signal handler for USR1 when a splitter finishes its work
void splitterIsDone(int signum){
        num_of_usr1++;
        signal(SIGUSR1, splitterIsDone); 
}


// signal handler for USR2 when a builder finishes its work
void builderIsDone(int signum){
        num_of_usr2++;
        signal(SIGUSR2, builderIsDone);
}

struct word_with_count{
    char* word;
    int* count;
};



int main(int argc, char* argv[]){  

    // struct sigaction: describes an action that will be performed for a specific signal
    // sigaction(): associates the action(handler) with the signal

    struct sigaction sa1;
    sa1.sa_handler = splitterIsDone;  // signal handler definition
    sa1.sa_flags = SA_RESTART;       // SA_RESTART so that potentially blocked system calls continue 
    sigemptyset(&sa1.sa_mask);       // do not block any signal during handler execution


    if (sigaction(SIGUSR1, &sa1, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    struct sigaction sa2;
    sa2.sa_handler = builderIsDone;  // signal handler definition
    sa2.sa_flags = SA_RESTART;       // SA_RESTART so that potentially blocked system calls continue 
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
    
    // open file to count its lines
    int fd = open(input_file, O_RDONLY);
    if(fd < 0){
        char* message = "Failure opening input file\n";
        write(STDERR_FILENO, message, strlen(message));
        exit(1);
    }

    //################ CALCULATION OF FILE'S NUMBER OF LINES ###################//
    int lines = 1;//κραταμε τον αριθμο γραμμων του αρχειου
    char c = 0;

    // read file character by character
    int bytes_to_read;
    while((bytes_to_read =  read(fd, &c, sizeof(c))) > 0) {  // for EOF returns 0
        
        if(c == '\n'){
            lines++;
        }
    }

    if (bytes_to_read == 0) {
        // perror("End of input in root\n");
    } 
    else if (bytes_to_read < 0) {
        perror("Error reading from input file\n");
        exit(1);
    }

    lseek(fd, 0, SEEK_SET); //επαναφορα δεικτη στην αρχη του αρχειου
    

// array with line offsets
    long int* offset_of_line = malloc((lines + 1) * sizeof(long int)); // +1 because we don't count position 0 as a line
    int count = 1; // line counter
    int bytes = 0; // how many bytes we have read up to a point
    offset_of_line[0] = 0;
    
    while(read(fd, &c, sizeof(c)) > 0) {  
        bytes += sizeof(c);
        
        if(c == '\n'){  // new line
            offset_of_line[count] = offset_of_line[count-1] + bytes;
            count++;
            bytes = 0;
        }
        // if it is the last line it may not end with \n
        if(count == lines){
            offset_of_line[count] = offset_of_line[count-1] + bytes;
        }
            
    }

    close(fd); // close the file, we will reopen it through pipes


    // CREATE PIPES FOR SPLITTERS-BUILDERS COMMUNICATION

    // we want one pipe for each builder for splitters-builders communication,
    // i.e. num_of_builder pipes with 2 positions each for the pipefd
    int pipes_builder[num_of_builders][2]; // pipe[i][0] = read end of pipe i & pipe[i][1] = write end of pipe i

    for(int i = 0; i < num_of_builders; i++){
            
        if(pipe(pipes_builder[i]) == -1){
            perror("error with creation of pipe builder\n");
            exit(1);
        }
    }

    // Create a pipe for communication between builders and root
    int fd_root[2]; // fd_pipe[0] read end, fd_pipe[1] write end
    if(pipe(fd_root) == -1){
        perror("error with creation of pipe builder\n");
        exit(1);
    }


    int input_of_splitter = lines / num_of_splitter; // lines per splitter
    pid_t splitter[num_of_splitter]; // array with the pid of each splitter

    
    // CREATE SPLITTERS
      
    for(int i = 0; i < num_of_splitter; i++){
    
        pid_t splitter_pid = fork(); // create splitter processes
        
        if(splitter_pid == -1){
            perror("error with fork splitter\n");
            exit(1);
        }
        
        if(splitter_pid != 0){ // inside parent process
            
            splitter[i] = splitter_pid; // store the pid of splitter i   
          
        }

        if(splitter_pid == 0){ // inside child process

            // splitters should only write to the pipes
            for(int j = 0; j < num_of_builders; j++){

                close(pipes_builder[j][0]); // close the read end
                dup2(pipes_builder[j][1], j + 500); // redirect the write end so they have access after exec
                close(pipes_builder[j][1]); // close the original write end

                // close fd_root since it doesn't concern the splitter
                close(fd_root[0]); 
                close(fd_root[1]);
            }
            

            // for calculating the range of lines for each splitter
            int start_line = 0;
            int end_line = 0;

            // if it is the 1st splitter read from the beginning of the file to input_of_splitter
            if(i == 0){
                start_line = 1;
                end_line = input_of_splitter;
            }
            // read from where the previous one stopped
            else if((i != 0) && (i != num_of_splitter - 1)){
                start_line = i * input_of_splitter + 1;
                end_line = start_line + input_of_splitter - 1; // e.g. if start_line = 5 and input = 5, must read until line 9
            }
            // if it is the last splitter read until end of file
            // because if it doesn't divide exactly, lines will remain.
            else if(i == num_of_splitter - 1){
                start_line = i * input_of_splitter + 1;
                end_line = lines;
            }   


            long int offset_start_line = offset_of_line[start_line - 1]; // Bytes until before the line start_line

            char offset_start_line_str[10]; 
            char start_line_str[10];
            char end_line_str[10]; 
            char num_of_builders_str[10];
            
            // convert integers to string for exec
            snprintf(start_line_str, sizeof(start_line_str), "%d", start_line);
            snprintf(end_line_str, sizeof(end_line_str), "%d", end_line);  
            snprintf(offset_start_line_str, sizeof(offset_start_line_str), "%ld", offset_start_line);
            snprintf(num_of_builders_str, sizeof(num_of_builders_str), "%d", num_of_builders);

            // execute the splitter located in the same directory
            execlp("./splitter", "splitter", input_file, start_line_str, end_line_str, 
                offset_start_line_str, exclusion_list_file, num_of_builders_str, NULL); 
		    
            perror("exec failure\n");  // execution won't reach this point if exec is successful    
            exit(1);         

        }
    }   

    
    //#################################################### ΔΗΜΙΟΥΡΓΙΑ BUILDERS ######################################################//

    pid_t builder[num_of_builders];
    
    for(int i = 0; i < num_of_builders; i++){

            pid_t builder_pid = fork(); // create builder processes

            if(builder_pid == -1){
                perror("error with fork builder\n");
                return -1;
            }
            if(builder_pid != 0){ // inside parent process

                builder[i] = builder_pid; // store the PID of child i
            }

            if(builder_pid == 0){ // inside child process

                close(fd_root[0]); // only write to root
                
                // we want to close all read ends of pipes
                // that don't concern this specific builder as well as all write ends
                for(int j = 0; j < num_of_builders; j++){
                    if(j != i){
                        close(pipes_builder[j][0]);
                    }
                    close(pipes_builder[j][1]);
                }

                int fd_read = pipes_builder[i][0]; // the read end fd of the pipe
                int fd_root_write_end = fd_root[1]; 

                char fd_read_str[10];
                char num_of_builder_str[10];
                char fd_root_write_str[10];

                snprintf(fd_read_str, sizeof(fd_read_str), "%d", fd_read);
                snprintf(num_of_builder_str, sizeof(num_of_builder_str), "%d", i);
                snprintf(fd_root_write_str, sizeof(fd_root_write_str), "%d", fd_root_write_end);


                // execute the builder located in the same directory
                execlp("./builder", "builder", num_of_builder_str, fd_read_str, fd_root_write_str, NULL);
                perror("exec failure\n"); // if it fails
                exit(1);
            } 
  
    }  



    // SYNCHRONIZATION

    close(fd_root[1]); // root should only read from the root-builders pipe

    // close all ends of builders-splitters pipes, since they don't concern root
    for(int i = 0; i < num_of_builders; i ++){
        close(pipes_builder[i][0]);
        close(pipes_builder[i][1]);
    }

    // the parent should initially wait for the splitter so that all words have been sent to the builders
    for(int i = 0; i < num_of_splitter; i++){
        int status;
        if (waitpid(splitter[i], &status, 0) == -1) {
            perror("error with waitpid splitter\n");
            exit(1);
        }
    }


  
    // READ RESULTS FROM ROOT
    
    // create a simple array that has pointers to struct word_with_count with words and their frequencies
    int* array_size = malloc(sizeof(int)); 
    *array_size = 1000; // initial array size

    // returns the array with words and their frequencies
    WordWithCount* words = rootReadFromBuilder(fd_root[0], array_size); // read from the root-builders pipe

    // so that root can read simultaneously as the builder writes, wait must be done after the function
    for (int i = 0; i < num_of_builders; i++) {
        int status;
        if (waitpid(builder[i], &status, 0) == -1) {
            perror("error with waitpid builder\n");
        }
    }


    rootPrintToOutputFile(output_file, words, num_of_top_popular); // print the most popular words to the output file
   


    printf("\nroot received %d USR1 signal(s)\n", num_of_usr1);
    printf("root received %d USR2 signal(s)\n", num_of_usr2);
   
    close(fd_root[0]); // close the read end of the root-builders pipe

    // free memory               
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

    WordWithCount* words = malloc(array_size * sizeof(WordWithCount)); // allocate space for size_of_array pointers to struct word_with_count
    // malloc returns a pointer to the first position of the array

    int buffer_size = 4096;
    int size_word = 0;
    int capacity = 20;
    char* word = malloc(capacity);
    int frequency = 0;
    char* buffer = malloc(buffer_size);

    int array_index = 0;
    int bytes_to_read = 0;

    bool waiting_for_frequency = false;

    // read data in the form word:count word:count ...
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
            
            // word formation
            else if(buffer[i] == ':'){
                word[size_word] = '\0';    
                waiting_for_frequency = true;  
            } 
            
            else if( isdigit(buffer[i]) ){

                if(waiting_for_frequency == true){
                    // each time we read a new digit its position is the previous one
                    frequency = frequency * 10 + (buffer[i] - '0'); // convert from char to int by subtracting ASCII code of 0 which is 48
                    // and differs by the number from its ASCII form
                }
            }

            // move to the next word
            else if(buffer[i] == '-'){

                if(array_index >= array_size){
                    array_size *= 2;
                    words = realloc(words, array_size * sizeof(WordWithCount));
                    
                }

                waiting_for_frequency = false;

                words[array_index] = malloc(sizeof(struct word_with_count)); // allocate space for the struct itself
                // malloc returns a pointer to it

                words[array_index]->word = malloc(strlen(word) + 1);
                strcpy(words[array_index]->word, word);

                words[array_index]->count = malloc(sizeof(int));
                *(words[array_index]->count) = frequency; 
                
                array_index++;

                // restore to empty word
                memset(word, '\0', strlen(word));
                
                // restore variables
                frequency = 0;
                size_word = 0;
            }
                    
        }
      
    memset(buffer, '\0', buffer_size); // clear buffer
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

    qsort(words, array_index, sizeof(WordWithCount), compareWordStructs); // sort words by their frequency

    return words;
}



// takes two pointers to WordWithCount and compares the frequencies of words since
// qsort is called with two pointers to array elements, not the elements themselves
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


// print the num_of_top_popular most popular words to the output file
void rootPrintToOutputFile(char* output, WordWithCount* words,  int num_of_top_popular){

    // open output file
    // write only, create if it doesn't exist, truncate contents if not empty
    int fd = open(output, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(fd < 0){
        char* message = "Failure opening output file\n";
        write(STDERR_FILENO, message, strlen(message));
        return;
    }

    printf("The %d most popular words are:\n", num_of_top_popular);

    // print the num_of_top_popular most popular words


    for(int i = 0; i < num_of_top_popular; i++){
  
        char* word = words[i]->word;
        int count = *(words[i]->count);
        char count_str[10];
        snprintf(count_str, sizeof(count_str), "%d", count); // convert count to string
  
        printf("%s: %d\n", word, count);

        write(fd, word, strlen(word));
        write(fd, ":", 1);
        write(fd, count_str, strlen(count_str));
        write(fd, "\n", 1);

    }
  


    close(fd);

}