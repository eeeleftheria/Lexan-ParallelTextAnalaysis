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
    GraphNode graph_node = malloc(sizeof(struct graph_node));

    graph_node->user = user;

    graph_node->outgoing_edges = listCreate();
    graph_node->incoming_edges = listCreate();

    
    List list_nodes = graph->nodes;
    listInsert(list_nodes, graph_node);

    //θα προσθετουμε τον αντιστοιχο graph node και στο hash_table που μας δινεται
    hashAdd(hash_table, user, graph_node);

    graph->size ++;   
    
}

void graphRemove(Graph graph, int user, HashTable hash_table){
   
    List list_nodes = graph->nodes;
    int* p = &user;
    
    //ευρεση του graph node μεσω hash για πιο γρηγορη αναζητηση 
    GraphNode node_to_remove = hashFindGraphNodeWithKey(hash_table, user);
    ListNode list_node = hashFindListNodeWithValue(hash_table, node_to_remove);

    //πρεπει να διαγραψουμε το value node_to_remove του list node  και να κανουμε free τον κομβο της λιστας
    listRemove(list_nodes, findNodeWithValue(list_nodes, node_to_remove), graphDestroyNode);


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

    incomingEdgesDestroy(node_to_destroy);
    outgoingEdgesDestroy(node_to_destroy);

    
    free(graph_node);
}

//κατα την καταστροφη του γραφου θελουμε να καταστρεφουμε καταλληλα
//το value καθε κομβου
void graphDestroy(Graph graph, DestroyValueFunc func){

    listDestroy(graph->nodes, graphDestroyNode);
    free(graph);
}




void graphDisplay(Graph graph){
    ListNode node;
    List list = graph->nodes;
    
    for(node = listFirst(list); node != NULL; node = listGetNext(node)) {
        
        GraphNode graph_node = listNodeValue(list, node);
        printf("User with id '%d':\n", graph_node->user);

        //ΠΡΕΠΕΙ ΝΑ ΕΚΤΥΠΩΝΩ ΚΑΙ ΤΙΣ ΑΚΜΕΣ (ΣΥΝΑΛΛΑΓΕΣ ΚΛΠ)

    }
}






/////////////// ΑΚΜΕΣ ///////////////
void edgeAdd(Graph graph, int amount, char* date, int source_user, int dest_user, HashTable hash_table){

    Edge new_edge = malloc(sizeof(struct edge));
    
    new_edge->amount = amount;
    new_edge->date = date;

    //αν οι χρηστες(κορυφες) με ονομα source_user, dest_user αντιστοιχα δεν υπαρχουν,
    //πρεπει να δημιουργηθουν
    if(hashFindGraphNodeWithKey(hash_table, source_user) == NULL) {
        printf("User with id '%d' added\n\n", source_user);
        graphAdd(graph, source_user, hash_table);
    }


    if(hashFindGraphNodeWithKey(hash_table, dest_user) == NULL) {
        printf("User with id '%d' added\n\n", dest_user);
        graphAdd(graph, dest_user, hash_table);
    }


    new_edge->dest_node = hashFindGraphNodeWithKey(hash_table, dest_user);
    new_edge->source_node = hashFindGraphNodeWithKey(hash_table, source_user);

    listInsert(new_edge->dest_node->incoming_edges, new_edge);
    listInsert(new_edge->source_node->outgoing_edges, new_edge);

    printf("new transaction: user '%d' -> user '%d', amount = '%d'\n\n", source_user, dest_user, amount);



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
}



void edgeRemove(Graph graph, int source_user, int dest_user, HashTable hash_table){
    
    GraphNode source = hashFindGraphNodeWithKey(hash_table, source_user);
    GraphNode dest = hashFindGraphNodeWithKey(hash_table, dest_user);
    
    Edge edge;
    
    if(source != NULL &&  dest!= NULL){
         edge = edgeFind(graph, source_user, dest_user, hash_table);
        
        ListNode node_to_remove = findNodeWithValue(source->outgoing_edges, edge);
        if(node_to_remove !=NULL) {
            listRemove(source->incoming_edges, node_to_remove, edgeDestroyValueForIncoming);
            listRemove(source->outgoing_edges, node_to_remove, edgeDestroyValueForOutgoing);
        }
    }
    
    //τωρα πρεπει να αφαιρεθει απο τις δυο λιστες το list node
    



}



//δεν πρεπει να καταστρεφουμε τα graph nodes source, dest
//αρκει να κανουμε free την ακμη, αφου δεν δεσμευουμε αλλη μνημη 
void edgeDestroyValueForOutgoing(Pointer value){
    // free(value); //οπου value ειναι το edge
    //πρεπει να γινει μονο σε μια απο τις δυο λιστες!!!!
}

void edgeDestroyValueForIncoming(Pointer value){

}





void incomingEdgesDestroy(GraphNode node){
    listDestroy(node->incoming_edges, edgeDestroyValueForIncoming);
}

void outgoingEdgesDestroy(GraphNode node){
    listDestroy(node->outgoing_edges, edgeDestroyValueForOutgoing);
}


