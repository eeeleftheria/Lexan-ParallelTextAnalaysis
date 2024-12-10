#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>


int main(int agrc, char* argv[]){

    if(argc != 3){
        printf("Usage: ./monitor -s shmid\n");
        exit(1);
    }

    int shmid;

    for(int i = 0; i < argc; i++){
        
        if(strcmp(argv[i], "-s") == 0){
            ordertime = atoi(argv[i+1]);
        }
    }

    

}