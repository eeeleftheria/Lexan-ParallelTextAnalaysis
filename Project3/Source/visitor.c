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
#include <time.h>

int main(int argc, char* argv[]){

    srand(time(NULL) + getpid());

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
        close(fdLogging);
        exit(1);
    }

    // map the shared memory segment to the address space of the visitor
    struct sharedObjects* sharedData = mmap(NULL, sizeof(struct sharedObjects), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(sharedData == MAP_FAILED){
        printf("mmap failure in visitor\n");
        close(fdLogging);
        exit(1);
    }


    // ########################### //
    // ####### VISITOR CODE ###### //
    // ########################### //

    pid_t pid = getpid(); // get the pid of the visitor

    sem_wait(&sharedData->mutex); 

    //#### VISITOR ENTERS WAITING LINE
    sem_wait(&sharedData->maxWaiting); // if it below 0, the visitor gets suspended since the waiting line is full

    // if the bar is closing, the cient should leave
    if(sharedData->isClosing == true){
        sem_post(&sharedData->maxWaiting); // free the semaphore
        sem_post(&sharedData->mutex); // free the mutex

        // detach the shared memory segment
        if(munmap(sharedData, sizeof(struct sharedObjects)) == -1){
            printf("munmap failure in monitor\n");
            close(fdLogging);
            exit(1);
        }

        close(fd);
        close(fdLogging);

        exit(0);
    }
    
    // 1. check if the waiting line was empty: 2 cases
    if(sharedData->waitingLine.count == 0){
        
        // the visitor is both the first & last of the queue
        sharedData->waitingLine.first = 0;
        sharedData->waitingLine.last = 0;
        sharedData->waitingLine.buffer[0] = pid;
        sharedData->waitingLine.count++;

        // the visitor checks himself wether he can have a seat at the bar
        
        // 1a) if there is space, he sits and places an order
        if(checkForTable(sharedData, fdLogging, pid) == true){
            
            placeOrder(sharedData, fdLogging, pid);

            stayInBar(sharedData, fdLogging, pid, resttime);
        }

        // 1b) if there is no space, he remains in the waiting line and waits for a table
        else{
            // write to logging file that a visitor has entered the waiting line
            char message[100];
            sprintf(message, "%d has entered the waiting line\n", pid);
            write(fdLogging, message, strlen(message));

            sem_post(&sharedData->mutex); // free the mutex
            
            sem_wait(&sharedData->waitingLine.sems[0]); // wait for someone to wake him up when there is a free table
        }
    
    }
    
    // 2. if the waiting line is not empty, add visitor to it 
    else{
        sharedData->waitingLine.last = (sharedData->waitingLine.last + 1) % MAX_WAITING;
        sharedData->waitingLine.buffer[sharedData->waitingLine.last] = pid;
        sharedData->waitingLine.count++;

        char message[100];
        sprintf(message, "%d has entered the waiting line\n", pid);
        write(fdLogging, message, strlen(message));

        sem_post(&sharedData->mutex); // free the mutex
        
        sem_wait(&sharedData->waitingLine.sems[0]); // wait for someone to wake him up when there is a free table

    }


    // detach the shared memory segment
    if(munmap(sharedData, sizeof(struct sharedObjects)) == -1){
        printf("munmap failure in monitor\n");
        close(fdLogging);
        exit(1);
    }

    close(fd);
    close(fdLogging);

    exit(0);


}

// if there is a table that is not full, the visitor enters and the function returns true
// otherwise, the function returns false
bool checkForTable(struct sharedObjects* sharedData, int fdLogging, pid_t pid){
    
    // the visitor checks himself whether he can have a seat at the bar
    for(int i = 0; i < MAX_TABLES; i++){

        // if there is a table that is not full, the visitor can enter
        if(sharedData->tables[i].is_full == false){

            // the visitor is added to the table
            sharedData->tables[i].chairs[sharedData->tables[i].occupiedSeats].visitor = pid;
            sharedData->tables[i].occupiedSeats++;

            char message[100];
            sprintf(message,"%d had a seat at table %d\n" ,pid, i);
            write(fdLogging, message, strlen(message));


            // if all 4 seats are occupied, the table is now full & 
            // is_full becomes false, only if all of the 4 visitors leave
            if(sharedData->tables[i].occupiedSeats == 4){
                sharedData->tables[i].is_full = true;

                char message[100];
                sprintf(message,"Table %d is now full\n", i);
                write(fdLogging, message, strlen(message));
            }

            // new visitor is added to the statistics
            sharedData->stats.totalVisitors++;

            // remove visitor from waiting line
            // since the buffer is now empty, we do not need to declare a new first & last
            sharedData->waitingLine.count--; 
            sharedData->waitingLine.buffer[sharedData->waitingLine.first] = 0; // removal of visitor's pid from the buffer
            
            sem_post(&sharedData->maxWaiting); // one less visitor in the waiting line         
            
            return true;
        }
    }

    return false;
}


