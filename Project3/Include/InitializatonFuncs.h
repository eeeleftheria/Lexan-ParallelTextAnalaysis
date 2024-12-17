#include "SharedMemory.h"

// initializes the shared memory segment
void initSharedMemory(struct sharedObjects* sharedData);

// forks one receptionist process & executes it
void createRecept(float orderTime, char* shmid, char* loggingFile, pid_t* receptPid);

// forks multiple visitor processes & executes them
void createVisitor(int numOfVisitors, float restTime, char* shmid, char* loggingFile, pid_t visitorPids[]);

// creates a logging file where all the actions will be written
void createLoggingFile(char* loggingFile);

void createClosingProcess(char* shmid, int openTimeOfBar, pid_t* closingPid, char* loggingFile);
