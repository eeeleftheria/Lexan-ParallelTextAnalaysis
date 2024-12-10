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

struct statistics{
    int numOfWaterDrinks;
    int numOfWineDrinks;
    int numOfCheesePlates;
    int numOfSalads;
    int numOfVisitors;
    int avgWaitTime;
    int avgStayTime;
};

struct sharedObjects{
    sem_t mutex;
    sem_t receptionist;
    sem_t visitor;
    struct statistics stats;
};

int main(){

    // Creation of shared memory segment
    // arguments:   
    //      name: my_memory
    //      flags: O_CREAT | O_RDWR for creation if it does not exist and read/write
    //      rights: 0666 for read/write for all users
    int fd = shm_open("/sharedMemory", O_CREAT | O_RDWR, 0666);

    if(fd == -1){
        perror("shm_open failure"); 
        exit(1);
    }

    // Change the size of the shared memory segment to be as much as the struct's sharedObjects
    if( ftruncate(fd, sizeof(struct sharedObjects))  == -1){
        perror("ftruncate failure");
        exit(1);
    }

    // Map the shared memory segment to the address space of the calling process
    struct sharedObjects* sharedData = mmap(NULL, sizeof(struct sharedObjects), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(sharedData == MAP_FAILED){
        perror("mmap failure");
        exit(1);
    }

    printf("Shared memory segment created and mapped successfully\n");





}