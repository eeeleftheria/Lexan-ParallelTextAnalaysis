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
#include "SharedMemory.h"
#include <string.h>


int main(int argc, char* argv[]){

    if(argc != 7){
        printf("Usage: ./receptionist -d ordertime -s shmid -l loggingFile\n");
        exit(1);
    }

    int ordertime;
    char* shmid;
    int fdLogging;

    for(int i = 0; i < argc; i++){
        
        if(strcmp(argv[i], "-d") == 0){
            ordertime = atoi(argv[i+1]);
        }

        if(strcmp(argv[i], "-s") == 0){
            shmid = argv[i+1];
        }
        if(strcmp(argv[i], "-l") == 0){
            // logging file
            char* loggingFile = argv[i+1];
            
            fdLogging = open(loggingFile, O_RDWR | O_APPEND, 0666);
            if(fdLogging == -1){
                printf("failure opening logging in receptionist\n");
                exit(1);
            }

        }
    }

    // open the shared memory segment - only read/write flags since it has already been created
    int fd = shm_open(shmid, O_RDWR, 0666);
    if(fd == -1){
        printf("shm_open failure in receptionist\n");
        exit(1);
    }

    // map the shared memory segment to the address space of the receptionist
    struct sharedObjects* sharedData = mmap(NULL, sizeof(struct sharedObjects), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(sharedData == MAP_FAILED){
        printf("mmap failure in receptionist\n");
        exit(1);
    }

    // ####### RECEPTIONIST CODE #######

    sem_wait(&sharedData->receptionist); // if no customers, suspend the receptionist -> sem = -1

    // if he is woken up, dicrement the mutex so only one process is in critical section
    sem_wait(&sharedData->mutex);

    //  PICKUP ORDER




    

}