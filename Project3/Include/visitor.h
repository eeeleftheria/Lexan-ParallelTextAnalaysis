#include "SharedMemory.h"

// adds a visitor to the waiting line & updates the data of the latter
void addVisitorToWaitingLine(struct sharedObjects* sharedData, int restTime, int fdLogging, pid_t pid);

// writes a message to the log file for the visitor with pid
void writeToLog(int fd, char* message, pid_t pid);