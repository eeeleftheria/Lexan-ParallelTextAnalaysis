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
#include "monitor.h"



int main(int argc, char* argv[]){

    if(argc != 2){
        printf("Usage: ./monitor <shmid>\n");
        exit(1);
    }

    char* shmid = argv[1]; // name of the shared memory segment

    // Open the shared memory segment, only read/write flags since
    // it has already been created
    int fd = shm_open(shmid, O_RDWR, 0666);

    if(fd == -1){
        printf("shm_open failure in monitor\n");
        exit(1);
    }

    // Map the shared memory segment to the address space of the calling process
    struct sharedObjects* sharedData = mmap(NULL, sizeof(struct sharedObjects), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if(sharedData == MAP_FAILED){
        printf("mmap failure in monitor\n");
        exit(1);
    }

    sem_wait(&sharedData->mutex);

    // ###### print data of shared memory segment
    printf("Shared memory segment:\n");
    printStatistics(sharedData->stats);
    printTables(sharedData->tables);
    printWaitingLine(sharedData->waitingLine);
    printOrdersOrder(sharedData->ordersOrder);


    sem_post(&sharedData->mutex);

    // detach the shared memory segment
    if(munmap(sharedData, sizeof(struct sharedObjects)) == -1){
        printf("munmap failure in monitor\n");
        exit(1);
    }

    exit(0);

}


void printStatistics(statistics stats){

    printf("-----Statistics-----\n");
    printf("Total water drinks served: %d\n", stats.totalWaterDrinks);
    printf("Total wine drinks served: %d\n", stats.totalWineDrinks);
    printf("Total cheese plates served: %d\n", stats.totalCheesePlates);
    printf("Total salads served: %d\n", stats.totalSalads);
    printf("Total visitors served: %d\n", stats.totalVisitors);
    printf("Total visitors: %d\n", stats.totalVisitors);
    printf("Average waiting time: %f\n", stats.avgWaitTime);
    printf("Average stay time: %f\n", stats.avgStayTime);
    printf("Average serving time: %f\n\n", stats.avgServeTime);
}


void printTables(table tables[]){

    printf("-----Tables-----\n");

    for(int i = 0; i < MAX_TABLES; i++){

        printf("Table %d: ", i);

        if(tables[i].occupiedSeats == 0){ // if the table is empty
            printf("Empty\n");
        }

        else{

            for(int j = 0; j < 4; j++){ // for each chair print which visitor is seated
                
                if(tables[i].chairs[j].visitor != 0){
                    printf("%d in chair %d | ", tables[i].chairs[j].visitor, j);
                }
            }
            printf("\n");
            
        }
    printf("\n");
    }
}




void printWaitingLine(circularBuffer waitingLine){
    printf("-----Waiting Line-----\n");

    int count = waitingLine.count;
    
    if(waitingLine.count == 0){ // if no clients are waiting to enter the bar
        printf("No clients waiting\n\n");
        return;
    }
    
    printf("First: %d\n", waitingLine.first);
    printf("Last: %d\n", waitingLine.last);
    printf("Count: %d\n", waitingLine.count);
    
    // print the pids of the clients waiting
    printf("Pids: "); 
    for(int i = 0; i < count; i++){
        printf("%d ", waitingLine.buffer[i]);
    }

    printf("\n\n");
}


void printOrdersOrder(circularOrders ordersOrder){
    printf("-----Orders Line-----\n");
    
    if(ordersOrder.count == 0){ // if there are no orders
        printf("No orders\n\n");
        return;
    }
    
    printf("First order: %d\n", ordersOrder.first);
    printf("Last order: %d\n", ordersOrder.last);
    printf("Total orders: %d\n\n", ordersOrder.count);

    int first = ordersOrder.first;
    int last = ordersOrder.last;
    
    for(int i = first; i <= last; i++){
            
        printf("Order %d of %d: ", i, ordersOrder.buffer[i].visitor_id);
        
        int numOfItems = ordersOrder.buffer[i].count;
        printf("Number of items: %d\n", numOfItems);
        for(int i = 0; i < numOfItems; i++){

            if(ordersOrder.buffer[i].items[i] == -1){
                continue;
            }

            if(ordersOrder.buffer[i].items[i] == 0){
                printf("Water | ");
            }
            else if(ordersOrder.buffer[i].items[i] == 1){
                printf("Wine | ");
            }
            else if(ordersOrder.buffer[i].items[i] == 2){
                printf("Cheese | ");
            }
            else if(ordersOrder.buffer[i].items[i] == 3){
                printf("Salad | ");
            }
      
        }
    
    printf("\n\n");
    }
}

void printSemaphores(struct sharedObjects* sharedData){
    printf("Semaphores' values:\n");
    int sem_value;
    sem_getvalue(&sharedData->mutex, &sem_value);
    printf("Mutex: %d\n", sem_value);
    sem_getvalue(&sharedData->receptionist, &sem_value);
    printf("Receptionist: %d\n", sem_value);
    sem_getvalue(&sharedData->maxWaiting, &sem_value);
    printf("Number of available seats in waiting line: %d\n", sem_value);
    printf("\n");
}