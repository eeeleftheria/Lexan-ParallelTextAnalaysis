#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/types.h>
#include "splitter.h"


int main(int argc, char* argv[]){

    if(argc != 3){
        char* message = "Error\nUsage is: ./builder numOfBuilder fdReadEnd\n";
        write(STDOUT_FILENO, message, strlen(message));
        exit(1);
    }

    int num_of_builder = atoi(argv[1]);
    int fd_read_end = atoi(argv[2]);
 

    int size_buffer = 1046;
    char* buffer = malloc(size_buffer);
    int bytes_to_read;
    int size = 0;

    printf("\n\nread fd of builder: %d\n", fd_read_end);
    char* word;

    while((bytes_to_read = read(fd_read_end, buffer, size_buffer)) > 0){
       
       printf("builder received bytes: %d\n", bytes_to_read);
       
       for(int i = 0; i < bytes_to_read; i++){
          

            if(isalpha( buffer[i] )){
                size++;
            }
            else if(buffer[i] == ' '){
                word = malloc(size + 1); //+1 for \0
                memcpy(word, buffer + i - 1 - size, size);
                word[size] = '\0';
                printf("builder received word: %s\n", word);
              
                size = 0;
                free(word);
            } 
           
        }
      
    }
    if (bytes_to_read == 0) {
    printf("End of input in builder\n");
    } 
    else if (bytes_to_read < 0) {
        perror("Error reading from pipe");
    }

    close(fd_read_end);

    exit(1);

}