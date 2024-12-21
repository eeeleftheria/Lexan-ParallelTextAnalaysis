#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/times.h>
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

    double twaitingStart = 0;
    double twaitingEnd = 0;
    double tstayingStart = 0;
    double tstayingEnd = 0;

    // how many clock ticks per second does the CPU
    double ticspersec = (double) sysconf(_SC_CLK_TCK);

    // time(NULL) returns the current time of the system 
    // it is the same in all the processes since they are executed almost at the same time
    srand(time(NULL)^getpid());

    if(argc != 7){
        printf("Usage: ./visitor -d resttime -s shmid -l loggingFile\n");
        exit(1);
    }

    int resttime; // max time the visitor will stay in the bar after getting his order
    char* shmid; // shared memory name
    int fdLogging; // file descriptor for the logging file

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

    //#### VISITOR ENTERS WAITING LINE

    // the time the visitor started waiting to enter the bar
    twaitingStart = (double) times(NULL);
    
    sem_wait(&sharedData->maxWaiting); // if it falls below 0, the visitor gets suspended since the waiting line is full
    sem_wait(&sharedData->mutex); 


    // if the bar is closing, the cient should leave
    if(sharedData->isClosing == true){
        sem_post(&sharedData->maxWaiting); // free the semaphore
        sem_post(&sharedData->mutex); // free the mutex

        // detach the shared memory segment
        if(munmap(sharedData, sizeof(struct sharedObjects)) == -1){
            printf("munmap failure in monitor\n");
            close(fdLogging); // close the logging file
            exit(1);
        }

        close(fd); // close the shared memory segment
        close(fdLogging); // close the logging file

        exit(0);
    }
    
    // 1. check if the waiting line is empty: 2 cases
    // a) if it is empty, can have a seat at the bar if there is space
    // b) or he remains in the waiting line and waits for a table
    if(sharedData->waitingLine.count == 0){
        
        // if it is indeed empty, the visitor becomes both the first & last of the queue
        sharedData->waitingLine.first = 0;
        sharedData->waitingLine.last = 0;
        sharedData->waitingLine.buffer[0] = pid;
        sharedData->waitingLine.count++;

        // the visitor checks himself wether he can have a seat at the bar
        
        // 1a) if there is space, he sits and places an order
        if(checkForTable(sharedData, fdLogging, pid) == true){
            
            sem_post(&sharedData->maxWaiting); // one less visitor in the waiting line 
            
            twaitingEnd = (double) times(NULL); // the time the visitor was sitted
            tstayingStart = (double) times(NULL); // same
            
            // the mutex is freed inside the function & the visitor sleeps on 
            // its chair until the order is ready
            placeOrder(sharedData, fdLogging, pid);

            sem_wait(&sharedData->mutex);
            stayInBar(sharedData, fdLogging, pid, resttime);
            sem_post(&sharedData->mutex);
        }

        // 1b) if there is no space, he remains in the waiting line and waits for a table
        else{
            

            // write to logging file that a visitor has entered the waiting line
            char message[100];
            sprintf(message, "NEW CLIENT: %d has entered the waiting line\n\n", pid);
            write(fdLogging, message, strlen(message));


            sem_post(&sharedData->mutex); // free the mutex
            
            sem_wait(&sharedData->waitingLine.sems[0]); // wait for someone to wake him up when there is a free table

            sem_wait(&sharedData->mutex); // lock the mutex again

            // the client has been woken up, so he has a seat at the bar
            checkForTable(sharedData, fdLogging, pid);
            twaitingEnd = (double) times(NULL); // the time the visitor was sitted
            tstayingStart = (double) times(NULL); // same
            sem_post(&sharedData->maxWaiting); // one less visitor in the waiting line 

            // the mutex is freed inside the function & the visitor sleeps on 
            // its chair until the order is ready
            placeOrder(sharedData, fdLogging, pid);

            sem_wait(&sharedData->mutex);
            stayInBar(sharedData, fdLogging, pid, resttime);
            sem_post(&sharedData->mutex);
        }
    
    }
    
    // 2. if the waiting line is not empty, add visitor to it 
    else{
        // if waiting line has 5 spots and last = 4, the new visitor should be
        // added to (4 + 1) % 5 = 0, since 4 is the last position of the buffer
        sharedData->waitingLine.last = (sharedData->waitingLine.last + 1) % MAX_WAITING;
        sharedData->waitingLine.buffer[sharedData->waitingLine.last] = pid; // store the visitor's pid
        sharedData->waitingLine.count++; // increase the number of visitors in the waiting line

        char message[100];
        sprintf(message, "NEW CLIENT: %d has entered the waiting line\n\n", pid);
        write(fdLogging, message, strlen(message));

        sem_post(&sharedData->mutex); // free the mutex
       
        int last = sharedData->waitingLine.last;

        sem_wait(&sharedData->waitingLine.sems[last]); // wait for someone to wake him up when there is a free table

        sem_wait(&sharedData->mutex); // lock the mutex again

        // the client has been woken up, he can now have a seat at the bar
        checkForTable(sharedData, fdLogging, pid);
        twaitingEnd = (double) times(NULL); // the time the visitor was sitted
        tstayingStart = (double) times(NULL); // same as above
        sem_post(&sharedData->maxWaiting); // one less visitor in the waiting line 

        // the mutex is freed inside the function & the visitor sleeps on 
        // its chair until the order is ready
        placeOrder(sharedData, fdLogging, pid);

        sem_wait(&sharedData->mutex);
        stayInBar(sharedData, fdLogging, pid, resttime);
        sem_post(&sharedData->mutex);

    }

    sem_wait(&sharedData->mutex);

    // check if bar is closing
    if(sharedData->isClosing == true){
        
        // if it is the last visitor to leave the bar, the receptionist should be woken up
        if(isLastVisitor(sharedData) == true){
            sem_post(&sharedData->receptionist);
        }
    
    }

    sem_post(&sharedData->mutex);

    // #### CALCULATION OF TIMES

    // the time before the visitors exits the bar
    tstayingEnd = (double) times(NULL);

    // the duration the visitor stayed in the bar
    double twaiting = (twaitingEnd - twaitingStart) / ticspersec;
    double tstaying = (tstayingEnd - tstayingStart) / ticspersec;

    sem_wait(&sharedData->mutex);
    
    // the division by the total visitors is done by the receptionist
    // when the bar is about to close
    sharedData->stats.avgStayTime = (sharedData->stats.avgStayTime + tstaying);
    sharedData->stats.avgWaitTime =  (sharedData->stats.avgWaitTime + twaiting);
    
    sem_post(&sharedData->mutex);


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
            sprintf(message,"CLIENT SEATED: %d had a seat at table %d\n\n" ,pid, i);
            write(fdLogging, message, strlen(message));


            // if all 4 seats are occupied, the table is now full & 
            // is_full becomes false, only if all of the 4 visitors leave
            if(sharedData->tables[i].occupiedSeats == 4){
                sharedData->tables[i].is_full = true;

                char message[100];
                sprintf(message,"FULL TABLE: Table %d is now full\n\n", i);
                write(fdLogging, message, strlen(message));
            }

            // new visitor is added to the statistics
            sharedData->stats.totalVisitors++;

            // remove visitor from waiting line
            // since the buffer is now empty, we do not need to declare a new first & last
            sharedData->waitingLine.count--; 
            sharedData->waitingLine.buffer[sharedData->waitingLine.first] = 0; // removal of visitor's pid from the buffer
            
            // if the waiting line is empty, the first & last are set to 0
            if(sharedData->waitingLine.count == 0){
                sharedData->waitingLine.first = 0;
                sharedData->waitingLine.last = 0;
            }

            // else we should update the first to be the next visitor in line
            else{
                sharedData->waitingLine.first = (sharedData->waitingLine.first + 1) % MAX_WAITING;
            }
            
                    
            
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

    int minFood = 0; // least number of food items
    int maxFood = 2; // maximum number of food items
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

    // if the buffer is empty, the order becomes both the first & last 
    if(sharedData->ordersOrder.count == 0){
        sharedData->ordersOrder.first = 0;
        sharedData->ordersOrder.last = 0;
        sharedData->ordersOrder.buffer[sharedData->ordersOrder.last] = newOrder;

    }

    // else it is added to the end of the buffer
    else{
        sharedData->ordersOrder.last = (sharedData->ordersOrder.last + 1) % MAX_ORDERS;
        sharedData->ordersOrder.buffer[sharedData->ordersOrder.last] = newOrder;
        
    }

    sharedData->ordersOrder.count++;
    
    // write to logging file that a visitor has placed an order
    char message[100];
    sprintf(message, "NEW ORDER: %d ordered %d drinks and %d plates\n\n", pid, numOfDrinks, numOfFood);
    write(fdLogging, message, strlen(message));

    sem_post(&sharedData->receptionist); // wake up the receptionist to take the order

    int pos = findChairOfVisitor(sharedData, pid);
    int posTable = findTableOfVisitor(sharedData, pid);

    sem_post(&sharedData->mutex); // free mutex

    sem_wait(&sharedData->tables[posTable].sems[pos]); // wait for the order to be ready

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
    
    // [0.7 * restTime, restTime]
    float minTime = 0.7 * restTime;
    float maxTime = restTime;

    // for example: if min = 10 and max = 15
    // then actualTime = rand() % 6 + 10 = [0, 5] + 10 = [10, 15]
    float actualTime = rand() % (int)(maxTime - minTime + 1) + minTime;

    char message[100];
    sprintf(message, "CLIENT EATING: %d eats and converses for %f time\n\n", pid, actualTime);
    write(fdLogging, message, strlen(message));

    sem_post(&sharedData->mutex);
    
    sleep(actualTime);

    sem_wait(&sharedData->mutex);

    char message2[100];
    sprintf(message2, "CLIENT LEAVING: %d left the bar\n\n", pid);
    write(fdLogging, message2, strlen(message2));

    // remove visitor from the table
    int chairNum = findChairOfVisitor(sharedData, pid);
    int tableNum = findTableOfVisitor(sharedData, pid);

    sharedData->tables[tableNum].chairs[chairNum].visitor = 0; // set the visitor to 0
    sharedData->tables[tableNum].occupiedSeats--; // one less occupied seat
    
    // if all 4 clients left, mark the table as not full
    if(sharedData->tables[tableNum].occupiedSeats == 0){
        sharedData->tables[tableNum].is_full = false;

        // last visitor leaving the table should wake up up to 4 visitors waiting 
        wakeUpWaitingVisitors(sharedData, tableNum);
    }

}


