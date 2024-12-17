#include "SharedMemory.h"

// checks if there is a table available for the visitor and returns true if there is, else false
bool checkForTable(struct sharedObjects* sharedData, int fdLogging, pid_t pid);

// places an order for the visitor
void placeOrder(struct sharedObjects* sharedData, int fdLogging, pid_t pid);

// returns the chair number of the visitor with the given pid or -1 if the visitor is not found
int findChairOfVisitor(struct sharedObjects* sharedData, pid_t pid);

// the client stays in the bar for a random time and then leaves, updating all the necessary data
void stayInBar(struct sharedObjects* sharedData, int fdLogging, pid_t pid,float restTime);