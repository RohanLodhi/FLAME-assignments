# Inter-process Communication : my_ipc.c

## Team Members: 
Rohan Lodhi, Nidhi

## Overview
This C program facilitates the execution of two separate commands, a generator command, and a consumer command, with the output of the generator piped into the input of the consumer. It allows users to specify the generator and consumer commands via command line arguments. Upon completion of both commands, the program waits for their execution to finish and then exits.


## Usage 
The command line structure for running the program is as follows:

    ./my_ipc -g [generator_command] -c [consumer_command]


- `-g`: Specifies the generator command to be executed.
- `-c`: Specifies the consumer command to be executed.

Note: Both -g and -c options are compulsory.

## Implementation Details
- The program parses command line arguments to extract the generator and consumer commands.

- It uses the `fork()` system call to create two child processes, one for the generator and one for the consumer.

- A pipe is created to establish communication between the generator and consumer processes.

- The generator's standard output is redirected to the write end of the pipe, and the consumer's standard input is redirected to the read end of the pipe.

- The generator and consumer commands are executed in their respective child processes using execvp().

- The parent process waits for both child processes to finish using waitpid()


## Compilation
The provided Makefile can be used to compile the program using gcc:
    
    make

Alternatively, you can compile using gcc:
    gcc -o my_ipc my_ipc.c


### Example Usage
    ./my_ipc -g "ls -l" -c "wc -l"

This command executes the ls -l command as the generator and wc -l command as the consumer. The output of ls -l is piped into the input of wc -l, which counts the number of lines in the output of ls -l. 


## Technical Implementation

- The program uses the pipe() system call to create a unidirectional communication channel between the generator and consumer processes. This approach ensures efficient data transfer between the processes.

- Forking two child processes allows the generator and consumer commands to execute concurrently, improving overall program performance.

- Redirecting standard input and output using dup2() ensures seamless communication between the generator and consumer processes, eliminating the need for intermediary files or manual data transfer mechanisms.

- The execvp() system call is used to execute the generator and consumer commands, providing flexibility and simplicity in command execution.

- Error handling mechanisms are implemented to deal with failures in process creation, pipe creation, and command execution, ensuring the robustness and reliability of the program.

