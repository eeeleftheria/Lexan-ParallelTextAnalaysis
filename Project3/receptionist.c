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

    if(argc != 5){
        printf("Usage: ./receptionist -d ordertime -s shmid\n");
        exit(1);
    }

    int ordertime;
    int shmid;

    for(int i = 0; i < argc; i++){
        
        if(strcmp(argv[i], "-d") == 0){
            ordertime = atoi(argv[i+1]);
        }

        if(strcmp(argv[i], "-s") == 0){
            shmid = atoi(argv[i+1]);
        }
    }

    

}