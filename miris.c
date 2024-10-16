#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "graph.h"
#include "hash.h"
#include <unistd.h>
#include <getopt.h>


//argc αριθμος παραμετρων συμπεριλαμβανομενου του ονοματος του προγραμματος
//αν argc = 1 τοτε δεν εχουμε καμια παραμετρο
//argv[0] = ονομα προγραμματος
int main(int argc, char* argv[]) {
   
    FILE* file;
   
    char* input_file = NULL;
    char* output_file = NULL;

    int opt;

    while((opt = getopt(argc, argv, "i:o:")) != -1){

        if(opt == 'i') {
            input_file = optarg; 
        }
        
        else if(opt == 'o'){
            output_file = optarg;
        }
        else{
            fprintf(stderr, "Correct format is: %s -i inputfile -o outputfile\n", argv[0]);
            return -1;
        }
    }

    file = fopen(input_file, "r");


    Graph graph = graphCreate();
    
    int user1;
    int user2;
    int amount;
    char* date = malloc(11*sizeof(char));

    char content[200];

    int count = 0;
    //αναγνωση περιεχομενου κ αποθηκευση μεσα στο content
    //η fget σταματαει να διαβαζει οταν συναντησει α΄΄λλαγη γραμμης ή EOF

    while(fgets(content, 200, file) != NULL) {
        count++;
    }

    //επαναφορα δεικτη στην αρχη του αρχειου
    fseek(file, 0, SEEK_SET);

    //μεγεθος hash table οσο 2 φορες τις γραμμες του αρχειου
    HashTable table = hashCreate(2*count);
    
    while(fgets(content, 200, file) != NULL){
        sscanf(content, "%d %d %d %s", &user1, &user2, &amount, date);
        
        edgeAdd(graph, amount, date, user1, user2, table);
    }

    fclose(file);


    file = fopen(output_file, "w");

    graphDisplay(graph, file, table);

    fclose(file);
    
    /////////////////////////////
    ///////// PROMPT ///////////
    ////////////////////////////

     
    

    
    graphDestroy(graph, graphDestroyNode, table);

    free(date);










}