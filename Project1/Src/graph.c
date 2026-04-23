#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "list.h"
#include "hash.h"
#include <string.h>


struct graph{
    List nodes; // the vertices/nodes of the graph
    int size;
};

struct graph_node{ 
    int user;
    List outgoing_edges; // stores the outgoing edges of the node
    List incoming_edges; // stores the incoming edges of the node
};


struct edge{
    int amount; // the transaction amount
    char* date; // transaction date (together with the amount they form the edge weight)
    GraphNode dest_node; // the destination of this edge
    GraphNode source_node; // the source of this edge
};


Graph graphCreate(){
    Graph graph = malloc(sizeof(struct graph));
    graph->nodes = listCreate();
    graph->size = 0;

    return graph;
}


void graphAdd(Graph graph, int user, HashTable hash_table){
    
    // we cannot add a user with the same id
    if(hashFindGraphNodeWithKey(hash_table, user) != NULL) {
        printf("User with id '%d' already exists\n", user);
        return;
    }
    
    GraphNode graph_node = malloc(sizeof(struct graph_node));

    graph_node->user = user;
    graph_node->outgoing_edges = listCreate();
    graph_node->incoming_edges = listCreate();

    // adding node to the list with the vertices
    List list_nodes = graph->nodes;
    listInsert(list_nodes, graph_node);

    // we will add the corresponding graph node to the hash_table that is given to us
    hashAdd(hash_table, user, graph_node);

    // update graph size
    graph->size ++;   
    
}


void graphRemove(Graph graph, int user, HashTable hash_table){
   
    List list_nodes = graph->nodes;
    
    // finding the graph node via hash for faster search
    GraphNode node_to_remove = hashFindGraphNodeWithKey(hash_table, user);

    // Important: Before deleting the node, remove all edges that reference it from other nodes
    // This prevents dangling pointers in edge structures
    
    // Step 1: Remove all outgoing edges from other nodes that point to this node
    // We need to iterate through all nodes and check their outgoing edges
    for(ListNode n = listFirst(list_nodes); n != NULL; n = listGetNext(n)) {
        GraphNode current_node = listNodeValue(list_nodes, n);
        
        if(current_node != node_to_remove) {
            // Check outgoing edges of current_node
            List outgoing = current_node->outgoing_edges;
            ListNode edge_node = listFirst(outgoing);
            
            while(edge_node != NULL) {
                ListNode next_edge_node = listGetNext(edge_node);
                Edge edge = listNodeValue(outgoing, edge_node);
                
                // If this edge points to the node we're removing, remove it
                if(edge->dest_node == node_to_remove) {
                    listRemove(outgoing, edge_node, edgeDestroyValueForOutgoing);
               
                    // Also remove from incoming list of the node being removed
                    ListNode incoming_node = findNodeWithValue(node_to_remove->incoming_edges, edge);
                    if(incoming_node != NULL) {
                        listRemove(node_to_remove->incoming_edges, incoming_node, edgeDestroyValueForIncoming);
                    }
                }
                
                edge_node = next_edge_node;
            }
        }
    }
    
    // Step 2: Remove all incoming edges to this node from other nodes
    // Iterate through all remaining outgoing edges of the node being removed
    ListNode outgoing_node = listFirst(node_to_remove->outgoing_edges);
    while(outgoing_node != NULL) {
        ListNode next_outgoing = listGetNext(outgoing_node);
        Edge edge = listNodeValue(node_to_remove->outgoing_edges, outgoing_node);
        GraphNode dest = edge->dest_node;
        
        // Remove this edge from destination's incoming list
        ListNode incoming_edge = findNodeWithValue(dest->incoming_edges, edge);
        if(incoming_edge != NULL) {
            listRemove(dest->incoming_edges, incoming_edge, edgeDestroyValueForIncoming);
        }
        
        outgoing_node = next_outgoing;
    }
    
    // remove first from the hash table, without freeing the graph node
    hashRemove(hash_table, user, hashDestroyValue);

    // we need to delete the value node_to_remove of the list node and free the list node
    listRemove(list_nodes, findNodeWithValue(list_nodes, node_to_remove), graphDestroyNode);

    // update the size of the graph
    graph->size--;

}

