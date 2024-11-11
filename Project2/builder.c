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
<<<<<<< HEAD
 
=======
>>>>>>> 23d37c6571b1240b98922b16bcd5b6d4951a4e9d

    int size_buffer = 1046;
    char* buffer = malloc(size_buffer);
    int bytes_to_read;
<<<<<<< HEAD
    int size = 0;

    printf("read fd of builder: %d\n", fd_read_end);
=======

    printf("read fd of builder: %d\n", fd_read_end);
        bool start_of_size = false; 
        bool start_of_word = false;
        int start_i = 0;
        int size = 0;
>>>>>>> 23d37c6571b1240b98922b16bcd5b6d4951a4e9d
        char* word;

    while((bytes_to_read = read(fd_read_end, buffer, size_buffer)) > 0){
        printf("builder received bytes: %d\n", bytes_to_read);
       
<<<<<<< HEAD
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
    
        if (bytes_to_read == 0) {
            printf("End of input stream\n");
        } 
        else if (bytes_to_read < 0) {
            perror("Error reading from pipe");
        }

    }
=======
        printf("builder received: %s\n", buffer);

        // for(int i = 0; i < bytes_to_read; i++){
        //     if( isalpha(buffer[i]) == false && (start_of_size == false)){
        //             start_of_size = true;
        //             start_i = i;
        //             size = 0;
        //             printf("start_i: %d\n", start_i);
        //     }
          
        //     if( isalpha((unsigned char)buffer[i]) == true){
        //         printf("INNN---");
        //         fflush(stdout);
                
        //         if(start_of_word == false){
        //             start_of_size = false;
        //             start_of_word = true;
        //             int digits = i - start_i; //ποσα ψηφια εχει ο αριθμος
        //             printf("digits: %d\n", digits);
                    
        //             for(int j = start_i; j < i; j++){
        //                 size = size * 10 + (buffer[j] - '0');
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

>>>>>>> 23d37c6571b1240b98922b16bcd5b6d4951a4e9d
}