void placeOrder(struct sharedObjects* sharedData, int fdLogging, pid_t pid){
    
    order newOrder;
    newOrder.visitor_id = pid;

    newOrder.count = 0;

    // minimum 1 item, maximum 4 items
    int minDrinks = 1; //if he only gets a drink
    int maxDrinks = 2; // if he gets the whole menu
    int numOfDrinks = rand() % maxDrinks + minDrinks; // number of drinks the visitor will order

    if(numOfDrinks == 1){
        
        newOrder.count++;

        int drink = rand() % 2; // 0 for water or 1 for wine
        
        if(drink == 0){
            newOrder.items[0] = WATER;
            newOrder.items[1] = -1;
        }
        else{
            newOrder.items[1] = WINE;
            newOrder.items[0] = -1;
        }
    }
    else if(numOfDrinks == 2){

        newOrder.count += 2;

        newOrder.items[0] = WATER;
        newOrder.items[1] = WINE;
    }

    int minFood = 0;
    int maxFood = 2;
    int numOfFood = rand() % maxFood + minFood; // number of food items the visitor will order

    if(numOfFood == 0){
        // no food
    }

    else if(numOfFood == 1){

        int food = rand() % 2; // 0 for cheese or 1 for salad
        
        if(food == 0){
            newOrder.items[2] = CHEESE;
            newOrder.items[3] = -1;
        }
        else{
            newOrder.items[3] = SALAD;
            newOrder.items[2] = -1;
        }

        newOrder.count++;
    }

    else if(numOfFood == 2){

        newOrder.items[2] = CHEESE;
        newOrder.items[3] = SALAD;
        
        newOrder.count += 2;
    }

    // add the order to the circular buffer
    if(sharedData->ordersOrder.count == 0){
        sharedData->ordersOrder.first = 0;
        sharedData->ordersOrder.last = 0;
        sharedData->ordersOrder.buffer[sharedData->ordersOrder.last] = newOrder;

    }
    else{
        sharedData->ordersOrder.last = (sharedData->ordersOrder.last + 1) % MAX_ORDERS;
        sharedData->ordersOrder.buffer[sharedData->ordersOrder.last] = newOrder;
        
    }

    sharedData->ordersOrder.count++;
    
    // write to logging file that a visitor has placed an order
    char message[100];
    sprintf(message, "%d ordered %d drinks and %d plates\n", pid, numOfDrinks, numOfFood);
    write(fdLogging, message, strlen(message));

    sem_post(&sharedData->receptionist); // wake up the receptionist to take the order

    int pos = findChairOfVisitor(sharedData, pid);

    // ???????????? !!!!!!!!!!!!!!!!!!!!!!!!
    sem_post(&sharedData->mutex); // free mutex

    sem_wait(&sharedData->tables[0].sems[pos]); // wait for the order to be ready
}


int findChairOfVisitor(struct sharedObjects* sharedData, pid_t pid){
    
    for(int i = 0; i < MAX_TABLES; i++){
        
        for(int j = 0; j < MAX_CHAIRS; j++){
        
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
                return i;
            }
        }
    }

    // no visitor found at any table
    return -1;
}

void stayInBar(struct sharedObjects* sharedData, int fdLogging, pid_t pid, float restTime){
    
    sem_wait(&sharedData->mutex);

    float minTime = 0.7 * restTime;
    float maxTime = restTime;

    float actualTime = rand() % (int)maxTime + minTime;

    char message[100];
    sprintf(message, "%d eats and converses for %f time\n", pid, actualTime);
    write(fdLogging, message, strlen(message));

    sem_post(&sharedData->mutex);
    
    sleep(actualTime);

    sem_wait(&sharedData->mutex);

    char message2[100];
    sprintf(message2, "%d left the bar\n", pid);
    write(fdLogging, message2, strlen(message2));

    // remove visitor from the table
    int chairNum = findChairOfVisitor(sharedData, pid);
    int tableNum = findTableOfVisitor(sharedData, pid);

    sharedData->tables[tableNum].chairs[chairNum].visitor = 0;
    sharedData->tables[tableNum].occupiedSeats--;
    
    // if all 4 clients left, mark the table as not full
    if(sharedData->tables[tableNum].occupiedSeats == 0){
        sharedData->tables[tableNum].is_full = false;
    }

    float avgStayTime = sharedData->stats.avgStayTime;
    int totalVisitors = sharedData->stats.totalVisitors;
    avgStayTime = (avgStayTime + actualTime) / totalVisitors;

    sharedData->stats.avgStayTime = avgStayTime;

    sem_post(&sharedData->mutex);


}

