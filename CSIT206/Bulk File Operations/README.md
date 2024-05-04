# Bulk File System Operations : my_bfm.c

## Team Members: 
Rohan Lodhi, Nidhi

## Overview
This C program manipulates a given file or directory using system calls. The user can provide specifiers depending on what they want to achieve. 

Flags: 

1. -c creates a file depending on the path provided by the user

2. -s writes the text given by the user as a string to the end of the file at the given path

3. -d deletes the file or an empty directory found at the provided path

4. -r renames a given file or directory

5. -e writes 50 bytes of even numbers between 51 and 199 according to the starting point specified by the user

6. -l generates a log file and records the actions performed in file or directory found within the given directory

## Rationale
The program lets the user specify whether the given file is a binary or not because it is impossible for a C program to guarantee with 100% certainty that a file is binary. The program also calls the helper function by default to teach the user the usage of the program.

## Usage 

### Makefile

A Makefile (`Makefile`) is provided in the repository for easy compilation of the program.


Build the program using make in the source directory

    make 
(or) 

    gcc -o my_bfm my_bfm.c

### Running the Program:
Run the build file called "my_bfm"

    ./my_bfm -<flag <path> [data]

Replace <flag> with one of the supported flags (-c, -s, -e, -d, -s, -l), <path> with the file or directory path, and [data] with optional data for write operations.

Build the program using make in the source directory

    make

Run the build file called "my_bfm"

    ./my_bfm -<flag <path> [data]

Replace <flag> with one of the supported flags (-c, -s, -e, -d, -s, -l), <path> with the file or directory path, and [data] with optional data for write operations.


## Functions in our C-Program

## 1. Create

   The create functionality allows users to create files or directories. It checks if the specified path already exists and whether it's a file or directory before attempting to create it.

   **Usage:**

    ./my_fm -c <path>

`<path>`: The path where the file or directory will be created.

**Example Commands:**
    
    ./my_bfm -c ./new_file.txt


This command creates a new file named new_file.txt in the current directory.

    ./my_bfm -c new_directory 
This command creates a new directory named new_directory in the current directory.


## 2. Append
    
The append functionality allows users to append content to a file. The content can be a text string or a sequence of binary integers, depending on the provided parameters.

**Usage:**
    
    ./my_bfm -s <file_relative_path> <text_or_start_number>
    ./my_bfm -e <file_relative_path> <start_number>


`<file_relative_path>`: Path to the file to which content needs to be appended.

`<text_or_start_number>`: Text string to be appended for -s flag. Starting even number for the sequence of binary integers to be appended for the -e flag.

`<start_number>`: Starting even number for the sequence of binary integers to be appended for the -e flag.

**Example Commands:**

    ./my_bfm -s existing_file.txt "New text to append"
    ./my_bfm -e existing_file.txt 5


## 3. Deleting Files or Directories

The delete functionality allows users to delete files or directories. It checks for the existence and type of the specified path before attempting deletion.

If the path exists and is a directory, it uses the `<rmdir>` function to delete the directory. If the path exists and is a file, it uses the `<unlink>` function to delete the file. The function returns meaningful error codes as per the man page to indicate the cause of deletion failure, such as permission issues or non-existent paths.

**Usage:**
    
    ./my_bfm -d <file/directory_path>

`<file/directory_path>`: Path or name of the file or directory to be deleted.

**Example Commands:**
    
    ./my_bfm -d old_file.txt
    ./my_bfm -d old_directory

    

## 4. Renaming Files or Directories

The rename functionality allows users to rename files or directories. It checks for the existence and type of the specified old path before attempting renaming.

**Usage:**

    ./my_bfm -r <old_relative_path> <new_relative_path>

`<old_relative_path>`: Current path or name of the file or directory to be renamed.

`<new_relative_path>`: New path or name to assign to the file or directory.

**Example Commands:**

    ./my_bfm -r old_file.txt new_file.txt

    ./my_bfm -r old_directory new_directory

## 5. Log Action

The log_action function records actions performed on files or directories in a log file along with timestamps.


**Function Signature:**
void log_action(const char *log_file, const char *action, const char *path);

**Parameters:**
`log_file`: Path to the log file where actions are recorded.
`action`: Action performed (e.g., Created, Text Added, Deleted).
`path`: Path of the file or directory on which the action was performed.

**Function Description:**

Retrieves the current system time and formats it.
Opens the log file in append mode.
Writes the action along with the timestamp and path to the log file.
Closes the log file after writing the action.

**Example**

    ./my_bfm -c new_file.txt

This example records the action of creating a new file named "new_file.txt" in the log file "my_bfm_log.txt".

[2024-03-05 11:47:10] Created: ./hello.txt

## Checking Error Status:
To see the return code for the last process executed, errno codes are returned.

**Usage:**
    
    echo $?

**Example:**

    21

In the errno header file, 21 refers to "EISDIR" error, which means

Error: Directory already exists
 