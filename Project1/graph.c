#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "list.h"
#include "hash.h"
#include <string.h>


struct graph{
    List nodes; //οι κορυφες/κομβοι του γραφου
    int size;
};

struct graph_node{ 
    int user;
    List outgoing_edges; //αποθηκευει τις εξερχομενες ακμες του κομβου
    List incoming_edges; //αποθηκευει τις εισερχομενες ακμες του κομβου
};


struct edge{
    int amount; //το ποσο συναλλαγης
    char* date; //ημερομηνια συναλλαγης (μαζι με το ποσό αποτελουν το βαρος της ακμης)
    GraphNode dest_node; //ο προορισμος της συγκεκριμενης ακμης
    GraphNode source_node;
};


Graph graphCreate(){
    Graph graph = malloc(sizeof(struct graph));
    graph->nodes = listCreate();
    graph->size = 0;

    return graph;
}


void graphAdd(Graph graph, int user, HashTable hash_table){
    
    //δεν μπορουμε να προσθεσουμε user με ιδιο id
    if(hashFindGraphNodeWithKey(hash_table, user) != NULL) {
        printf("User with id '%d' already exists\n", user);
        return;
    }
    
    GraphNode graph_node = malloc(sizeof(struct graph_node));

    graph_node->user = user;

    graph_node->outgoing_edges = listCreate();
    graph_node->incoming_edges = listCreate();

    //προσθηκη κομβου στη λιστα με τις κορυφες
    List list_nodes = graph->nodes;
    listInsert(list_nodes, graph_node);

    //θα προσθετουμε τον αντιστοιχο graph node και στο hash_table που μας δινεται
    hashAdd(hash_table, user, graph_node);

    //ενημερωση μεγεθους γραφου
    graph->size ++;   
    
}


void graphRemove(Graph graph, int user, HashTable hash_table){
   
    List list_nodes = graph->nodes;
    
    //ευρεση του graph node μεσω hash για πιο γρηγορη αναζητηση 
    GraphNode node_to_remove = hashFindGraphNodeWithKey(hash_table, user);

    //αφαιρεση πρωτα απο το hash table, χωρις να κανουμε free τον graph node
    hashRemove(hash_table, user, hashDestroyValue);

    //πρεπει να διαγραψουμε το value node_to_remove του list node  και να κανουμε free τον κομβο της λιστας
    listRemove(list_nodes, findNodeWithValue(list_nodes, node_to_remove), graphDestroyNode);

    //ενημερωση του μεγεθους του γραφου
    graph->size--;

}

int graphSize(Graph graph){
    List list = graph->nodes;
    
    return listSize(list);
}


int graphGetUser(GraphNode graph_node){
    return graph_node->user;
}



//δεικτης σε συναρτηση που καταστρεφει ενα graph node
//τυπου DestroyFunc
void graphDestroyNode(Pointer graph_node){
   
    GraphNode node_to_destroy = (GraphNode)graph_node;

    //πρεπει να καταστρεψουμε κ τις δυο λιστες με τις ακμες του
    listDestroy(node_to_destroy->incoming_edges, edgeDestroyValueForIncoming);
    listDestroy(node_to_destroy->outgoing_edges, edgeDestroyValueForOutgoing);
    
    free(graph_node);
}

//κατα την καταστροφη του γραφου θελουμε να καταστρεφουμε καταλληλα
//το value καθε κομβου
void graphDestroy(Graph graph, DestroyValueFunc func, HashTable table){

    hashDestroy(table);
    listDestroy(graph->nodes, graphDestroyNode);
    free(graph);
}




void graphDisplay(Graph graph, FILE* output, HashTable table){
    ListNode node;
    List list = graph->nodes;
    
    for(node = listFirst(list); node != NULL; node = listGetNext(node)) {
        
        GraphNode graph_node = listNodeValue(list, node);
        fprintf(output, "User with id '%d'\n", (graph_node->user));

        //εκτυπωση συναλλαγων/ακμων κλπ
        edgesOfNodeDisplay(graph, graph_node->user, table, output);
        
        fprintf(output, "\n\n");

    }
}


