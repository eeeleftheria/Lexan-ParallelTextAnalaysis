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
#include <stdbool.h>
#include "receptionist.h"
#include "visitor.h"
#include <time.h>


int main(int argc, char* argv[]){

    srand(time(NULL)^getpid());

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

    while(1){
        
        sem_wait(&sharedData->receptionist); // if no customers, suspend the receptionist -> sem = -1
        
        // if he is woken up, dicrement the mutex so only one process is in critical section
        sem_wait(&sharedData->mutex);


        if(sharedData->isClosing == true){
     
            if(checkIfBarIsEmpty(sharedData) == true && sharedData->waitingLine.count == 0){
                printf("Bar is empty\n");
                closeBar(sharedData, fdLogging, fd, shmid);

                sem_post(&sharedData->receptionist);
                sem_post(&sharedData->mutex);
                break;
            }

            // if it is not yet empty continue with the orders
        }
    

        // ##### CLOSING
        // if the bar is closing, the receptionist should not take any more orders
        // wait till all customeres leave

        // #####  ORDERS
        // 1. check if there are any orders
        if(sharedData->ordersOrder.count > 0){
            
            // take the first order
            order currentOrder = sharedData->ordersOrder.buffer[sharedData->ordersOrder.first];
            
            // update the statistics
            updateStats(sharedData, currentOrder, fdLogging); 

            // prepare the order for a random time between [0.5 * torder, torder]
            float minTime = 0.5 * ordertime;
            int maxTime = (int)ordertime;
            float actualTime = rand() % maxTime + minTime;

            char message[100];
            sprintf(message, "ORDER PREPARATION: Order of %d is being prepared for %f time\n\n", currentOrder.visitor_id, actualTime);
            write(fdLogging, message, strlen(message));

            sem_post(&sharedData->mutex); 
           
            sleep(actualTime); // sleep for the time it takes to prepare the order
           
            sem_wait(&sharedData->mutex); 

            // update the circular buffer storing the orders
            sharedData->ordersOrder.count--; // current order is served

            // % MAX_ORDERS so it is in the bounds of the buffer & it is circular
            // for example: buffer[4], if first = 3, after it is consumed new first is (3+1) % 4 = 0
            sharedData->ordersOrder.first = (sharedData->ordersOrder.first + 1) % MAX_ORDERS; // move to the next order

            sharedData->stats.totalVisitorsServed++; // increment the number of visitors served

            // calculate the average serving time
            float avgServeTime = sharedData->stats.avgServeTime;
            int totalVisitorsServed = sharedData->stats.totalVisitorsServed;
            sharedData->stats.avgServeTime = (avgServeTime + actualTime) / totalVisitorsServed;

            // write to logging file that the order is ready
            sprintf(message, "ORDER READY: Order of %d was served\n\n", currentOrder.visitor_id);
            write(fdLogging, message, strlen(message));
            
            // searching for the table and chair of the visitor
            int tableNum = findTableOfVisitor(sharedData, currentOrder.visitor_id);
            int chairNum = findChairOfVisitor(sharedData, currentOrder.visitor_id);

            // the recept wakes up the visitor when his order is ready
            sem_post(&sharedData->tables[tableNum].sems[chairNum]); // wake up the visitor

            sem_post(&sharedData->mutex);

        }

        // if there are no orders, continue to loop
        else{
            sem_post(&sharedData->mutex);
            continue;
        }

    } 
 

}


void updateStats(struct sharedObjects* sharedData, order currentOrder, int fdLogging){
    
    for(int i = 0; i < MAX_NUM_OF_ITEMS_PER_ORDER; i++){
                
        if(currentOrder.items[i] == WATER){
            sharedData->stats.totalWaterDrinks++;
        }
        if(currentOrder.items[i] == WINE){
            sharedData->stats.totalWineDrinks++;
        }
        if(currentOrder.items[i] == CHEESE){
            sharedData->stats.totalCheesePlates++;
        }
        if(currentOrder.items[i] == SALAD){
            sharedData->stats.totalSalads++;
        }

    }
}


int findChairOfVisitor(struct sharedObjects* sharedData, pid_t pid){
    
    for(int i = 0; i < MAX_TABLES; i++){
        
        for(int j = 0; j < MAX_CHAIRS; j++){
            
            // if the visitor is seated at chair j of table i
            if(sharedData->tables[i].chairs[j].visitor == pid){
                return j;
            }
        }
    }

    return -1;
}


int findTableOfVisitor(struct sharedObjects* sharedData, pid_t pid){
    
    for(int i = 0; i < MAX_TABLES; i++){
        
        // if the visitor is seated at table i
        for(int j = 0; j < MAX_CHAIRS; j++){
        
            if(sharedData->tables[i].chairs[j].visitor == pid){
                return i; // return the table number
            }
        }
    }

    // no visitor found at any table
    return -1;
}