void wakeUpWaitingVisitors(struct sharedObjects* sharedData, int tableNum){

    int totalWaiting = sharedData->waitingLine.count;
    
    if(totalWaiting == 0){
        return;
    }

    // if the line has less than 4 visitors
    else if((totalWaiting <= 4) && (totalWaiting > 0)){

        int first = sharedData->waitingLine.first;
        int last = sharedData->waitingLine.last;

        //wake them all up
        for(int i = first; i <= last; i++){

            sem_post(&sharedData->waitingLine.sems[i]);
        }

        // remove the visitors from the waiting line
        for(int i = first; i <= last; i++){
            sharedData->waitingLine.buffer[i] = 0; // set all the pids of the consumed positions to 0
        }
    }

    else if(totalWaiting > 4){
        // wake up the first 4 visitors
        int first = sharedData->waitingLine.first;

        for(int i = first; i < first + 4; i++){
            sem_post(&sharedData->waitingLine.sems[i]);
        }

        // remove the first 4 visitors from the waiting line
        for(int i = first; i < first + 4; i++){
            sharedData->waitingLine.buffer[i] = 0;
        }
    }
}

bool isLastVisitor(struct sharedObjects* sharedData){
    
    for(int i = 0; i < MAX_TABLES; i++){
        
        for(int j = 0; j < MAX_CHAIRS; j++){
        
            if(sharedData->tables[i].occupiedSeats > 0){
                return false;
            }
        }
    }

    return true;
}