//#################################//
//############# ΑΚΜΕΣ #############//
//#################################//
void edgeAdd(Graph graph, int amount, char* date, int source_user, int dest_user, HashTable hash_table){

    Edge new_edge = malloc(sizeof(struct edge));
    
    new_edge->amount = amount;
    new_edge->date = date;

    //αν οι χρηστες(κορυφες) με ονομα source_user, dest_user αντιστοιχα, δεν υπαρχουν
    //πρεπει να δημιουργηθουν
    if(hashFindGraphNodeWithKey(hash_table, source_user) == NULL) {
        graphAdd(graph, source_user, hash_table);
    }


    if(hashFindGraphNodeWithKey(hash_table, dest_user) == NULL) {
        graphAdd(graph, dest_user, hash_table);
    }

    //αναθεση source graph node και dest graph node της ακμης
    new_edge->dest_node = hashFindGraphNodeWithKey(hash_table, dest_user);
    new_edge->source_node = hashFindGraphNodeWithKey(hash_table, source_user);

    //προσθηκη στην αντιστοιχη λιστα
    listInsert(new_edge->dest_node->incoming_edges, new_edge);
    listInsert(new_edge->source_node->outgoing_edges, new_edge);

}

//ευρεση ακμης για συγκεκριμενο source, dest
//καθως η ιδια ακμη υπαρχει στην λιστα incoming edges της κορυφης προορισμου
//αλλα και στην λιστα outgoing edges της ακμης πηγης, αρκει η αναζητηση της σε μια μονο λιστα
Edge edgeFind(Graph graph, int source_user, int dest_user, HashTable hash_table){

    GraphNode source = hashFindGraphNodeWithKey(hash_table, source_user);
    GraphNode dest = hashFindGraphNodeWithKey(hash_table, dest_user);

    
    List list = source->outgoing_edges;
    ListNode node;
    for(node = listFirst(list); node != NULL; 
        node = listGetNext(node)){

            Edge edge = listNodeValue(list, node);

            //αν βρουμε την ακμη με τον δοθεν προορισμο, την επιστρεφουμε
            if(edge->dest_node == dest) {
                return edge;
            }
    }   
    return NULL;  
}

//ευρεση ακμης με συγκεκριμενο source, dest, sum, date
Edge edgeFindWithAmountAndDate(Graph graph, int source_user, int dest_user, int sum, char* date,  HashTable hash_table){
    GraphNode source = hashFindGraphNodeWithKey(hash_table, source_user);
    GraphNode dest = hashFindGraphNodeWithKey(hash_table, dest_user);

    List list = source->outgoing_edges;
    ListNode node;
    for(node = listFirst(list); node != NULL; 
        node = listGetNext(node)){

            Edge edge = listNodeValue(list, node);

            //αν βρουμε την ακμη με τον δοθεν προορισμο, την επιστρεφουμε
            if(edge->dest_node == dest && edge->amount == sum && (strcmp(date, edge->date) == 0)) {
                return edge;
            }
    }   
    return NULL;
}



//αφαιρεση ακμης μεταξυ δυο κορυφων
void edgeRemove(Graph graph, int source_user, int dest_user, HashTable hash_table){
    
    //ευρεση των δυο κορυφων με βαση το id τους
    GraphNode source = hashFindGraphNodeWithKey(hash_table, source_user);
    GraphNode dest = hashFindGraphNodeWithKey(hash_table, dest_user);
    
    Edge edge;
    
    if(source != NULL &&  dest!= NULL){
        //ευρεση μιας μεταξυς τους ακμης
         edge = edgeFind(graph, source_user, dest_user, hash_table);

         if(edge == NULL) {
            printf("transaction from user '%d' -> user '%d' not found\n", source_user, dest_user);
            return;
         }
        
        //αφαιρεση κομβου λιστας που εμπεριεχει τη συγκεκριμενη ακμη
        ListNode node_to_remove_incoming = findNodeWithValue(dest->incoming_edges, edge);
        if(node_to_remove_incoming != NULL) {
           
            listRemove(dest->incoming_edges, node_to_remove_incoming, edgeDestroyValueForIncoming);
        }

        //διαγραφη ακμης και αφαιρεση κομβου λιστας με τη συγκεκριμενη ακμη
        ListNode node_to_remove_outgoing = findNodeWithValue(source->outgoing_edges, edge);
        if(node_to_remove_outgoing != NULL) {

            listRemove(source->outgoing_edges, node_to_remove_outgoing, edgeDestroyValueForOutgoing);
        }
    } 
            

}



//δεν πρεπει να καταστρεφουμε τα graph nodes source, dest
//αρκει να κανουμε free την ακμη, αφου δεν δεσμευουμε αλλη μνημη 
void edgeDestroyValueForOutgoing(Pointer value){
    free(value); //οπου value ειναι το edge
    //πρεπει να γινει μονο σε μια απο τις δυο λιστες!!!!
}

void edgeDestroyValueForIncoming(Pointer value){

}




//καταστροφη λιστας με εισερχομενες ακμες
void incomingEdgesDestroy(GraphNode node){
    listDestroy(node->incoming_edges, edgeDestroyValueForIncoming);
}

