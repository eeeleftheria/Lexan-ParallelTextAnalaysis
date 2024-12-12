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

    char* shmid = argv[1];

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

    // ###### print shared memory segment
    printf("Shared memory segment:\n");
    printStatistics(sharedData->stats);
    printTables(sharedData->tables);
    printOrders(sharedData->orders);
    printWaitingLine(sharedData->waitingLine);
    printOrdersOrder(sharedData->ordersOrder);

    
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
    printf("Average waiting time: %f\n", stats.avgWaitTime);
    printf("Average stay time: %f\n", stats.avgStayTime);
    printf("Average serving time: %f\n\n", stats.avgServeTime);
}

void printTables(table tables[]){
    printf("-----Tables-----\n");

    for(int i = 0; i < MAX_TABLES; i++){

        printf("Table %d: ", i);

        if(tables[i].occupiedSeats == 0){
            printf("Empty\n");
        }
        else{

            for(int j = 0; j < MAX_CHAIRS; j++){
                printf("%d in chair %d | ", tables[i].chairs[j].visitor, j);
            }
            printf("\n");
            
            printf("Is full: %d\n", tables[i].is_full);
        }
    printf("\n");
    }
}

void printOrders(order orders[MAX_TABLES][MAX_CHAIRS]){
    printf("-----Orders-----\n");
    
    for(int i = 0; i < MAX_TABLES; i++){
    
        printf("Table %d:\n", i);


        for(int j = 0; j < MAX_CHAIRS; j++){
    
            printf("    Chair %d: ", j);
            printf("visitor id: %d | ", orders[i][j].visitor_id);
            printf("Items: ");

            int count = orders[i][j].count;

            for(int k = 0; k < count; k++){
                printf("%d ", orders[i][j].items[k]);
            }
            printf("\n");
        }
    }
    printf("\n");
}

void printWaitingLine(circularBuffer waitingLine){
    printf("-----Waiting Line-----\n");
    
    if(waitingLine.count == 0){
        printf("No clients waiting\n\n");
        return;
    }
    
    printf("First: %d\n", waitingLine.first);
    printf("Last: %d\n", waitingLine.last);
    printf("Count: %d\n", waitingLine.count);
    

    printf("Pids: ");
    for(int i = 0; i < MAX_WAITING; i++){
        printf("%d ", waitingLine.buffer[i]);
    }
    printf("\n\n");
}

void printOrdersOrder(circularOrders ordersOrder){
    printf("-----Orders Line-----\n");
    
    if(ordersOrder.count == 0){
        printf("No orders\n\n");
        return;
    }
    
    printf("First: %d\n", ordersOrder.first);
    printf("Last: %d\n", ordersOrder.last);
    printf("Count: %d\n", ordersOrder.count);
    

    printf("Orders: ");
    for(int i = 0; i < MAX_ORDERS; i++){
        printf("%d ", ordersOrder.buffer[i].visitor_id);
    }
    printf("\n\n");
}