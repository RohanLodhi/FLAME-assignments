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

#include "process_details.h"

#define SLEEP_INTERVAL 5
#define PROC_DIR "/proc"
#define BUF_SIZE 1024

// Helper function to get username from UID
char *username_from_uid(uid_t uid) {
    struct passwd *pw = getpwuid(uid);
    if (pw) {
        return pw->pw_name;
    }
    return "unknown";
}

void list_all_processes() {
    int pipefd[2];
    pid_t pid;

    // Create a pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork the process
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process: Connect the read-end of the pipe to stdin and run less
        close(pipefd[1]);  // Close the unused write end of the pipe
        dup2(pipefd[0], STDIN_FILENO);  // Duplicate the read end to stdin
        close(pipefd[0]);  // Close the original read end

        // Execute less
        execlp("less", "less", (char *)NULL);
        perror("execlp");
        _exit(EXIT_FAILURE);  // If execlp fails, exit
    }

    // Parent process: Write to the write-end of the pipe
    close(pipefd[0]);  // Close the unused read end of the pipe
    FILE *stream = fdopen(pipefd[1], "w");
    if (stream == NULL) {
        perror("fdopen");
        exit(EXIT_FAILURE);
    }

    // Open /proc directory
    DIR *dir = opendir("/proc");
    if (dir == NULL) {
        perror("Failed to open /proc");
        exit(EXIT_FAILURE);
    }

    // Print header to the stream
    fprintf(stream, "%-8s %-10s %-6s %-10s %s\n", "PID", "USER", "STAT", "TIME", "COMMAND");

    // Read directory entries in /proc
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (isdigit(entry->d_name[0])) {  // Ensure it's a process directory
            int pid = atoi(entry->d_name);
            get_process_info(pid, stream);  // Fetch and print process info
        }
    }
    closedir(dir);

    fflush(stream);  // Make sure all output is sent to less
    fclose(stream);  // Close the stream
    close(pipefd[1]);  // Close the write end to allow less to finish
    wait(NULL);  // Wait for the child process to finish
}

void get_process_info(int pid, FILE *stream) {
    char path[256];
    char buffer[BUF_SIZE];
    int fd;
    ssize_t nread;

    // Open and read from /proc/[pid]/status to get UID and Status
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    fd = open(path, O_RDONLY);
    if (fd == -1) return;
    
    nread = read(fd, buffer, BUF_SIZE - 1);
    if (nread > 0) {
        buffer[nread] = '\0';
        char *line = strtok(buffer, "\n");
        char state[32];
        uid_t uid = -1;
        while (line != NULL) {
            if (strncmp(line, "Uid:", 4) == 0) {
                sscanf(line, "Uid:\t%d", &uid);
            } else if (strncmp(line, "State:", 6) == 0) {
                sscanf(line, "State:\t%31s", state);
            }
            line = strtok(NULL, "\n");
        }
        char *username = username_from_uid(uid);
        
        // Read command line
        snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
        close(fd);
        fd = open(path, O_RDONLY);
        if (fd != -1) {
            nread = read(fd, buffer, BUF_SIZE - 1);
            if (nread > 0) {
                buffer[nread] = '\0';
                for (int i = 0; i < nread; i++) {
                    if (buffer[i] == '\0') buffer[i] = ' '; // Replace nulls with spaces
                }
            }
        }

        // Output process info
        fprintf(stream, "%-8d %-10s %-6s %s\n", pid, username, state, buffer);
    }
    if (fd != -1) close(fd);
}

void process_info(int pid) {
    char path[256];
    int fp;
    char line[1024];
    uid_t uid;
    char state[32];
    unsigned long utime, stime;
    float cputime;
    char cmdline[4096] = {0};

    printf("Showing information for PID %d:\n", pid);

    // Read and display user and state from /proc/[pid]/status
    sprintf(path, "/proc/%d/status", pid);
    if ((fp = open(path, O_RDONLY)) != -1) {
        while (read(fp, line, sizeof(line)) != 0) {
            if (strncmp(line, "Uid:", 4) == 0) {
                sscanf(line, "Uid:\t%u", &uid);
                printf("User: %d\n", uid);
            }
            if (strncmp(line, "State:", 6) == 0) {
                sscanf(line, "State:\t%31s", state);
                printf("State: %s\n", state);
            }
        }
        close(fp);
    }

    // Read and calculate CPU time from /proc/[pid]/stat
    sprintf(path, "/proc/%d/stat", pid);
    if ((fp = open(path, O_RDONLY)) != -1) {
        if (read(fp, line, sizeof(line)) != 0) {
            sscanf(line, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu", &utime, &stime);
        }
        close(fp);
    }
    cputime = (utime + stime) / sysconf(_SC_CLK_TCK);
    printf("CPU Time: %.2f seconds\n", cputime);

    // Print process status
    sprintf(path, "/proc/%d/status", pid);
    if ((fp = open(path, O_RDONLY)) != -1) {
        printf("Status Information:\n");
        while (read(fp, line, sizeof(line)) != 0) {
            printf("%s", line);
        }
        close(fp);
    } else {
        printf("Could not open %s\n", path);
    }
}

int get_pgid_from_stat(int pid) {
    char path[256];
    sprintf(path, "/proc/%d/stat", pid);
    FILE* file = fopen(path, "r");
    if (!file) return -1;
    
    int pgid;
    fscanf(file, "%*d %*s %*c %*d %d", &pgid);
    fclose(file);

    return pgid;
}

// Lists all processes that share the same group as the target process
void group_processes(int target_pid) {
    int target_pgid = get_pgid_from_stat(target_pid);
    if (target_pgid == -1) {
        printf("Failed to get pgid for PID %d\n", target_pid);
        return;
    }

    DIR *dir = opendir("/proc");
    if (!dir) {
        perror("Failed to open /proc");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (entry->d_type == DT_DIR && isdigit(entry->d_name[0])) {
            int pid = atoi(entry->d_name);
            int pgid = get_pgid_from_stat(pid);
            if (pgid == target_pgid) {
                printf("PID %d is in group %d\n", pid, target_pgid);
            }
        }
    }
    closedir(dir);
}

// Utility function to extract UID from /proc/[pid]/status
int get_uid_from_status(int pid, int effective) {
    char path[256], line[1024];
    sprintf(path, "/proc/%d/status", pid);
    FILE* file = fopen(path, "r");
    if (!file) return -1;
    
    int uid = -1;
    while (fgets(line, sizeof(line), file)) {
        if (effective && strncmp(line, "Uid:", 4) == 0) {
            sscanf(line, "Uid:\t%d", &uid);
            break;
        }
        if (!effective && strncmp(line, "Euid:", 5) == 0) {
            sscanf(line, "Euid:\t%d", &uid);
            break;
        }
    }
    fclose(file);

    return uid;
}

// Lists all processes running under a specific UID
void user_processes(uid_t uid, int is_effective) {
    DIR *dir = opendir("/proc");
    if (!dir) {
        perror("Failed to open /proc");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (entry->d_type == DT_DIR && isdigit(entry->d_name[0])) {
            int pid = atoi(entry->d_name);
            int process_uid = get_uid_from_status(pid, is_effective);
            if (process_uid == uid) {
                printf("PID %d is run by UID %d\n", pid, uid);
            }
        }
    }
    closedir(dir);
}