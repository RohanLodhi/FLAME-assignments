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

#include "./processes/process_details.h"
#include "./tree/process_tree.h"

#define SLEEP_INTERVAL 5
#define PROC_DIR "/proc"
#define BUF_SIZE 1024

#define E_OK 0
#define E_INCORRECT_ARGS -1

void helper();

int main(int argc, char *argv[]) {
    int opt;
    int pid;
    uid_t uid;
    int is_effective = 0;

    if (argc == 1) {
        list_all_processes();
        return 0;
    }

    while ((opt = getopt(argc, argv, "a:tg:u:U:")) != -1) {
        switch (opt) {
            case 'a':
                pid = atoi(optarg);
                process_info(pid);
                break;
            case 't':
                read_proc();
                build_tree();
                ProcessNode* root = find_process_by_pid(1); // Assuming PID 1 is the init process
                if (root) {
                    print_tree(root, 0);
                }
                break;
            case 'g':
                pid = atoi(optarg);
                group_processes(pid);
                break;
            case 'u':
                uid = atoi(optarg);
                user_processes(uid, 0);
                break;
            case 'U':
                uid = atoi(optarg);
                user_processes(uid, 1);
                break;
            default: 
                helper();
                exit(EXIT_FAILURE);
        }
    }

    return E_OK;
}

void helper() {
    printf("Usage: ./flaps [options]\n");
    printf("Options:\n");
    printf("  -a <pid>    Display all available information about the process with the given PID.\n");
    printf("  -t          Print a tree of processes from the current session leader.\n");
    printf("  -g <pid>    Show all processes that are in the same group as the process with the given PID.\n");
    printf("  -u <uid>    Display all processes that are executing on behalf of the user with real user ID <uid>.\n");
    printf("  -U <uid>    Display all processes that are executing on behalf of the user with effective user ID <uid>.\n");
    printf("\n");
    printf("Examples:\n");
    printf("  ./flaps -a 1234    Shows detailed information for the process with PID 1234.\n");
    printf("  ./flaps -t         Displays a tree of all processes related to the current session leader.\n");
    printf("  ./flaps -g 1234    Lists all processes in the same group as the process with PID 1234.\n");
    printf("  ./flaps -u 1001    Lists all processes running under the real UID 1001.\n");
    printf("  ./flaps -U 1001    Lists all processes running under the effective UID 1001.\n");
}