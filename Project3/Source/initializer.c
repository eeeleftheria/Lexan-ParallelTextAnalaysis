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
#include "InitializatonFuncs.h"
#include <string.h>
#include <sys/wait.h>


#define SHARED_MEMORY_NAME "/sharedMemory" // name of the shared memory segment
#define NUM_OF_VISITORS 100 // number of visitor processes to be created

int main(int argc, char* argv[]){
    
    float orderTime; // max time for the receptionist to take an order
    float restTime; // max duration of a visitor's stay in sleep mode after he gets served
    char* loggingFile; // name of the logging file
    int openTime; // time the bar will remain open
    
    pid_t* receptPid = malloc(sizeof(pid_t)); // pid of the receptionist process
    pid_t* visitorPids = malloc(NUM_OF_VISITORS * sizeof(pid_t)); // pids of the visitor processes
    pid_t* closingPid = malloc(sizeof(pid_t)); // pid of the closing process

    if(argc != 9){
        printf("Usage: ./main -o <orderTime> -r <restTime> -l <LoggingFile> -t <openTime>\n");
        exit(1);
    }

    for(int i = 0; i < argc; i++){
        
        if(strcmp(argv[i], "-o") == 0){
            orderTime = atof(argv[i+1]);
        }

        else if(strcmp(argv[i], "-r") == 0){
            restTime = atof(argv[i+1]);
        }

        else if(strcmp(argv[i], "-l") == 0){
            loggingFile = argv[i+1];
        }
        else if(strcmp(argv[i], "-t") == 0){
            // time the bar will remain open
            openTime = atoi(argv[i+1]);
        }
    }



    // Creation of shared memory segment
    // arguments:   
    //      name: sharedMemory
    //      flags: O_CREAT | O_RDWR for creation if it does not exist and read/write
    //      rights: 0666 for read/write for all users
    int fd = shm_open("/sharedMemory", O_CREAT | O_RDWR, 0666);

    if(fd == -1){
        printf("shm_open failure"); 
        exit(1);
    }

    // Change the size of the shared memory segment to be as much as the struct's sharedObjects
    // only one process does this
    if( ftruncate(fd, sizeof(struct sharedObjects))  == -1){
        printf("ftruncate failure");
        exit(1);
    }

    // Map the shared memory segment to the address space of the calling process
    struct sharedObjects* sharedData = mmap(NULL, sizeof(struct sharedObjects), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(sharedData == MAP_FAILED){
        printf("mmap failure");
        exit(1);
    }

    printf("Shared memory segment created and mapped successfully\n");



    // ####### INITIALIZATION OF SHARED MEMORY #######
    initSharedMemory(sharedData);

    // ####### INITIALIZATION OF LOGGING FILE #######
    createLoggingFile(loggingFile);

    // ####### FORKING OF PROCESSES #######
    
    // creates and executes a closing process
    createClosingProcess(SHARED_MEMORY_NAME, openTime, closingPid, loggingFile);

    // creation of receptionist process
    createRecept(orderTime, SHARED_MEMORY_NAME, loggingFile, receptPid);

    // Creation of visitor processes
    createVisitor(NUM_OF_VISITORS, restTime, SHARED_MEMORY_NAME, loggingFile, visitorPids);


    // ######## SYNCHRONIZATION ########

    int status;
   
    if(waitpid(*closingPid, &status, 0) == -1){
        perror("waitpid failure in closing");
        exit(1);
    }

    for(int i = 0; i < NUM_OF_VISITORS; i++){
        
        if(waitpid(visitorPids[i], &status, 0) == -1){
            perror("waitpid failure in visitor");
            exit(1);
        }
    }

    if(waitpid(*receptPid, &status, 0) == -1){
        perror("waitpid failure in receptionist");
        exit(1);
    }


    // ####### FREE DYNAMICALLY ALLOCATED MEMORY #######
    free(receptPid);
    free(visitorPids);
    free(closingPid);


    sleep(10);
   
   
    // ####### DETAHCING SHARED MEMORY #######
    if(munmap(sharedData, sizeof(struct sharedObjects)) == -1){
        printf("munmap failure in monitor\n");
        exit(1);
    }



}


// ############################ //
// ######### FUNCTIONS ######## //
// ############################ //

void initSharedMemory(struct sharedObjects* sharedData){

    sharedData->isClosing = false;
    
    int res;

    // ####### Initialization of semaphores
    res = sem_init(&sharedData->mutex, 1, 1); // initially unlocked so the first process can enter its critical section
    if(res == -1){
        perror("sem_init mutex failed");
        exit(1);
    }

    res = sem_init(&sharedData->receptionist, 1, 0); // the receptionist is initially asleep 
    // till he is woken up by a client.
    if(res == -1){
        perror("sem_init recept failed");
        exit(1);
    }


    // max number of waiting visitors, if it falls below 0 the visitor cannot enter the waiting queue
    // and get suspended on the semaphore
    res = sem_init(&sharedData->maxWaiting, 1, MAX_WAITING);
    if(res == -1){
        perror("sem_init maxWaiting failed");
        exit(1);
    } 
    

    // ####### Initialization of array of tables
    for(int i = 0; i < MAX_TABLES; i++){
        
        sharedData->tables[i].is_full = false; // each table is initially empty
        sharedData->tables[i].occupiedSeats = 0; // no visitor has sat on the table yet
        
        for(int j = 0; j < MAX_CHAIRS; j++){

            sem_init(&sharedData->tables[i].sems[j], 1, 0); // semaphore for each chair where the visitor gets suspended
            // while waiting for his order  
            
            sharedData->tables[i].chairs[j].visitor = -1; // no visitor has sat yet
        }

    }


    // ####### Initialization of circular buffers

    // no visitors yet
    sharedData->waitingLine.first = 0;
    sharedData->waitingLine.last = 0;
    sharedData->waitingLine.count = 0;

    for(int i = 0; i < MAX_WAITING; i++){
        
        res = sem_init(&sharedData->waitingLine.sems[i], 1, 0); /// each visitor in the queue is initially asleep
        // while there is no place to sit in the bar
        
        if(res == -1){
            perror("sem_init waitingLine failed");
            exit(1);
        }

        sharedData->waitingLine.buffer[i] = -1; // no visitor has entered the bar yet
    }

    // no orders yet
    sharedData->ordersOrder.first = 0;
    sharedData->ordersOrder.last = 0;
    sharedData->ordersOrder.count = 0;
    
    for(int i = 0; i < MAX_ORDERS; i++){
        sharedData->ordersOrder.buffer[i].visitor_id = -1; // no visitor has placed an order yet
        sharedData->ordersOrder.buffer[i].count = 0; // no items in the order yet

        for(int j = 0; j < MAX_NUM_OF_ITEMS_PER_ORDER; j++){
            sharedData->ordersOrder.buffer[i].items[j] = -1; // no item in the order yet
        }
    }

    

    // ####### Initialization of Statistics
    sharedData->stats.totalWaterDrinks = 0;
    sharedData->stats.totalWineDrinks = 0;
    sharedData->stats.totalCheesePlates = 0;
    sharedData->stats.totalSalads = 0;
    sharedData->stats.totalVisitors = 0;
    sharedData->stats.totalVisitorsServed = 0;
    sharedData->stats.avgWaitTime = 0;
    sharedData->stats.avgStayTime = 0;
}



void createRecept(float orderTime, char* shmid, char* loggingFile, pid_t* receptPid){
    
    // forking a receptionist process
    pid_t pid = fork();
    
    if(pid == -1){
        perror("forking receptionist failed");
        exit(1);
    }

    // inside of receptionist process
    if(pid == 0){
        char orderTimeStr[10];
        snprintf(orderTimeStr, sizeof(orderTimeStr), "%f", orderTime);

        execlp("receptionist", "receptionist", "-d", orderTimeStr, "-s", shmid, "-l", loggingFile, NULL);
        perror("execlp receptionist failed");
    }
    else{
        *receptPid = pid; // store the pid of the receptionist process
    }
}



void createVisitor(int numOfVisitors, float restTime, char* shmid, char* loggingFile, pid_t visitorPids[]){
    
    // forking numOfVisitors processes
    for(int i = 0; i < numOfVisitors; i++){
        
        pid_t pid = fork();
        
        if(pid == -1){
            perror("forking visitor failed");
            exit(1);
        }
        
        // inside of visitor process
        if(pid == 0){
            char restTimeStr[10];
            snprintf(restTimeStr, sizeof(restTimeStr), "%f", restTime);

            execlp("visitor", "visitor", "-d", restTimeStr, "-s", shmid, "-l", loggingFile, NULL);
            perror("execlp visitor failed");
        }
        else{
            visitorPids[i] = pid; // store the pid of the visitor process
        }
    }

}


void createLoggingFile(char* loggingFile){
    
    // open the file for writing - create if it does not exist
    int fd = open(loggingFile, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if(fd == -1){
        printf("opening logging file failed\n");
        exit(1);
    }

    char* message = "Logging file of the bar:\n";
    
    ssize_t bytesWritten = write(fd, message, strlen(message));
    if(bytesWritten == -1){
        printf("writing to logging file failed\n");
        exit(1);
    }

    // close the file
    if(close(fd) == -1){
        printf("closing logging file failed\n");
        exit(1);
    }
}   


void createClosingProcess(char* shmid, int openTimeOfBar, pid_t* closingPid, char* loggingFile){
    
    // forking a closing process
    pid_t pid = fork();
    
    if(pid == -1){
        perror("forking closing process failed");
        exit(1);
    }

    // inside of closing process
    if(pid == 0){
        char openTimeOfBarStr[10];
        snprintf(openTimeOfBarStr, sizeof(openTimeOfBarStr), "%d", openTimeOfBar);

        execlp("closing", "closing", "-s", shmid, "-t", openTimeOfBarStr, "-l", loggingFile, NULL);
        perror("execlp closing failed");
    }

    else{
        *closingPid = pid; // store the pid of the closing process
    }
}
