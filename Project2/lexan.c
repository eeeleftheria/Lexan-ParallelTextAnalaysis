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


FILE* file;
   
char* input_file = NULL;
char* output_file = NULL;
char* exclusion_list_file = NULL;
int num_of_splitter, num_of_builders, num_of_top_popular;

int main(int argc, char* argv[]){
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

    char buffer[200];
    int count = 0;//κραταμε τον αριθμο γραμμων του αρχειου

    //αναγνωση περιεχομενου κ αποθηκευση μεσα στο buffer
    //η fgets σταματαει να διαβαζει οταν συναντησει α΄΄λλαγη γραμμης ή EOF
    while(fgets(buffer, 200, file) != NULL) { 
        count++;
    }

    //ποσες γραμμες μπορει να διαβασει καθε splitter
    int input_of_splitter = count / num_of_splitter;

    pid_t splitter[num_of_splitter]; //πινακας με τα pid του καθε splitter

    for(int i = 0; i < num_of_splitter; i++){
        pid_t child_pid = fork();
        
        if(child_pid == -1){
            perror("error with fork\n");
            return -1;
        }
        
        splitter[i] = child_pid; //αποθηκευση του pid του splitter i

        if(child_pid == 0){ //εντος child process

        }
    }

}