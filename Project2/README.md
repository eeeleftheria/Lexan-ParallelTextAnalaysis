# Parallel Text Analysis using Process Hierarchies `lexan`

This project implements a parallel text analysis system in C using a hierarchy of processes.
It focuses on process creation, inter-process communication (IPC), and synchronization using POSIX system calls.

The program analyzes a text file and computes the top-k most frequent words, excluding a given list of words.

## 🧠 Overview

The input file is split into parts. Multiple processes analyze different parts concurrently and results are aggregated to produce the final output.

The architecture consists of three types of processes:

- Root (lexan) – creates and coordinates all other processes
- Splitters – responsible for reading and preprocessing the input file
- Builders –  receive processed words from splitters and count their occurrences
 
## 🧩 Process Architecture

```
                  Root
                   |
    --------------------------------
    |       |        |       |    |
Splitter Splitter Splitter ... Splitter
   |         |        |           |
   ---------------------------------
   |        |        |            |
 Builder Builder Builder         ...
                   |
                  Root
```

This design enables parallel execution while maintaining a clear separation of responsibilities:

🔹 Root Process
- Creates l splitter processes using `fork()`
- Divides the input file into segments
- Passes execution parameters via `exec()`
- Creates m builder processes
- Manages communication between processes
- Collects and merges final results
- Sorts the words based on their frequencies
- Writes to the output file

🔹 Splitter Processes
- Read the assigned file segment
- Extract valid words
- Ignore punctuation, symbols, and digits
- Filter words using the exclusion list
- Send valid words to appropriate builder through pipes

🔹 Builder Processes
- Receive words from all splitters 
- Count word occurrences
- Store results in a hash table with the word as key and frequency as value
- Send results back to the root process through a pipe


## 🔗 Communication

### Splitters to Builders

When splitters extract a word, they send it to the appropriate builder through a pipe. A hash function is used to ensure that the same word is always routed to the same builder.

`word word word ... `


### Builders to Root

Builders send their results to the root process' pipe write end in the form:

`word:frequency-word:frequency-...`

This format allows the root to identify:

- the word itself
- its frequency
- where each record ends


## 📡 Signals and Synchronization

The system uses signals to indicate completion:

- SIGUSR1 → sent by splitters when they finish
- SIGUSR2 → sent by builders when they finish

The root process:

- counts received signals
- keeps track of completed workers
- ensures the entire hierarchy terminates correctly

This adds an asynchronous coordination mechanism on top of the pipe-based communication.

## 💡 Technical Highlights

This project demonstrates:

- process creation with `fork()`
- loading a new executable with `exec()`
- inter-process communication with pipes
- file descriptor manipulation with dup2()
- asynchronous signaling with SIGUSR1 and SIGUSR2
- parallel file processing
- hash-based workload distribution
- dynamic memory management
- incremental parsing of partial read() results


## 🚀 How to Run

### Compile

```bash
make
```

### Run 
The `Testing/` directory contains sample input files:

`ExclusionList1_a.txt`
`ExclusionList2_a.txt`
`Republic_a.txt`
`GreatExpectations_a.txt`
`WilliamShakespeareWorks_a.txt`

You can run some sample runs through:

```bash
make run1 
make run2
make run3
```

To run manually:
```bash
./lexan -i <input file> -l <num of splitters> -m <num of builders> -t <top words> -e < exclusion list> -o <output file>
```



## 🎓 Academic Context

This project was developed as part of the Operating Systems course K22 of the Department of Informatics and Telecommunications of UoA, with emphasis on:

- process hierarchies
- IPC mechanisms
- concurrency
- system-level programming