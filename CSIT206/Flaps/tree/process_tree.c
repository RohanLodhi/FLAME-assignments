#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <ctype.h>
#include <pwd.h>

#include "process_tree.h"

ProcessNode* head = NULL;  // Head of the flat list of all processes

ProcessNode* add_process(int pid, int ppid, const char* name) {
    ProcessNode* new_node = (ProcessNode*)malloc(sizeof(ProcessNode));
    new_node->pid = pid;
    new_node->ppid = ppid;
    strcpy(new_node->name, name);
    new_node->next = head;
    new_node->child = NULL;
    new_node->sibling = NULL;
    head = new_node;
    return new_node;
}

void build_tree() {
    for (ProcessNode* p = head; p != NULL; p = p->next) {
        if (p->ppid != 0) {  // Ignore the root process
            ProcessNode* parent = find_process_by_pid(p->ppid);
            if (parent) {
                // Insert as the first child
                p->sibling = parent->child;
                parent->child = p;
            }
        }
    }
}

ProcessNode* find_process_by_pid(int pid) {
    for (ProcessNode* p = head; p != NULL; p = p->next) {
        if (p->pid == pid) {
            return p;
        }
    }
    return NULL;
}

void print_branches(int depth) {
    for (int i = 0; i < depth; i++) {
        if (i == depth - 1)
            printf("├─");
        else
            printf("| ");
    }
}

void print_tree(ProcessNode* node, int depth) {
    if (node == NULL)
        return;

    // Print the current node
    print_branches(depth);
    printf("%s(%d)\n", node->name, node->pid);

    // Recursively print each child
    ProcessNode* child = node->child;
    while (child) {
        print_tree(child, depth + 1);
        child = child->sibling;
    }

    // Print a connector to separate siblings more clearly if needed
    if (node->sibling) {
        for (int i = 0; i < depth; i++) {
            printf("| ");
        }
        printf("|\n");
    }
}

void read_proc() {
    // Open the /proc directory using opendir
    DIR *dir = opendir("/proc");
    if (dir == NULL) {
        perror("Failed to open /proc");
        return;
    }

    // Declare the dirent structure for reading the directory entries
    struct dirent *entry;

    // Loop through all entries in the directory
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && isdigit(entry->d_name[0])) {
            int pid = atoi(entry->d_name);
            if (pid > 0) {
                char path[256];
                snprintf(path, sizeof(path), "/proc/%d/stat", pid);
                int fd = open(path, O_RDONLY);
                if (fd != -1) {
                    char buffer[1024];
                    int bytes_read = read(fd, buffer, sizeof(buffer) - 1);
                    if (bytes_read > 0) {
                        buffer[bytes_read] = '\0'; // Null-terminate the string
                        int ppid;
                        char name[256];
                        // Extract PID, PPID, and the command name from the buffer
                        if (sscanf(buffer, "%*d (%255[^)]) %*c %d", name, &ppid) == 2) {
                            // Add this process to your process list or print it
                            add_process(pid, ppid, name);
                            printf("PID: %d, PPID: %d, Name: %s\n", pid, ppid, name);
                        }
                    }
                    close(fd); 
                }
            }
        }
    }

    // Close the directory stream after finishing the reading
    closedir(dir);
}