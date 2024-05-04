#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 100

int main(int argc, char *argv[]) {
    char *generator_command[MAX_COMMAND_LENGTH];
    char *consumer_command[MAX_COMMAND_LENGTH];
    int generator_argc = 0;
    int consumer_argc = 0;
    int i;

    // Check if both -g and -c are present
    int has_g = 0, has_c = 0;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-g") == 0) {
            has_g = 1;
        } else if (strcmp(argv[i], "-c") == 0) {
            has_c = 1;
        }
    }

    if (argc < 5 || !has_g || !has_c) {
        fprintf(stderr, "Usage: %s -g [generator_command] -c [consumer_command]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Parse generator command
    for (i = 2; i < argc && strcmp(argv[i], "-c") != 0; i++) {
        generator_command[generator_argc++] = argv[i];
    }
    generator_command[generator_argc] = NULL;  // Null-terminate the command array

    // Parse consumer command
    for (i = i + 1; i < argc; i++) {
        consumer_command[consumer_argc++] = argv[i];
    }
    consumer_command[consumer_argc] = NULL;  // Null-terminate the command array

    // Create pipe
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork first child (generator)
    pid_t generator_pid = fork();
    if (generator_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (generator_pid == 0) { // Child process (generator)
        // Close unused read end
        close(pipefd[0]);

        // Redirect stdout to write end of pipe
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        // Execute generator command
        execvp(generator_command[0], generator_command);
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    // Fork second child (consumer)
    pid_t consumer_pid = fork();
    if (consumer_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (consumer_pid == 0) { // Child process (consumer)
        // Close unused write end
        close(pipefd[1]);

        // Redirect stdin to read end of pipe
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        // Execute consumer command
        execvp(consumer_command[0], consumer_command);
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    // Close pipe ends in parent
    close(pipefd[0]);
    close(pipefd[1]);

    // Wait for both child processes to finish
    waitpid(generator_pid, NULL, 0);
    waitpid(consumer_pid, NULL, 0);

    return 0;
}
