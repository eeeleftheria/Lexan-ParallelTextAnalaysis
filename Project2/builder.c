#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include "splitter.h"


int main(int argc, char* argv[]){

    if(argc != 2){
        char* message = "Error\nUsage is: ./builder\n";
        write(STDOUT_FILENO, message, strlen(message));
        exit(1);
    }

    int num_of_builders = atoi(argv[1]);
    



  
    
}