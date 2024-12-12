#include "SharedMemory.h"

// prints the statistics of the bar
void printStatistics(statistics stats);

// prints the tables of the bar and the visitors sitting
void printTables(table tables[]);

// prints the order of each client of each table
void printOrders(order orders[MAX_TABLES][MAX_CHAIRS]);

// prints the waiting line of the bar
void printWaitingLine(circularBuffer waitingLine);

// prints the order of the orders
void printOrdersOrder(circularOrders ordersOrder);