//καταστροφη λιστας με εξερχομενες ακμες
void outgoingEdgesDestroy(GraphNode node){
    listDestroy(node->outgoing_edges, edgeDestroyValueForOutgoing);
}


//εκτυπωση ακμων μιας κορυφης
void edgesOfNodeDisplay(Graph graph, int user, HashTable table, FILE* output){
    GraphNode graph_node = hashFindGraphNodeWithKey(table, user);
    List list_inc = graph_node->incoming_edges;
    List list_out = graph_node->outgoing_edges;

    fprintf(output, "Transactions of user '%d' are: \n", user);
    fprintf(output, "from   to    amount       date\n");

    ListNode node;

    for (node = listFirst(list_out); node != NULL; node = listGetNext(node)){
        
        Edge edge = listNodeValue(list_out, node);
        int source = edge->source_node->user;
        int dest = edge->dest_node->user;

        fprintf(output, "  %d    %d      %d      %s\n", source, dest, edge->amount, edge->date);
    }

    fprintf(output, "\n");
    fprintf(output, "Incoming transactions:\n");
    fprintf(output, "from   to    amount       date\n");



    for (node = listFirst(list_inc); node != NULL; node = listGetNext(node)){
        
        Edge edge = listNodeValue(list_inc, node);
        int source = edge->source_node->user;
        int dest = edge->dest_node->user;

        fprintf(output, "  %d     %d     %d       %s\n", source, dest, edge->amount, edge->date);
    }

}


//εκτυπωση εξερχομενων ακμων μιας κορυφης
void edgesOutgoingOfNodeDisplay(Graph graph, int user, HashTable table){
    
    GraphNode graph_node = hashFindGraphNodeWithKey(table, user);
    List list_out = graph_node->outgoing_edges;

    printf("Outgoing edges of user '%d' are: \n", user);
    printf("from   to    amount       date\n");

    ListNode node;

    //διατρεχουμε τη λιστα με τις εξερχομενες ακμες του κομβου και εκτυπωνουμε
    //ολες τις πληροφοριες των ακμων του
    for (node = listFirst(list_out); node != NULL; node = listGetNext(node)){
        
        Edge edge = listNodeValue(list_out, node);
        int source = edge->source_node->user;
        int dest = edge->dest_node->user;

        printf("  %d    %d      %d      %s\n", source, dest, edge->amount, edge->date);
    }

}

//εκτυπωση εισερχομενων ακμων κορυφης
void edgesIncomingOfNodeDisplay(Graph graph, int user, HashTable table){
    
    GraphNode graph_node = hashFindGraphNodeWithKey(table, user);
    List list_out = graph_node->incoming_edges;

    printf("Incoming edges of user '%d' are: \n", user);
    printf("from   to    amount       date\n");

    ListNode node;

    //διατρεχουμε τη λιστα με τις εισερχομενες ακμες του κομβου και εκτυπωνουμε
    //ολες τις πληροφοριες των ακμων του
    for (node = listFirst(list_out); node != NULL; node = listGetNext(node)){
        
        Edge edge = listNodeValue(list_out, node);
        int source = edge->source_node->user;
        int dest = edge->dest_node->user;

        printf("  %d    %d      %d      %s\n", source, dest, edge->amount, edge->date);
    }

}


//τροποποιηση ακμης
void edgeModify(Graph graph, HashTable table, int source, int dest, int old_sum, int new_sum, char* old_date, char* new_date){
    
    //ευρεση πρωτης ακμης με το συγκεκριμενο ποσο συναλλαγης
    Edge edge = edgeFindWithAmountAndDate(graph, source, dest, old_sum, old_date, table);    

    if(edge != NULL) {
        edge->amount = new_sum;
        edge->date = new_date;
    }

    return;
    
}


void graphFindCircle(Graph graph, int user, HashTable table, List visited){
    GraphNode node = hashFindGraphNodeWithKey(table, user);
    List list = node->outgoing_edges;

    for(ListNode node = listFirst(list); node != NULL; node = listGetNext(node)){
        Edge edge = listNodeValue(list, node);
        GraphNode neighbor = edge->dest_node;

        //αν δεν εχει επισκεφθει ο κομβος
        if(findNodeWithValue(visited, neighbor) == NULL){
            listInsert(visited, neighbor);
            graphFindCircle(graph, neighbor->user, table, visited);
        }

        if(neighbor->user == user){
            printf("Circle found:\n");

            for(ListNode node = listFirst(visited); node != NULL; node = listGetNext(node)){
                
            }
        }
    }
}



