#include "SharedMemory.h"

// updates the counters of the products of the order in the statistics
void updateStats(struct sharedObjects* sharedData, order currentOrder, int fdLogging);

// returns the table number of the visitor with the given pid or -1 if the visitor is not found
int findTableOfVisitor(struct sharedObjects* sharedData, pid_t pid);

// returns the chair number of the visitor with the given pid or -1 if the visitor is not found
int findChairOfVisitor(struct sharedObjects* sharedData, pid_t pid);

bool checkIfBarIsEmpty(struct sharedObjects* sharedData);

void closeBar(struct sharedObjects* sharedData, int fdLogging, int fdSharedMem, char* shmid);

void writeStats(struct sharedObjects* sharedData, int fdLogging);