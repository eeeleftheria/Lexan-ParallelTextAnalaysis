#include "SharedMemory.h"

void initSharedMemory(struct sharedObjects* sharedData);
void createRecept(float orderTime, char* shmid);
void createVisitor(int numOfVisitors, float restTime, char* shmid);
