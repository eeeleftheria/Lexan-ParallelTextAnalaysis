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


//handler of signal USR1 when a splitter finishes its job.
void splitterIsDone(int signum){
        signal(SIGUSR1, splitterIsDone);
        printf("splitter is done\n");
}


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

    //######### ΥΠΟΛΟΓΙΣΜΟΣ ΓΡΑΜΜΩΝ ΑΡΧΕΙΟΥ ##########//
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
    

    //########### ΥΠΟΛΟΓΙΣΜΟΣ OFFSET ΤΗΣ ΚΑΘΕ ΓΡΑΜΜΗΣ ###########//
    
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


    //####### ΔΗΜΙΟΥΡΓΙΑ PIPES ΓΙΑ ΕΠΙΚΟΙΝΩΝΙΑ SPLITTERS - BUILDERS #######//

    //θελουμε ενα pipe για καθε builder για επικοινωνια splitters-builders, 
    //δηλ num_of_builder pipes με 2 θεσεις το καθενα για τον pipefd
    int pipes_builder[num_of_builders][2]; //pipe[i][0] = read end of pipe i & pipe[i][1] = write end of pipe i
    int write_fds[num_of_builders]; //write_fds[i] = write end fd of builder i

    for(int i = 0; i < num_of_builders; i++){
            
        if(pipe(pipes_builder[i]) == -1){
            perror("error with creation of pipe builder\n");
            return -1;
        }

        write_fds[i] = pipes_builder[i][1];
    }

    // char string_with_fds[64];
    // int offset = 0;
    // for(int i = 0; i < num_of_builders; i++){
    //     char fd_str[10];
    //     printf("%d\n", write_fds[i]);
    //     int written = snprintf(fd_str, sizeof(fd_str), "%d", write_fds[i]); //μετατροπη fd σε string για να τα περασουμε ολα μαζι ως παραμετρο
    //     memcpy(string_with_fds + offset, fd_str, sizeof(fd_str));
    //     memcpy(string_with_fds + offset + 1, " ", 1);
    //     offset += written;
    // }
    // printf("FDS ARE %s\n", string_with_fds);




    int input_of_splitter = lines / num_of_splitter; //γραμμες ανα splitter
    pid_t splitter[num_of_splitter]; //πινακας με τα pid του καθε splitter

    
    //######### ΔΗΜΙΟΥΡΓΙΑ SPLITTERS #########//
      
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

    //ο parent πρεπει να περιμενει 
    for(int i = 0; i < num_of_splitter; i++){
        int status;
        if (waitpid(splitter[i], &status, 0) == -1) {
            perror("error with waitpid splitter\n");
        }
    }

    for (int i = 0; i < num_of_builders; i++){
        close(pipes_builder[i][1]);
    }


    //######### ΔΗΜΙΟΥΡΓΙΑ BUILDERS #########//
    
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

                //θελουμε να κλεισουμε ολα τα read end των pipes 
                //που δεν απασχολουν τον συγκεκριμενο builder καθως και ολα τα write end
                for(int j = 0; j < num_of_builders; j++){
                    if(j != i){
                        close(pipes_builder[j][0]);
                    }
                    close(pipes_builder[j][1]);
                }

                int fd_read = pipes_builder[i][0]; //το read end fd του pipe

                char fd_read_str[10];
                char num_of_builder_str[10];

                snprintf(fd_read_str, sizeof(fd_read_str), "%d", fd_read);
                snprintf(num_of_builder_str, sizeof(num_of_builder_str), "%d", i);

                //εκτελεση του builder που βρισκεται στο ιδιο directory
                execlp("./builder", "builder", num_of_builder_str, fd_read_str, NULL);
                perror("exec failure\n"); //εαν αποτυχει
                exit(1);
            } 
  
    }

    for (int i = 0; i < num_of_builders; i++) {
        int status;
        if (waitpid(builder[i], &status, 0) == -1) {
            perror("error with waitpid builder\n");
        }
    }

    for(int i = 0; i < num_of_builders; i ++){
        close(pipes_builder[i][0]);
        close(pipes_builder[i][1]);
    }


               
    free(offset_of_line);
    exit(1);
}