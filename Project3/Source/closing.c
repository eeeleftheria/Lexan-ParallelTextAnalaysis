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
        printf("Usage: ./closing -s shmid -t openTimeOfBar -l logFile\n");
        exit(1);
    }

    char* shmid;
    int openTimeOfBar;
    char* logFile;

    for(int i = 0; i < argc; i++){
        
        if(strcmp(argv[i], "-s") == 0){
            shmid = argv[i+1];
        }
        
        if(strcmp(argv[i], "-t") == 0){
            openTimeOfBar = atoi(argv[i+1]);
        }

        if(strcmp(argv[i], "-l") == 0){
            logFile = argv[i+1];
        }
    }

    // open the shared memory segment 
    int fd = shm_open(shmid, O_RDWR, 0666);
    if(fd == -1){
        printf("shm_open failure in closing\n");
        exit(1);
    }

    // map the shared memory segment to the address space of the closing process
    struct sharedObjects* sharedData = mmap(NULL, sizeof(struct sharedObjects), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(sharedData == MAP_FAILED){
        printf("mmap failure in closing\n");
        exit(1);
    }

    int fdLog = open(logFile, O_RDWR | O_APPEND, 0666);
    if(fdLog == -1){
        printf("failure opening log file in closing\n");
        exit(1);
    }


    // the time the bar remains open
    sleep(openTimeOfBar);

    sem_wait(&sharedData->mutex);
    // the bar is closing
    sharedData->isClosing = true;

 
    char message[100];
    sprintf(message, "CLOSING: Bar is closing\n\n");
    write(fdLog, message, strlen(message));

    close(fdLog);
    
    sem_post(&sharedData->receptionist);

    sem_post(&sharedData->mutex);

    // detach the shared memory segment
    if(munmap(sharedData, sizeof(struct sharedObjects)) == -1){
        printf("munmap failure in closing\n");
        exit(1);
    }

    exit(0);    


}