bool checkIfBarIsEmpty(struct sharedObjects* sharedData){
    
    for(int i = 0; i < MAX_TABLES; i++){
        
        // if there is a least one table with occupied seats, then the bar is not empty
        if(sharedData->tables[i].occupiedSeats > 0){
            return false;
        }
    }

    // if no table has occupied seats, the bar is empty
    return true;
}


void closeBar(struct sharedObjects* sharedData, int fdLogging, int fdSharedMem, char* shmid){
    
    // write the final statistics to the logging file
    writeStats(sharedData, fdLogging);

    // ##### DESTROY SEMAPHORES
    if(sem_destroy(&sharedData->mutex) == -1){
        printf("destroying mutex failure in receptionist\n");
        exit(1);
    }

    if(sem_destroy(&sharedData->receptionist) == -1){
        printf("destroying receptionist failure in receptionist\n");
        exit(1);
    }

    if(sem_destroy(&sharedData->maxWaiting) == -1){
        printf("destroying maxWaiting failure in receptionist\n");
        exit(1);
    }


    for(int i = 0; i < MAX_TABLES; i++){
        
        for(int j = 0; j < MAX_CHAIRS; j++){
            
            if(sem_destroy(&sharedData->tables[i].sems[j]) == -1){
            
                printf("destroying table sems failure in receptionist\n");
            
                exit(1);
            }
        }
    }

    for(int i = 0; i < MAX_WAITING; i++){
        
        if(sem_destroy(&sharedData->waitingLine.sems[i]) == -1){
        
            printf("destroying waitingLine sems failure in receptionist\n");
           
            exit(1);
        }
    }

    // #### DETACH SHARED MEMORY SEGMENT

    if(munmap(sharedData, sizeof(struct sharedObjects)) == -1){
        printf("munmap failure in receptionist\n");
        exit(1);
    }

    // close the shared memory segment
    close(fdSharedMem);

    // ##### UNLINK SHARED MEMORY
    // ONLY the receptionist unlinks the shared memory
    if(shm_unlink(shmid) == -1){
        printf("shm_unlink failure in receptionist\n");
        exit(1);
    }

    exit(0);
}



void writeStats(struct sharedObjects* sharedData, int fdLogging){
    
    write(fdLogging, "-------------------------------", strlen("-------------------------------"));
    write(fdLogging, "\nFinal Statistics:\n", strlen("\nFinal Statistics:\n"));
    printf("-------------------------------\n");
    printf("Final Statistics:\n");
    
    char message[100];
    sprintf(message, "Total water drinks served: %d\n", sharedData->stats.totalWaterDrinks);
    write(fdLogging, message, strlen(message));
    printf("Total water drinks served: %d\n", sharedData->stats.totalWaterDrinks);

    sprintf(message, "Total wine drinks served: %d\n", sharedData->stats.totalWineDrinks);
    write(fdLogging, message, strlen(message));
    printf("Total wine drinks served: %d\n", sharedData->stats.totalWineDrinks);

    sprintf(message, "Total cheese plates served: %d\n", sharedData->stats.totalCheesePlates);
    write(fdLogging, message, strlen(message));
    printf("Total cheese plates served: %d\n", sharedData->stats.totalCheesePlates);

    sprintf(message, "Total salads served: %d\n", sharedData->stats.totalSalads);
    write(fdLogging, message, strlen(message));
    printf("Total salads served: %d\n", sharedData->stats.totalSalads);

    sprintf(message, "Total visitors that entered the bar: %d\n", sharedData->stats.totalVisitors);
    write(fdLogging, message, strlen(message));
    printf("Total visitors that entered the bar: %d\n", sharedData->stats.totalVisitors);

    sprintf(message, "Total visitors served: %d\n", sharedData->stats.totalVisitorsServed);
    write(fdLogging, message, strlen(message));
    printf("Total visitors served: %d\n", sharedData->stats.totalVisitorsServed);

    sprintf(message, "Average waiting time: %f\n", sharedData->stats.avgWaitTime);
    write(fdLogging, message, strlen(message));
    printf("Average waiting time: %f\n", sharedData->stats.avgWaitTime);

    sprintf(message, "Average stay time: %f\n", sharedData->stats.avgStayTime);
    write(fdLogging, message, strlen(message));
    printf("Average stay time: %f\n", sharedData->stats.avgStayTime);

    sprintf(message, "Average serving time: %f\n", sharedData->stats.avgServeTime);
    write(fdLogging, message, strlen(message));
    printf("Average serving time: %f\n", sharedData->stats.avgServeTime);

    close(fdLogging);
}
