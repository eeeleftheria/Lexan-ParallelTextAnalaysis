#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "list.h"
#include <string.h>


struct graph{
    List nodes; //οι κορυφες/κομβοι του γραφου
    int size;
};

struct graph_node{ 
    char* user;
    List neighbors; //η λιστα καθε κορυφης με τις γειτονικες της
};


struct edge{
    int amount; //το ποσο συναλλαγης
    char* date; //ημερομηνια συναλλαγης (μαζι με το ποσο αποτελουν το βαρος της ακμης)
    GraphNode dest_node; //ο προορισμος της συγκεκριμενης ακμης
};


Graph graphCreate(){
    Graph graph = malloc(sizeof(struct graph));
    graph->nodes = listCreate();
    graph->size = 0;

    return graph;
}


void graphAdd(Graph graph, char* user){
    GraphNode graph_node = malloc(sizeof(struct graph_node));

    graph_node->user = malloc(strlen(user + 1)); //me to \0
    strcpy(graph_node->user, user);
    // graph_node->user = user; oxi etsi!!!!!!!1

    graph_node->neighbors = listCreate();
    
    List list_nodes = graph->nodes;
    listInsert(list_nodes, graph_node);

    graph->size ++;   
    
}

// void graphRemove(Graph graph, GraphNode graph_node){
//     List list_nodes = graph->nodes;
//     listRemove(list_nodes, findNode(list_nodes, graph_node));

//     graph->size--;

// }

int graphSize(Graph graph){
    List list = graph->nodes;
    
    return listSize(list);
}

//καταστροφη ενος graph node
void graphDestroyNode(GraphNode graph_node){
    free(graph_node->user);
    listDestroy(graph_node->neighbors);
    free(graph_node);
}

//καταστροφη του γραφου
void graphDestroy(Graph graph){

    listDestroy(graph->nodes);
    free(graph);
}

bool graphContainsNode(Graph graph, GraphNode node){
    if(node != NULL) {
        List list = graph->nodes;
        
    }
}

/////////////// ΑΚΜΕΣ
void addEdge(Graph graph, int amount, char* date, GraphNode source_node, GraphNode dest_node){

    Edge new_edge = malloc(sizeof(struct edge));

    new_edge->amount = amount;
    new_edge->date = date;
    new_edge->dest_node = dest_node;

    List list = source_node->neighbors;


}
