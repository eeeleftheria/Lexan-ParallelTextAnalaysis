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

void graphRemove(Graph graph, int user){
   
    List list_nodes = graph->nodes;
    int* p = &user;
    listRemove(list_nodes, findNodeWithValue(list_nodes, p));

    graph->size--;

}

int graphSize(Graph graph){
    List list = graph->nodes;
    
    return listSize(list);
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


bool graphContainsNode(Graph graph, GraphNode node){
    if(node != NULL) {
        List list = graph->nodes;
        
    }
}



int graphGetUser(GraphNode graph_node){
    return graph_node->user;
}



/////////////// ΑΚΜΕΣ
void edgeAdd(Graph graph, int amount, char* date, GraphNode source_node, GraphNode dest_node){

    Edge new_edge = malloc(sizeof(struct edge));

    new_edge->amount = amount;
    new_edge->date = date;
    new_edge->dest_node = dest_node;

    // List list = source_node->neighbors;


}

void incomigEdgeDestroyValue(){

}


void outgoingEdgeDestroyValue(){

}


void incomingEdgesDestroy(GraphNode node){
    listDestroy(node->incoming_edges, incomigEdgeDestroyValue);
}

void outgoingEdgesDestroy(GraphNode node){
    listDestroy(node->outgoing_edges, outgoingEdgeDestroyValue);
}


