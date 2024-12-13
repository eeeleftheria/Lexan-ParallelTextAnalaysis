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
#include "visitor.h"


int main(int argc, char* argv[]){

    if(argc != 7){
        printf("Usage: ./visitor -d resttime -s shmid -l loggingFile\n");
        exit(1);
    }

    int resttime;
    char* shmid;
    int fdLogging;

    for(int i = 0; i < argc; i++){
        
        if(strcmp(argv[i], "-d") == 0){
            resttime = atoi(argv[i+1]);
        }

        if(strcmp(argv[i], "-s") == 0){
            shmid = argv[i+1];
        }

        if(strcmp(argv[i], "-l") == 0){
            // logging file
            char* loggingFile = argv[i+1];
            fdLogging = open(loggingFile, O_RDWR | O_APPEND, 0666);
            if(fdLogging == -1){
                printf("failure opening logging in visitor\n");
                exit(1);
            }

        }
    }

    // open the shared memory segment - only read/write flags since it has already been created
    int fd = shm_open(shmid, O_RDWR, 0666);
    if(fd == -1){
        printf("shm_open failure in visitor\n");
        exit(1);
    }

    // map the shared memory segment to the address space of the visitor
    struct sharedObjects* sharedData = mmap(NULL, sizeof(struct sharedObjects), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(sharedData == MAP_FAILED){
        printf("mmap failure in visitor\n");
        exit(1);
    }


    // ########################### //
    // ####### VISITOR CODE ###### //
    // ########################### //

    pid_t pid = getpid(); // get the pid of the visitor

    sem_wait(&sharedData->mutex); 

    //#### CRITICAL SECTION: adding visitor to waiting line

    addVisitorToWaitingLine(sharedData, resttime, fdLogging, pid);

    // write to logging file that a visitor has entered the waiting line
    char* message = " has entered the waiting line\n";
    writeToLog(fdLogging, message, pid);

    sem_wait(&sharedData->maxWaiting); // if it below 0, the visitor gets suspended since the waiting line is full
    
    sem_post(&sharedData->mutex); 
    //#### END OF CRITICAL SECTION



    

}


void addVisitorToWaitingLine(struct sharedObjects* sharedData, int restTime, int fdLogging, pid_t pid){
    
    // add visitor to the waiting line
    int count = sharedData->waitingLine.count; // initially 0, so first visitor is at index 0
    sharedData->waitingLine.buffer[count] = pid; // added pid 
    
    int res = sem_init(&sharedData->waitingLine.sems[count], 1, 0); // semaphore for the visitor
    if(res == -1){
        printf("sem_init failure in adding visitor\n");
        exit(1);
    }


    sharedData->waitingLine.count++; // increment the number of visitors waiting
    
    // if there is only one customer waiting in line, it is both the first and the last
    if(sharedData->waitingLine.count == 1){
        sharedData->waitingLine.first = 0; // position of the first visitor
        sharedData->waitingLine.last = 0;  // both at index 0
    }
    else{
        sharedData->waitingLine.last++; // increment the last visitor
    }

}


void writeToLog(int fd, char* message, pid_t pid){
    
    char pidStr[20];
    sprintf(pidStr, "%d", pid); // convert pid to string
    
    if(write(fd, pidStr, strlen(pidStr)) == -1){ // write the pid to the log file
        printf("writing to log failed\n");
        exit(1);
    }

    if (write(fd, message, strlen(message)) == -1){ // write the action taken by the visitor to the log file
        printf("writing to log failed\n");
        exit(1);
    }
}