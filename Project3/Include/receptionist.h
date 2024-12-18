#include "SharedMemory.h"

// updates the counters of the products of the order in the statistics
void updateStats(struct sharedObjects* sharedData, order currentOrder, int fdLogging);

// returns the table number of the visitor with the given pid or -1 if the visitor is not found
int findTableOfVisitor(struct sharedObjects* sharedData, pid_t pid);

// returns the chair number of the visitor with the given pid or -1 if the visitor is not found
int findChairOfVisitor(struct sharedObjects* sharedData, pid_t pid);

// returns true if the bar is empty, false otherwise
bool checkIfBarIsEmpty(struct sharedObjects* sharedData);

// closes the bar, writes final statistics, destroys all semaphores and unlinks the shared memory
void closeBar(struct sharedObjects* sharedData, int fdLogging, int fdSharedMem, char* shmid);

// writes the final statistics to the logging file
void writeStats(struct sharedObjects* sharedData, int fdLogging);