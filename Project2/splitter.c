#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

int main(){

    int bytes_to_read;
    char c = 0;
    while((bytes_to_read = read(STDIN_FILENO, &c, sizeof(c))) > 0){

        if(bytes_to_read < 0){
            perror("error reading from pipe in splitter\n");
        }
        // printf("%c", c);
    }
    
}