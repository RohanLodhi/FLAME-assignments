# File Operations Program

## About 
Author: Rohan Lodhi

This C program manipulates a given file using system calls. The user can provide specifiers depending on what they want to achieve. 

Flags: 

1. -c creates a file depending on the path provided by the user

2. -w writes the data given by the user to the destination path

3. -r it reads 50 bytes of data of the given file and dumps it on the standard output

4. -d deletes the file or an empty directory

5. -R renames a given file

6. -b writes 50 bytes of odd numbers between 50 and 200 according to the starting point specified by the user

## Rationale
The program lets the user specify whether the given file is a binary or not because it is impossible for a C program to guarantee with 100% certainty that a file is binary. The program also calls the helper function by default to teach the user the usage of the program.

## Usage 

Build the program using make in the source directory

> `make`

Run the build file called "my_fm"

> `./my_fm -<flag <path> [data]`

Replace <flag> with one of the supported flags (-c, -w, -r, -d, -R, -b), <path> with the file or directory path, and [data] with optional data for write operations.
