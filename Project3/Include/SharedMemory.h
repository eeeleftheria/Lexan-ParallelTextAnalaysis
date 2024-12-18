#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>

#pragma once

typedef struct statistics statistics;
typedef struct chair chair;
typedef struct table table;
typedef struct order order;
typedef struct circularBuffer circularBuffer;
typedef struct circularOrders circularOrders;

#define MAX_WAITING 10 // max number of clients waiting
#define MAX_NUM_OF_ITEMS_PER_ORDER 4 // at least 1 drink & salad, cheese, both or none
#define MAX_ORDERS 12 // max number of orders
#define MAX_TABLES 3 // number of tables
#define MAX_CHAIRS 4 // number of chairs per table

enum MENU {
    WATER,
    WINE,
    CHEESE,
    SALAD
};

struct statistics{
    int totalWaterDrinks; // number of water drinks served
    int totalWineDrinks; // number of wine drinks served
    int totalCheesePlates; // number of cheese plates served
    int totalSalads; // number of salads served
    int totalVisitorsServed; // number of visitors served
    int totalVisitors; // number of visitors that entered the bar
    float avgWaitTime; // average waiting time of the visitors
    float avgStayTime; // average stay time of the visitors
    float avgServeTime; // average serving time
};

struct chair{
    pid_t visitor; //each chair has a visitor with a pid
};

struct table{
    chair chairs[4]; //4 seats per table
    int occupiedSeats; //number of occupied seats
    bool is_full; //boolean var to check if the table is full
    sem_t sems[MAX_CHAIRS]; //semaphore for each chair, so visitor gets suspended when he is waiting for his order
};

struct order{
    pid_t visitor_id; //each order belongs to a client/visitor
    enum MENU items[MAX_NUM_OF_ITEMS_PER_ORDER]; // each item is of type MENU
    int count; // number of items in the order
};

// this struct preserves the FCFS policy
struct circularBuffer{
    pid_t buffer[MAX_WAITING]; //buffer that stores the pids of the clients waiting
    int first; // pos of first one waiting
    int last; // pos of last one waiting
    int count; //number of clients waiting
    sem_t sems[MAX_WAITING]; //semaphore for each client, so that only one client wakes up at a time
    // from the waiting line & FIFO is ensured
};

struct circularOrders{
    order buffer[MAX_ORDERS]; //buffer that stores the pids of the clients waiting
    int first; // first order
    int last; // last order
    int count; //number of orders currently

};

// segment of shared memory
struct sharedObjects{
    statistics stats; //statistics struct
    table tables[MAX_TABLES]; //an array of 3 tables

    sem_t mutex; // mutual exclusion for the shared memory
    sem_t receptionist; // one receptionist

    circularBuffer waitingLine; // buffer that stores the pids of the clients waiting
    sem_t maxWaiting; //semaphore to control the number of clients waiting

    circularOrders ordersOrder; // the order of the orders

    bool isClosing; // boolean var to check if the restaurant is closing
    
};

