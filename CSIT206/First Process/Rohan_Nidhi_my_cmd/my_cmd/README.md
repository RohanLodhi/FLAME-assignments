# First Process : my_cmd.c

## Team Members: 
Rohan Lodhi, Nidhi

## Overview
This C program runs a binary executable file provided via the command line. It allows users to specify additional command line options for the executable and choose whether to run the process in the background or foreground. Upon completion of the binary execution, the program prints relevant information such as the command line received, process ID of the child (binary), exit code of the child (if any), and error codes (if any) of the program.


## Usage 
The command line structure for running the program is as follows:

    ./my_cmd -e <pathname of an executable> -o "String of command line options of the executable" [-b | -f]

- `-e`: Specifies the pathname of the executable file. This option is compulsory.
- `-o`: Provides a string of command line options for the executable. This option is not always required.
- `-b`: Runs the process in the background.
- `-f`: Runs the process in the foreground.

Note: [-b | -f] is used to control the process - either "run in the background, -b", or "run in the foreground, -f", but never both and never none.

## Implementation Details
- The program parses command line arguments to extract the executable pathname and options, as well as to determine whether to run the process in the background or foreground.

- It uses the `fork()` system call to create a child process, and the `execvp()` or `execl()` system call to execute the specified binary with the provided options.

- The parent process waits for the child process to finish if it's running in the foreground, and prints relevant information after the execution completes.

- Error handling is implemented to deal with failures in process creation, execution, and argument parsing.


### Background (-b) vs. Foreground (-f)
When `-b` (background) option is chosen, the parent process does not wait for the child process to finish. Instead, it proceeds with its execution independently, printing a message indicating that the process is running in the background.
When `-f` (foreground) option is chosen, the parent process waits for the child process to finish using `waitpid()` before printing relevant information about the execution.

## Compilation
A Makefile (Makefile) is provided to facilitate compilation:
    
    make

Alternatively, you can compile using gcc:
    gcc -o my_cmd my_cmd.c

### Example Usage
    ./my_cmd -e  <pathname of an executable> -o "String of command line options of the executable" [-b | -f ]

This command runs the executable located at `<path of an execuatble>` with the specified options in the foreground or background. 


## Execution
When the binary finishes its execution, then the following information is printed on the standard output.

- The full command line that the C program receives for execution. 
- Process ID of the child, i.e. the binary.
- Exit code of the child, if any.
- Error codes, if any, of the C program. (The error code of the child is the same as the exit code).

### Example

    The full command line: ./my_fm ./my_fm create text.txt 0
    Process ID of the child: 14034
    Exit code of the child: 0
    Error codes of the parent: 0
