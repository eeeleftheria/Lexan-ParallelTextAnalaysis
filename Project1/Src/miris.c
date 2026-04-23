#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "graph.h"
#include "hash.h"
#include <unistd.h>
#include <getopt.h>
#include <string.h>


// if argc = 1 then there are no parameters
int main(int argc, char* argv[]) {
   
    FILE* file;
   
    char* input_file = NULL;
    char* output_file = NULL;

    if(argc != 5) {
        fprintf(stderr, "Usage: %s -i <input file> -o <output file>\n", argv[0]);
        return 1;
    }

    for(int i = 0; i < argc; i++){
        
        // of the form ./miris -i inputfile -o outputfile
        if(strcmp(argv[i], "-i") == 0 && (i == 1)){
            input_file = argv[i+1];
        }
        if(strcmp(argv[i], "-o") == 0 && (i == 3)){
            output_file = argv[i+1];
        }
    }

    file = fopen(input_file, "r");


    Graph graph = graphCreate();
    
    int user1;
    int user2;
    int amount;
    char* date = malloc(11*sizeof(char));

    char content[200];

    int count = 0;// keep the number of lines in the file
    // so we can create a hash table of the appropriate size

    // read the contents and store them in content
    // fgets stops reading when it encounters a newline or EOF
    while(fgets(content, 200, file) != NULL) { 
        count++;
    }

    // reset the file pointer to the beginning of the file
    fseek(file, 0, SEEK_SET);

    // hash table size is twice the number of lines in the file
    HashTable table = hashCreate(2*count);
    
    while(fgets(content, 200, file) != NULL){ // read line
        sscanf(content, "%d %d %d %s", &user1, &user2, &amount, date);
        
        edgeAdd(graph, amount, date, user1, user2, table);
    }

    fclose(file);


    
    /////////////////////////////
    ///////// PROMPT ///////////
    ////////////////////////////
    char buffer[200];
    char* token;
    
    char command[20];

    while(1) {

        // read the command separately so we do not have issues with the newline
        if(scanf("%s", command) == EOF){ //EOF
            break;
        }

        // read the rest of the line
        fgets(buffer, 200, stdin);
        token = strtok(buffer, " "); // words are separated by spaces


        //########### 1 ##########//
        // i Ni [Nj Nk ...] - insert nodes
        if(strcmp(command, "insert") == 0 || strcmp(command, "i") == 0){
            

            while(token!= NULL){
                bool succ = false;

                int user = atoi(token); // string to int
                if (hashFindGraphNodeWithKey(table, user) == NULL){
                    succ = true;
                    graphAdd(graph, user, table);
                }
                else{
                    printf("Issue with: %d already exists\n", user);
                }
                if(succ == true){
                    printf("Successful insertion of: ");
                    printf("%d\n", user);
                }

                token = strtok(NULL, " "); // next token
            }
            printf("\n");
        }

        //########### 2 ##########//
        // n Ni Nj sum date - insert edges
        else if(strcmp(command, "insert2") == 0 || strcmp(command, "n") == 0){
            int source = atoi(token);
            
            token = strtok(NULL, " ");
            int dest = atoi(token);

            token = strtok(NULL, " ");
            int sum = atoi(token);

            token = strtok(NULL, " ");
            char* date = token;

            if(dest == source){
                printf("Issue with: %d %d\n", source, dest);
            }
            else{
                edgeAdd(graph, sum, date, source, dest, table);
                printf("Added edge from '%d' to '%d'\n", source, dest); 
            }
            
            token = strtok(NULL, " "); // next token

            printf("\n");
        }
        
        //########### 3 ##########//
        // d Ni [Nj Nk ...] - delete nodes
        else if(strcmp(command, "delete") == 0 || strcmp(command, "d") == 0){
            
            while(token!= NULL){

                int user = atoi(token); // string to int

                if (hashFindGraphNodeWithKey(table, user) != NULL){
                    graphRemove(graph, user, table);
                    printf("Successful deletion of node: %d\n", user);
                }
                else{
                    printf("Non existing node(s): %d\n", user);
                }
                token = strtok(NULL, " "); // next token
            }

        }

        //########### 4 ##########//
        // l Ni Nj - delete edge
        else if(strcmp(command, "delete2") == 0 || strcmp(command, "l") == 0){
            int source = atoi(token);
            
            token = strtok(NULL, " ");
            int dest = atoi(token);
            
            if(hashFindGraphNodeWithKey(table, source) == NULL){
                printf("Non-existing node(s): %d", source);
            }

            if(hashFindGraphNodeWithKey(table, dest) == NULL){
                printf("Non-existing node(s): %d", dest);
            }


            Edge edge = edgeFind(graph, source, dest, table);
            if(edge != NULL){
                edgeRemove(graph, source, dest, table);
                printf("Removed edge from '%d' to '%d'\n", source, dest);
            }
            else{
                printf("Non-existing edge from '%d' to '%d'\n", source, dest);
            }

            token = strtok(NULL, " "); // next token

        }

        //########### 5 ##########//
        // m Ni Nj sum sum1 date date1 - modify edge weight
        else if(strcmp(command, "modify") == 0 || strcmp(command, "m") == 0){
            int source = atoi(token);
            int dest = atoi(strtok(NULL, " "));
            int old_sum = atoi(strtok(NULL, " "));
            int new_sum = atoi(strtok(NULL, " "));
            char* old_date = strtok(NULL, " ");
            char* new_date = strtok(NULL, " ");

            // if at least one of the two nodes does not exist
            if((hashFindGraphNodeWithKey(table, source) == NULL) || (hashFindGraphNodeWithKey(table, dest)) == NULL){
                printf("Non-existing node(s): ");

                if(hashFindGraphNodeWithKey(table, source) == NULL){
                    printf("%d ", source);
                }
                if(hashFindGraphNodeWithKey(table, dest) == NULL) {
                    printf("%d\n", dest);
                }
    
            }
            // if they are not connected by an edge
            else if(edgeFind(graph, source, dest, table) == NULL){
                printf("Non-existing edge: %d %d %d %s", source, dest, old_sum, old_date);

            }
            else{
                edgeModify(graph, table, source, dest, old_sum, new_sum, old_date, new_date);
                printf("Successful modification of edge %d to %d\n", source, dest);
            }

            token = strtok(NULL, " ");
        }


        //########### 6 ##########//
        // f Ni - find all outgoing edges of Ni
        else if(strcmp(command, "find") == 0 || strcmp(command, "f") == 0){
            int user = atoi(token);
            
            if(hashFindGraphNodeWithKey(table, user) == NULL){
                printf("Non-existing node: %d\n", user);
            }
            else{
                edgesOutgoingOfNodeDisplay(graph, user, table);
            }
            strtok(NULL, " ");

        }

        //########### 7 ##########//
        // r Ni - find all incoming edges of Ni
        else if(strcmp(command, "receiving") == 0 || strcmp(command, "r") == 0){
            int user = atoi(token);
            
            if(hashFindGraphNodeWithKey(table, user) == NULL){
                printf("Non-existing node: %d\n", user);
            }
            else{
                edgesIncomingOfNodeDisplay(graph, user, table);
            }
            strtok(NULL, " ");
 
        }

        //########### 8 ##########//
        // c Ni - find cycles of Ni
        else if((strcmp(command, "circlefind") == 0) || (strcmp(command, "c") == 0)){
            int user = atoi(token);

            if(hashFindGraphNodeWithKey(table, user) == NULL){
                printf("Non-existing node: %d\n", user);
            }
            
        }


        //########### 12 ##########//
        // release memory and terminate the program
        else if(strcmp(command, "e") == 0 || strcmp(command, "exit") == 0){
            file = fopen(output_file, "w");
            graphDisplay(graph, file, table);
            fclose(file);


            graphDestroy(graph, graphDestroyNode, table);
            free(date);


            return 0;
        }
        else{
            printf("Format error - Command options:\n");
            printf("Insert node(s):\ni Ni [Nj Nk ...] or insert\n\n");
            printf("Insert an edge:\nn Ni Nj sum date or insert2\n\n");
            printf("Remove node(s):\nd Ni [Nj Nk ...] or delete\n\n");
            printf("Remove edge:\nl Ni Nj or delete2\n\n");
            printf("Modify edge:\nm Ni Nj sum sum1 date date1 or modify\n\n");
            printf("Find all outgoing edges from Ni:\nf Ni or find\n\n");
            printf("Find all incoming to Ni edges:\nr Ni or receiving\n\n");
            printf("Exit program:\ne or exit\n\n");
        }
    }

    graphDestroy(graph, graphDestroyNode, table);
    free(date);

    
}