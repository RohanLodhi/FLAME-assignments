#!/bin/bash

./flame_cp "$@"

# Check the exit status of the program and print error messages accordingly
case $? in
    0) echo "File copied successfully.";;
    1) echo "Invalid number of arguments. Usage: flame_cp -i <source file pathname> -o <destination file pathname>";;
    2) echo "Error opening source file: No such file or directory.";;
    13) echo "Error opening destination file: Permission denied.";;
    5) echo "Error reading from source file: Input/output error.";;
    5) echo "Error writing to destination file: Input/output error.";;
    22) echo "Invalid arguments provided: Invalid argument.";;
    *) echo "Unknown error occurred.";;
esac