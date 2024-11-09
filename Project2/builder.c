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

    printf("fd of builder: %d\n", fd_read_end);

    while((bytes_to_read = read(fd_read_end, buffer, size_buffer)) > 0){
        printf("builder received bytes: %d\n", bytes_to_read);
        // printf("builder received: %s\n", buffer);

        // int start_i = 0;
        // bool start_of_size = false; 
        // bool start_of_word = false;
        // int size = 0;
        // char* word;
        // for(int i = 0; i < sizeof(buffer); i++){
            
        //     if( isalpha(buffer[i]) == false && (start_of_size == false)){
        //             start_of_size = true;
        //             start_i = i;
        //             i++;
        //     }
            
        //     if( isalpha(buffer[i]) == true){
                
        //         if(start_of_word == false){
        //             start_of_word = true;
        //             int digits = i - start_i; //ποσα ψηφια εχει ο αριθμος
                    
        //             for(int j = start_i; j < i; j++){
        //                 size += size * 10 + (buffer[j] - '0');
        //                 digits--;
        //             }

        //             word = malloc(size);
        //             memcpy(word, buffer + i, size);
        //             word[size] = '\0';
        //             printf("Word: %s\n", word);
        //             start_of_size = false;;
        //             start_of_word = false;
        //             i = i + size - 1;

        //             free(word);
        //         }                
        //     }
        // }
    }
    
    if (bytes_to_read == 0) {
        printf("End of input stream\n");
    } 
    else if (bytes_to_read < 0) {
        perror("Error reading from pipe");
    }

}