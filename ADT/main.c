#include "hash.h"
#include <stdio.h>
#include "list.h"
#include <string.h>


int compareWords(Pointer a, Pointer b){
    return strcmp((char*)a, (char*)b);
}

int main(){
    HashTable table = hashCreate(5);
    char* w1 ="mam";
    char* w2 = "rfhfrf";
    char* w3 = "wx";
    int count = 0;
    int count2 = 0;
    int count3 = 0;
    
    hashAdd(table, w1, &count);
    hashRemove(table, w1, compareWords);

    hashAdd(table, w2, &count2);
    hashRemove(table, w2, compareWords);

    hashAdd(table, w3, &count3);
    hashRemove(table, w3,compareWords);
    

    hashDisplay(table);

    hashDestroy(table);
}

//συγκρινει δυο λεξεις
