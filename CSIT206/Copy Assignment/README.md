# File Copy Program

## About 
Author: Rohan Lodhi

This C program copies the contents of a given file to another using system calls. The user can even provide the input or output through the standard input/output streams. In case the program fails, there is a shell script that details out the error that was received during run time. This shell script runs the program itself and has to be provided the same arguments as the c executable. 

Flags: 

1. -i represents the input stream which should be followed by either the path to the file with the contents or "-" in case the user wants to use the standard input to copy contents from.

2. -o represents the output stream which should be followed by either the path to the file where the input should be copied to or "-" in case the user wants to use the standard output to copy contents to.

## Usage 

Build the program using make in the source directory

> make

Run the build file called "flame_cp"

> ./flame_cp  -i "source file pathname" -o "destination file pathname"

Provide executable permissions to the shell script

> chmod +x ./error_handling.sh

Run the shell script 

> ./error_handling.sh -i "source file pathname" -o "destination file pathname"