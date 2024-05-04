# Flaps

## Overview
This C program replicates the `top` and `ps` linux commands and adds on to their functionality. It lists out the running processes on a GNU/Linux system. Since the program makes use of `/proc`, which is not a POSIX requirement, the portability of the program can not be guaranteed to all UNIX like systems. However, if the machine uses /proc, the program can also list out process trees, groups, details per process etc. 

## Build
The program ships with a Makefile that can be used to directly build the project
```
make
```
Alternatively, you can also use gcc to build the files seperately:
```bash
gcc -c flaps.c -o flaps.o

gcc -c processes/process_details.c -o processes/process_details.o

gcc -c tree/process_tree.c -o tree/process_tree.o

gcc flaps.o processes/process_details.o tree/process_tree.o -o flaps
```

## Command Line Structure
The usage for running the flaps program would be structured as follows:

```bash
./flaps [options] 
```

Options

* -a <pid>: Displays detailed information about a process with the specified process ID (PID).
* -t: Displays a tree of processes starting from the current session leader.
* -g <pid>: Lists all processes that are in the same group as the process with the specified PID.
* -u <uid>: Lists all processes running under the specified real user ID (UID).
* -U <uid>: Lists all processes running under the specified effective user ID (UID).

## Usage Details

```
Usage: ./flaps -a [pid] | -t | -g [pid] | -u [uid] | -U [uid]
```

## Implementation Details
- The program parses the command line input to decide behaviour based on the switch provided while running. 
- The source code is structured on the basis of functionality, which is mainly divided on the basis of process details vs tree structuring. 
- The process_tree.c file assumes that the tree has init as its main node. If we want a tree specific to a certain process, we can also change the pid in the same function to give us a tree of the corresponding session leader. 
- To list out all of the process details with pagenation, we make use of `less`. The output of the program is redirected to less to provide a pagenated view. 
