#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define E_OK 0
#define E_INVALID_ARGS 1
#define E_EXEC_FAILURE 2
#define E_FORK_FAILURE 3

char *EXEC_PATH = NULL;
char *EXEC_OPTIONS = NULL;
int run_in_background = 0;

void print_usage() {
    printf("Usage: ./program -e <pathname of an executable> -o \"String of command line options of the executable\" [-b | -f]\n");
}

void parse_arguments(int argc, char *argv[]) {
    int opt;

    while ((opt = getopt(argc, argv, "e:o:bf")) != -1) {
        switch (opt) {
            case 'e':
                EXEC_PATH = optarg;
                break;
            case 'o':
                EXEC_OPTIONS = optarg;
                break;
            case 'b':
                run_in_background = 1;
                break;
            case 'f':
                run_in_background = 0;
                break;
            default:
                print_usage();
                exit(E_INVALID_ARGS);
        }
    }

    if (EXEC_PATH == NULL) {
        print_usage();
        exit(E_INVALID_ARGS);
    }
}

int main(int argc, char *argv[]) {
    parse_arguments(argc, argv);

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(E_FORK_FAILURE);
    } else if (pid == 0) { // Child process
        if (EXEC_OPTIONS != NULL) {
            char *token;
            char *args[100]; // Assuming maximum 100 arguments
            int i = 0;
            token = strtok(EXEC_OPTIONS, " ");
            while (token != NULL) {
                args[i++] = token;
                token = strtok(NULL, " ");
            }
            args[i] = NULL;
            execvp(EXEC_PATH, args);
        } else {
            execl(EXEC_PATH, EXEC_PATH, (char *)NULL);
        }
        perror("execvp");
        exit(E_EXEC_FAILURE);
    } else { // Parent process
        if (!run_in_background) {
            int status;
            waitpid(pid, &status, 0);
            printf("The full command line: %s %s\n", EXEC_PATH, EXEC_OPTIONS != NULL ? EXEC_OPTIONS : "");
            printf("Process ID of the child: %d\n", pid);
            if (WIFEXITED(status)) {
                printf("Exit code of the child: %d\n", WEXITSTATUS(status));
            } else {
                printf("Child process terminated abnormally\n");
            }
            printf("Error codes of the parent: %d\n", E_OK);
        } else {
            printf("The process is running in the background.\n");
        }
    }

    return E_OK;
}