#include "hash.h"
#include <stdio.h>
#include "list.h"
#include <string.h>


int compareWords(Pointer a, Pointer b){
    return strcmp((char*)a, (char*)b);
}

int main(){
    HashTable table = hashCreate(5, compareWords);
    char* w1 ="mam";
    char* w2 = "rfhfrf";
    char* w3 = "wx";
    int count = 0;
    int count2 = 0;
    int count3 = 0;
    
    hashAdd(table, w1, &count);
    hashRemove(table, w1);

    hashAdd(table, w2, &count2);

    hashAdd(table, w3, &count3);
    hashRemove(table, w3);

    hashAdd(table, w2, &count);
    hashAdd(table, w3, &count);
    hashAdd(table, w3, &count);

    

    hashDisplay(table);

    hashDestroy(table);
}

//συγκρινει δυο λεξεις
