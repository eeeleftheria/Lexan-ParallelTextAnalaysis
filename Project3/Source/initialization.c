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


#define SHARED_MEMORY_NAME "/sharedMemory" // name of the shared memory segment
#define NUM_OF_VISITORS 2 // number of visitor processes to be created

int main(int argc, char* argv[]){
    
    float orderTime; // max time for the receptionist to take an order
    float restTime; // max duration of a visitor's stay in sleep mode after he gets served

    if(argc != 4){
        printf("Usage: ./main <orderTime> <restTime> <LoggingFile>\n");
        exit(1);
    }

    orderTime = atof(argv[1]);
    restTime = atof(argv[2]);
    char* loggingFile = argv[3];


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

    // Initialize the shared memory segment
    initSharedMemory(sharedData);

    // Initialize logging file with all the actions of the clients and the receptionist
    createLoggingFile(loggingFile);


    //######## Creation of receptionist process
    
    // createRecept(orderTime, SHARED_MEMORY_NAME);


    // Creation of visitor processes

    createVisitor(NUM_OF_VISITORS, restTime, SHARED_MEMORY_NAME, loggingFile);

    sleep(10);
   
   
    // detach the shared memory segment
    if(munmap(sharedData, sizeof(struct sharedObjects)) == -1){
        printf("munmap failure in monitor\n");
        exit(1);
    }

    // destroy the shared memory segment: ONLY ONE PROCESS DOES THIS
    if(shm_unlink(SHARED_MEMORY_NAME) == -1){
        printf("shm_unlink failure in monitor\n");
        exit(1);
    }
 



}


// ############################ //
// ######### FUNCTIONS ######## //
// ############################ //

void initSharedMemory(struct sharedObjects* sharedData){
    
    int res;

    // ####### Initialization of semaphores
    res = sem_init(&sharedData->mutex, 1, 1);
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
    res = sem_init(&sharedData->maxWaiting, 1, MAX_WAITING);
    if(res == -1){
        perror("sem_init maxWaiting failed");
        exit(1);
    } 
    

    // ####### Initialization of arrays
    for(int i = 0; i < MAX_TABLES; i++){
        
        sharedData->tables[i].is_full = false;
        sharedData->tables[i].occupiedSeats = 0; // no visitor has sat on the table yet
        
        for(int j = 0; j < MAX_CHAIRS; j++){
            sharedData->tables[i].chairs[j].visitor = -1; // no visitor has sat yet
        }
    }


    for(int i = 0; i < MAX_TABLES; i++){
        
        for(int j = 0; j < MAX_CHAIRS; j++){
            sharedData->orders[i][j].visitor_id = -1; // no visitor has ordered anything yet
            
            for(int k = 0; k < MAX_NUM_OF_ITEMS_PER_ORDER; k++){
                sharedData->orders[i][j].items[k] = -1; // no item has been ordered yet
            }

            sharedData->orders[i][j].count = 0; // no items have been ordered yet
        }
    }

    // ####### Initialization of circular buffers
    circularBuffer waitingLine = sharedData->waitingLine;
    // no visitors yet
    waitingLine.first = 0;
    waitingLine.last = 0;
    waitingLine.count = 0;

    for(int i = 0; i < MAX_WAITING; i++){
        
        res = sem_init(&waitingLine.sems[i], 1, 0); ///??????????????????
        
        if(res == -1){
            perror("sem_init waitingLine failed");
            exit(1);
        }
    }

    circularOrders ordersOrder = sharedData->ordersOrder;
    // no orders yet
    ordersOrder.first = 0;
    ordersOrder.last = 0;
    ordersOrder.count = 0;

    for(int i = 0; i < MAX_ORDERS; i++){
        
        res = sem_init(&ordersOrder.sems[i], 1, 0); // each order is set to 0 and only the first in the queue will be woken up
        
        if(res == -1){
            perror("sem_init ordersOrder failed");
            exit(1);
        }
    }


    // ####### Initialization of Statistics
    sharedData->stats.totalWaterDrinks = 0;
    sharedData->stats.totalWineDrinks = 0;
    sharedData->stats.totalCheesePlates = 0;
    sharedData->stats.totalSalads = 0;
    sharedData->stats.totalVisitors = 0;
    sharedData->stats.avgWaitTime = 0;
    sharedData->stats.avgStayTime = 0;
}



void createRecept(float orderTime, char* shmid, char* loggingFile){
    
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
    }
}



void createVisitor(int numOfVisitors, float restTime, char* shmid, char* loggingFile){
    
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
