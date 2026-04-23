# Dynamic Transaction Graph

This project implements a dynamic transaction system using a directed graph, written in C.  
It focuses on efficient data management, memory handling, and graph operations using custom-built data structures.

The system simulates users and transactions between them, supporting real-time updates through a command-line interface.


## 🧠 Core Idea

Each user is represented as a node in a directed graph, and each transaction is an edge between two users.

The graph supports:
- dynamic insertion and deletion of users
- multiple transactions between the same users (multigraph)
- efficient lookup using a hash table
- real-time querying via a command-line interface


## 🧩 Data Structures

### 🔹 Doubly Linked List

The list is implemented in a generic way (`void*` values) since it can store any type of data throughout the project.

```C
struct list{
    ListNode first;
    ListNode last;
    int size;
};
```
```C
struct list_node{
    Pointer value;
    ListNode next;
    ListNode prev;
};
```

The list:
- Stores nodes and edges
- Supports insertion at the end of the list and deletion in any place
- Provides safe access through getters (encapsulation)

**Complexities:**
- Insert / Remove: `O(1)`
- Size / Access: `O(1)`
- Find: `O(n)`


### 🔹 Graph (Adjacency Lists)

The graph is implemented using adjacency lists.

```C
struct graph{
    List nodes; // the nodes of the graph
    int size;
};
```

```C
struct graph_node{ 
    int user;
    List outgoing_edges; // stores the outgoing edges of the node
    List incoming_edges; // stores the incoming edges of the node
};
```

Each node maintains:
- a list of outgoing edges
- a list of incoming edges

This design allows:
- efficient edge traversal
- fast deletion of nodes and all associated edges

**Key operations:**
- Add / Remove node
- Add / Remove edge
- Modify edge
- Query incoming / outgoing edges


### 🔹 Hash Table

Used for fast lookup of users.

- Implements **separate chaining**
- Each bucket contains a list of nodes (graph nodes)
- Hash function: `key % table_size`

This enables near `O(1)` access to graph nodes.


## ⚙️ System Design Highlights

- Directed **multigraph** (multiple edges between same nodes allowed)
- Separation of concerns:
  - List → storage
  - Graph → logic
  - Hash table → fast access
- Careful memory management:
  - no double frees
  - controlled destruction of shared edges
- Explicit handling of incoming vs outgoing edges


## 💾 Memory Management

Special attention is given to proper memory handling:

- Each edge is stored in both incoming and outgoing lists  
- It must be freed **only once** to avoid double free errors  
- When deleting a node all incoming and outgoing edges associated to that node must be freed.

The destruction process:
1. Destroy edges
2. Destroy adjacency lists
3. Destroy nodes
4. Destroy hash table


## 🚀 How to Run

### Compile

```bash
make
```

### Run 
With one of the commands below you can run with Valgrind any of the 5 inputs provided (`\Input`).
```bash
make run1
make run2
make run3
make run4
make run5
```

To run manually:
```bash
./miris -i <input file> -o <output file>
```

An output file has been provided with the name `output.txt` which captures the state of the graph as it evolves 
and reflects its final form upon completion of the program.

The program provides also an interactive command-line interface that allows users to manage and query the graph in real time.

### Available commands
```bash
i Ni [Nj Nk ...] Insert node(s)
n Ni Nj amount date Insert edge
d Ni [Nj Nk ...] Delete node(s)
l Ni Nj Delete edge
m Ni Nj sum sum1 date date1 Modify edge
f Ni Find outgoing edges
r Ni Find incoming edges
e Exit
```

### Example execution
```bash
$ ./miris -i Input/data8-3.txt -o output.txt

i 1 15
User 1 already exists
Successful insertion of user: 15

n 1 15 100 2024-01-01
Added transaction from '1' to '15'

n 2 3 200 2024-02-02
Added transaction from '2' to '3'

f 1
Outgoing edges of user '1' are: 
from   to    amount       date
  1    15      100      2024-01-01

r 3
Incoming edges of user '3' are: 
from   to    amount       date
  2    3      3900      2024-04-06
  4    3      3400      2024-06-15
  8    3      4200      2024-08-16
  2    3      200      2024-02-02

e
```
