#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

int main(int argc, char* argv[]){

    char* input_file = NULL;
    int start_line = 0;
    int end_line = 0;
    long int offset__of_start_line = 0;

    if(argc != 5){
        char* message = "Error\nUsage is: ./splitter input_file start_line end_line offset\n";
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
    }

    int fd = open(input_file, O_RDONLY);
    if(fd < 0){
        fprintf(stderr, "Failure opening input file\n");
    }

    int bytes_to_read;
    char c;
    int lines = start_line;

    printf("\nIN SPLITTER %d %d\n", start_line, end_line);

    //πηγαινουμε τον δεικτη διαβασματος στη γραμμη start_line
    //απο την οποια πρεπει να διαβασει ο splitter
    lseek(fd, offset__of_start_line, SEEK_SET);
    while((bytes_to_read = read(fd, &c, sizeof(c))) > 0){

        if(bytes_to_read < 0) {
            perror("error reading from pipe in splitter\n");
        }

        if(c == '\n'){
            lines++;
        }

        printf("%c", c);
        
        if(c == EOF || lines == end_line + 1){
            break;
        }
    }

    printf("\nI AM SPLITTER %d %d AND FINISHED\n", start_line, end_line);

    close(fd);
    
    exit(1);
    
}