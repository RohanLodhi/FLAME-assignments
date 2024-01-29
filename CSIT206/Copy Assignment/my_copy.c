// The error codes were obtained from the following website: https://mariadb.com/kb/en/operating-system-error-codes/
// These can be verified through running errno on the CLI

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define BUF_SIZE 4096
#define E_OK 0
#define E_INCORRECT_ARGS 22 // EINVAL 22 Invalid argument
#define E_OPEN_SOURCE_FILE 2 // ENOENT 2 No such file or directory
#define E_OPEN_DEST_FILE 13 // EACCES 13 Permission denied
#define E_READ_SOURCE_FILE 5 // EIO 5 Input/output error
#define E_WRITE_DEST_FILE 5 // EIO 5 Input/output error

int main (int argc, char* argv[]) {
    bool STD_IN = false, STD_OUT = false;
    int source_fd, dest_fd;
    ssize_t bytes_read, bytes_written;
    char buffer[BUF_SIZE];
    
    // Validate the input
    if(argc != 5 || (strcmp(argv[1], "-i") != 0) || (strcmp(argv[3], "-o") != 0)){
        printf("Usage: ./flame_cp -i <source file pathname> -o <destination file pathname>\n");
        return E_INCORRECT_ARGS;
    }

    // Open the source file
    if (strcmp(argv[2], "-") == 0) {
        source_fd = STDIN_FILENO;
        STD_IN = true;
    } else {
        source_fd = open(argv[2], O_RDONLY);
        if (source_fd == -1) {
            return E_OPEN_SOURCE_FILE;
        }
    }

    // Open the destination file
    if (strcmp(argv[4], "-") == 0) {
        dest_fd = STDOUT_FILENO; 
        STD_OUT = true;
    } else {
        dest_fd = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
        if (dest_fd == -1) {
            if (!STD_IN)
                close(source_fd);
            return E_OPEN_DEST_FILE;
        }
    }

    // Copy file contents
    while ((bytes_read = read(source_fd, buffer, BUF_SIZE)) > 0) {
        bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            if (!STD_IN)
                close(source_fd);
            if (!STD_OUT)
                close(dest_fd);
            return E_WRITE_DEST_FILE;
        }
    }

    // Error handling for reading
    if (bytes_read == -1) {
        if (!STD_IN)
            close(source_fd);
        if (!STD_OUT)
            close(dest_fd);
        return E_READ_SOURCE_FILE;
    }

    // Close files
    if (!STD_IN)
        close(source_fd);
    if (!STD_OUT)
        close(dest_fd);

    return E_OK;
}