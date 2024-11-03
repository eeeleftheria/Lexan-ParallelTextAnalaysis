#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

int main(){

    char buffer[1024];
    int bytes_to_read;
    char c;
    while(bytes_to_read = read(STDIN_FILENO, &c, sizeof(c)) > 0){

        if(bytes_to_read < 0){
            perror("error reading from pipe in splitter\n");
        }
        printf("%c", c);
    }
    
}