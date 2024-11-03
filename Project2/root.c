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



int main(int argc, char* argv[]){

    
    char* input_file = NULL;
    char* output_file = NULL;
    char* exclusion_list_file = NULL;
    int num_of_splitter, num_of_builders, num_of_top_popular;
    if(argc != 13){
        printf("Error\nUsage is: ./lexan -i TextFile -l numOfSplitter -m numOfBuilders -t TopPopular -e ExclusionList -o OutputFile\n");
        return -1;
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

    int fd = open(input_file, O_RDONLY);
    if(fd < 0){
        fprintf(stderr, "Failure opening input file\n");
    }


    int lines = 0;//κραταμε τον αριθμο γραμμων του αρχειου
    char c = 0;

    //διαβασμα αρχειου ανα χαρακτηρα, ωστε να μετρησουμε το πληθος γραμμων
    while(read(fd, &c, sizeof(c)) > 0) {  //για EOF επιστρεφει 0
        if(c == '\n'){
            lines++;
        }
    }

    lseek(fd, 0, SEEK_SET); //επαναφορα δεικτη στην αρχη του αρχειου

    //ποσες γραμμες μπορει να διαβασει καθε splitter
    int input_of_splitter = lines / num_of_splitter;

    // pid_t splitter[num_of_splitter]; //πινακας με τα pid του καθε splitter


    //######### ΔΗΜΙΟΥΡΓΙΑ SPLITTERS #########//
   
    //θελουμε ενα pipe για καθε ζευγος parent-child, 
    //δηλ num_of_splitter pipes με 2 θεσεις το καθενα για τον pipefd
    int pipes_splitter[num_of_splitter][2]; //pipe[i][0] = read end of pipe i & pipe[i][1] = write end of pipe i
   
    for(int i = 0; i < num_of_splitter; i++){
    
        if(pipe(pipes_splitter[i]) == -1){ //δημιουργια pipe i 
            perror("error creating pipe splitter\n");
            exit(1);
        } 

        pid_t splitter_pid = fork(); //δημιουργια splitter processes
        
        if(splitter_pid == -1){
            perror("error with fork splitter\n");
            return -1;
        }
        
        // splitter[i] = child_pid; //αποθηκευση του pid του splitter i   

        if(splitter_pid != 0){ //εντος parent process
            close(pipes_splitter[i][0]); //μονο γραψιμο
        }

        if(splitter_pid == 0){ //εντος child process
            
            close(pipes_splitter[i][1]); //μονο αναγνωση 
            
            //πρεπει να ανακατευθυνουμε το read end του pipe στο std input, ωστε το παιδι
            //να μην χρειαζεται να γνωριζει τον fd του pipe για να διαβασει
            //Ετσι, καθε φορα που θα διαβαζει απο το std input θα διαβαζει στην πραγματικοτητα
            //απο το read end του pipe. Δηλ, δειχνει στο ιδιο αρχειο με το pipe[i][0].
            dup2(pipes_splitter[i][0], STDIN_FILENO); 
            
            close(pipes_splitter[i][0]);
            execlp("./splitter", "splitter", NULL); //εκτελεση του splitter που βρισκεται στο ιδιο directory
		    perror("exec failure\n");

        }
    }

    //#### ΕΓΓΡΑΦΗ 1/L ΓΡΑΜΜΩΝ ΑΡΧΕΙΟΥ ΣΕ ΚΑΘΕ SPLITTER ####//
    for(int i = 1; i <= num_of_splitter; i++){
     
        int count = 0;

        //στον τελευταιο splitter πρεπει να γραψουμε οσες
        //γραμμες εχουν απομεινει
        if(i == num_of_splitter){
            input_of_splitter = lines - ((num_of_splitter - 1) * input_of_splitter);
        }

        while(read(fd, &c, sizeof(c)) > 0){
            if(c == '\n'){
                count++; //μετρητης γραμμων, ωστε να ξερει μεχρι που πρεπει
                //να διαβασει απο το αρχειο για το συγκεκριμενο child
            }
            
            write(pipes_splitter[i][1], &c, sizeof(c)); //εγγραφη καθε χαρακτηρα στο pipe

            //αν διαβασαμε οσες γραμμες πρεπει, βγαινουμε απο το while
            //και συνεχιζει το διαβασμα ο επομενος απο το σημειο αυτο
            if(count == input_of_splitter){
                close(pipes_splitter[i][1]); //αφου δεν το χρειαζομαστε αλλο το write end
                break;
            }
        }
    }



    //######### ΔΗΜΙΟΥΡΓΙΑ BUILDERS #########//
    int pipes_builder[num_of_builders][2];
   
    // for(int i = 0; i < num_of_builders; i++){
        
    //     if(pipe(pipes_builder[i]) == -1){ //δημιουργια pipe i 
    //         perror("error creating pipe builder\n");
    //         exit(2);
    //     } 

    //     pid_t builder_pid = fork();

    //     if(builder_pid = -1) {
    //         perror("error with fork builder");
    //         return -1;
    //     }

    //     if(builder_pid > 0) { //εντος root process
    //         close(pipes_builder[i][1]); //μονο διαβασμα απο τον builder
    //     }

    //     if(builder_pid == 0){ //εντος builder process
    //         close(pipes_builder[i][0]); //μονο εγγραφη στον root
    //     }
    // }
   

}