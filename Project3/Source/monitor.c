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
    printf("Water: %d\n", stats.totalWaterDrinks);
    printf("Wine: %d\n", stats.totalWineDrinks);
    printf("Cheese: %d\n", stats.totalCheesePlates);
    printf("Salads: %d\n", stats.totalSalads);
    printf("Total visitors: %d\n", stats.totalVisitors);
    printf("Total visitors served: %d\n", stats.totalVisitorsServed);

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
    
    if(waitingLine.count == 0){ // if no clients are waiting to enter the bar
        printf("No clients waiting\n\n");
        return;
    }
    
    printf("First: %d\n", waitingLine.first);
    printf("Last: %d\n", waitingLine.last);
    printf("Count: %d\n", waitingLine.count);
    
    // print the pids of the clients waiting
    int first = waitingLine.first;
    printf("Pids: "); 

    for(int i = 0; i < waitingLine.count; i++){
        printf("%d ", waitingLine.buffer[first]);
        first = (first + 1) % MAX_WAITING;

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

    int curr = ordersOrder.first;

    for(int i = 0; i < ordersOrder.count; i++){    

        printf("Order %d: ", i);
        printf("Visitor: %d | ", ordersOrder.buffer[curr].visitor_id);

        for(int j = 0; j < MAX_NUM_OF_ITEMS_PER_ORDER; j++){
            if(ordersOrder.buffer[curr].items[j]  == WATER){
                printf("Water ");
            }
            else if(ordersOrder.buffer[curr].items[j] == WINE){
                printf("Wine ");
            }
            else if(ordersOrder.buffer[curr].items[j] == CHEESE){
                printf("Cheese ");
            }
            else if(ordersOrder.buffer[curr].items[j] == SALAD){
                printf("Salad ");
            }
        }

        curr = (curr + 1) % MAX_ORDERS;
        printf("\n");
    }
  
}