int graphSize(Graph graph){
    List list = graph->nodes;
    
    return listSize(list);
}


int graphGetUser(GraphNode graph_node){
    return graph_node->user;
}



// pointer to a function that destroys a graph node
// of type DestroyFunc
void graphDestroyNode(Pointer graph_node){
   
    GraphNode node_to_destroy = (GraphNode)graph_node;

    // we need to destroy both lists with its edges
    listDestroy(node_to_destroy->incoming_edges, edgeDestroyValueForIncoming);
    listDestroy(node_to_destroy->outgoing_edges, edgeDestroyValueForOutgoing);
    
    free(graph_node);
}

// when destroying the graph we want to properly destroy
// the value of each node
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

        // printing transactions/edges etc
        edgesOfNodeDisplay(graph, graph_node->user, table, output);
        
        fprintf(output, "\n\n");

    }
}


//#################################//
//############# EDGES #############//
//#################################//
void edgeAdd(Graph graph, int amount, char* date, int source_user, int dest_user, HashTable hash_table){

    Edge new_edge = malloc(sizeof(struct edge));
    
    new_edge->amount = amount;
    new_edge->date = malloc(strlen(date) + 1);
    strcpy(new_edge->date, date);

    // if the users (vertices) named source_user, dest_user respectively, 
    // do not exist they must be created
    if(hashFindGraphNodeWithKey(hash_table, source_user) == NULL) {
        graphAdd(graph, source_user, hash_table);
    }

    if(hashFindGraphNodeWithKey(hash_table, dest_user) == NULL) {
        graphAdd(graph, dest_user, hash_table);
    }

    // assignment of source graph node and dest graph node of the edge
    new_edge->dest_node = hashFindGraphNodeWithKey(hash_table, dest_user);
    new_edge->source_node = hashFindGraphNodeWithKey(hash_table, source_user);

    // adding to the corresponding list
    listInsert(new_edge->dest_node->incoming_edges, new_edge);
    listInsert(new_edge->source_node->outgoing_edges, new_edge);

}

// finding edge for specific source, dest
// since the same edge exists in the incoming edges list of the destination vertex
// but also in the outgoing edges list of the source edge, searching for it in one list is enough
Edge edgeFind(Graph graph, int source_user, int dest_user, HashTable hash_table){

    GraphNode source = hashFindGraphNodeWithKey(hash_table, source_user);
    GraphNode dest = hashFindGraphNodeWithKey(hash_table, dest_user);

    List list = source->outgoing_edges;
    ListNode node;
    for(node = listFirst(list); node != NULL; node = listGetNext(node)){

        Edge edge = listNodeValue(list, node);

        // if we find the edge with the given destination, return it
        if(edge->dest_node == dest) {
            return edge;
        }
    }   
    return NULL;  
}

// finding edge with specific source, dest, sum, date
Edge edgeFindWithAmountAndDate(Graph graph, int source_user, int dest_user, int sum, char* date,  HashTable hash_table){
    GraphNode source = hashFindGraphNodeWithKey(hash_table, source_user);
    GraphNode dest = hashFindGraphNodeWithKey(hash_table, dest_user);

    List list = source->outgoing_edges;
    ListNode node;
    for(node = listFirst(list); node != NULL; node = listGetNext(node)){

            Edge edge = listNodeValue(list, node);

            // if we find the edge with the given destination, return it
            if(edge->dest_node == dest && edge->amount == sum && (strcmp(date, edge->date) == 0)) {
                return edge;
            }
    }   
    return NULL;
}



