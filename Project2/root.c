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
        fprintf(stderr, "Failure opening input file\n");
    }


    int lines = 1;//κραταμε τον αριθμο γραμμων του αρχειου
    char c = 0;

    //διαβασμα αρχειου ανα χαρακτηρα, ωστε να μετρησουμε το πληθος γραμμων
    while(read(fd, &c, sizeof(c)) > 0) {  //για EOF επιστρεφει 0
        if(c == '\n'){
            lines++;
        }
    }

    //κραταμε εναν πινακα με τα offset των γραμμων
    long int* offset_of_line = malloc((lines + 1) * sizeof(long int)); //πινακας με τα offset των γραμμων
    lseek(fd, 0, SEEK_SET); //επαναφορα δεικτη στην αρχη του αρχειου
    int count = 1;
    int bytes = 0;
    offset_of_line[0] = 0;
    
    while(read(fd, &c, sizeof(c)) > 0) {  
        bytes += sizeof(c);
        
        if(c == '\n'){ 
            offset_of_line[count] = offset_of_line[count-1] + bytes;
            // printf("LINE %d %ld\n", count, offset_of_line[count]);
            count++;
            bytes = 0;
        }
        //αν ειναι η τελευταια γραμμη μπορει να μην τελειωνει με \n
        if(count == lines){
            offset_of_line[count] = offset_of_line[count-1] + bytes;
        }
            
    }


    close(fd); //κλεισιμο του αρχειου, θα το ξαναανοιξουμε μεσω των pipes

    int input_of_splitter = lines / num_of_splitter; //γραμμες ανα splitter

    pid_t splitter[num_of_splitter]; //πινακας με τα pid του καθε splitter


    //######### ΔΗΜΙΟΥΡΓΙΑ SPLITTERS #########//
      
    for(int i = 0; i < num_of_splitter; i++){
    
        pid_t splitter_pid = fork(); //δημιουργια splitter processes
        
        if(splitter_pid == -1){
            perror("error with fork splitter\n");
            return -1;
        }
        
        splitter[i] = splitter_pid; //αποθηκευση του pid του splitter i   

        if(splitter_pid != 0){ //εντος parent process
            
            int status;
            if(waitpid(splitter_pid, &status, 0) == -1){
                perror("error with waitpid splitter\n");
                exit(1);
            }
        }

        if(splitter_pid == 0){ //εντος child process

            //για τον υπολογισμο του ευρους γραμμων καθε splitter
            int start_line = 0;
            int end_line = 0;

            //αν ειναι ο 1ος splitter διαβαζει απο την αρχη του αρχειου 
            //μεχρι το input_of_splitter
            if(i == 0){
                start_line = 1;
                end_line = input_of_splitter;
            }
            //διαβαζει απο εκει που σταματησε ο προηγουμενος
            else if((i != 0) && (i != num_of_splitter - 1)){
                start_line = i * input_of_splitter + 1;
                end_line = start_line + input_of_splitter - 1;
            }
            //αν ειναι ο τελευταιος splitter διαβαζει μεχρι το τελος του αρχειου
            //γιατι αν δεν διαιρειται ακριβως, θα περισσεψουν γραμμες. 
            else if(i == num_of_splitter - 1){
                start_line = i * input_of_splitter + 1;
                end_line = lines;
            }   

            // printf("start %d end %d\n", start_line, end_line);

            long int offset_start_line = offset_of_line[start_line - 1]; //ta bytes mexri kai prin thn grammh start_line

            char offset_start_line_str[10]; 
            char start_line_str[10];
            char end_line_str[10]; 
            
            //μετατροπη των ακεραιων σε string για την exec
            snprintf(start_line_str, sizeof(start_line_str), "%d", start_line);
            snprintf(end_line_str, sizeof(end_line_str), "%d", end_line);  
            snprintf(offset_start_line_str, sizeof(offset_start_line_str), "%ld", offset_start_line);

            //εκτελεση του splitter που βρισκεται στο ιδιο directory
            execlp("./splitter", "splitter", input_file, start_line_str, end_line_str, 
                offset_start_line_str, NULL); 
		    perror("exec failure\n");       
            exit(1);         

        }
    }   


    //######### ΔΗΜΙΟΥΡΓΙΑ BUILDERS #########//


    //θελουμε ενα pipe για καθε builder, 
    //δηλ num_of_builder pipes με 2 θεσεις το καθενα για τον pipefd
    int pipes_builder[num_of_builders][2]; //pipe[i][0] = read end of pipe i & pipe[i][1] = write end of pipe i

    for(int i = 0; i < num_of_builders; i++){
            if(pipe(pipes_builder[i]) == -1){
                perror("error with pipe builder\n");
                return -1;
            }

            pid_t builder_pid = fork(); //δημιουργια builder processes

            if(builder_pid == -1){
                perror("error with fork builder\n");
                return -1;
            }
            if(builder_pid != 0){
                int status;
                if(waitpid(builder_pid, &status, 0) == -1){
                    perror("error with waitpid builder\n");
                    exit(1);
                }
            }
            if(builder_pid == 0){
                execlp("./builder", "builder", NULL); //εκτελεση του builder που βρισκεται στο ιδιο directory
                perror("exec failure\n");
                exit(1);
            }              
    }


            
    free(offset_of_line);
    exit(1);
}