// removing edge between two vertices
void edgeRemove(Graph graph, int source_user, int dest_user, HashTable hash_table){
    
    // finding the two vertices based on their id
    GraphNode source = hashFindGraphNodeWithKey(hash_table, source_user);
    GraphNode dest = hashFindGraphNodeWithKey(hash_table, dest_user);
    
    Edge edge;
    
    if(source != NULL &&  dest!= NULL){
        // finding an edge between them
         edge = edgeFind(graph, source_user, dest_user, hash_table);

         if(edge == NULL) {
            printf("transaction from user '%d' -> user '%d' not found\n", source_user, dest_user);
            return;
         }
        
        // removing the list node that contains this edge
        ListNode node_to_remove_incoming = findNodeWithValue(dest->incoming_edges, edge);
        if(node_to_remove_incoming != NULL) {
           
            listRemove(dest->incoming_edges, node_to_remove_incoming, edgeDestroyValueForIncoming);
        }

        // deleting edge and removing list node with this edge
        ListNode node_to_remove_outgoing = findNodeWithValue(source->outgoing_edges, edge);
        if(node_to_remove_outgoing != NULL) {

            listRemove(source->outgoing_edges, node_to_remove_outgoing, edgeDestroyValueForOutgoing);
        }
    } 
            

}



// we should not destroy the graph nodes source, dest
// it is enough to free the edge, since we do not allocate other memory
void edgeDestroyValueForOutgoing(Pointer value){
    Edge edge = value;
    free(edge->date);
    free(value); // where value is the edge
    // it must be done only on one of the two lists!!!!
}

void edgeDestroyValueForIncoming(Pointer value){

}




// destruction of list with incoming edges
void incomingEdgesDestroy(GraphNode node){
    listDestroy(node->incoming_edges, edgeDestroyValueForIncoming);
}

// destruction of list with outgoing edges
void outgoingEdgesDestroy(GraphNode node){
    listDestroy(node->outgoing_edges, edgeDestroyValueForOutgoing);
}


// printing edges of a vertex
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


// printing outgoing edges of a vertex
void edgesOutgoingOfNodeDisplay(Graph graph, int user, HashTable table){
    
    GraphNode graph_node = hashFindGraphNodeWithKey(table, user);
    List list_out = graph_node->outgoing_edges;

    printf("Outgoing edges of user '%d' are: \n", user);
    printf("from   to    amount       date\n");

    ListNode node;

    // we traverse the list with the outgoing edges of the node and print
    // all the information of its edges
    for (node = listFirst(list_out); node != NULL; node = listGetNext(node)){
        
        Edge edge = listNodeValue(list_out, node);
        int source = edge->source_node->user;
        int dest = edge->dest_node->user;

        printf("  %d    %d      %d      %s\n", source, dest, edge->amount, edge->date);
    }

}

// printing incoming edges of a vertex
void edgesIncomingOfNodeDisplay(Graph graph, int user, HashTable table){
    
    GraphNode graph_node = hashFindGraphNodeWithKey(table, user);
    List list_out = graph_node->incoming_edges;

    printf("Incoming edges of user '%d' are: \n", user);
    printf("from   to    amount       date\n");

    ListNode node;

    // we traverse the list with the incoming edges of the node and print
    // all the information of its edges
    for (node = listFirst(list_out); node != NULL; node = listGetNext(node)){
        
        Edge edge = listNodeValue(list_out, node);
        int source = edge->source_node->user;
        int dest = edge->dest_node->user;

        printf("  %d    %d      %d      %s\n", source, dest, edge->amount, edge->date);
    }

}


// modification of edge
int edgeModify(Graph graph, HashTable table, int source, int dest, int old_sum, int new_sum, char* old_date, char* new_date){
    
    // finding first edge with specific transaction amount
    Edge edge = edgeFindWithAmountAndDate(graph, source, dest, old_sum, old_date, table);    

    if(edge != NULL) {
        free(edge->date);
        edge->date = malloc(strlen(new_date) + 1);
        strcpy(edge->date, new_date);

        edge->amount = new_sum;

        return 1;

    }

    return 0;
    
}


void graphFindCircle(Graph graph, int user, HashTable table, List visited){
    GraphNode node = hashFindGraphNodeWithKey(table, user);
    List list = node->outgoing_edges;

    for(ListNode node = listFirst(list); node != NULL; node = listGetNext(node)){
        Edge edge = listNodeValue(list, node);
        GraphNode neighbor = edge->dest_node;

        // if the node has not been visited
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



