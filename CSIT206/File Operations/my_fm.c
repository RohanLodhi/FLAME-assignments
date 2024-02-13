#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define E_OK 0
#define E_INCORRECT_ARGS -1
#define MAX_READ_WRITE_BYTES 50

void helper() {
    printf("Usage: ./my_fm -<flag> <path> [data]\n");
    printf("Flags:\n");
    printf("-c: Create file or directory\n");
    printf("-w: Write to file (provide data)\n");
    printf("-r: Read file\n");
    printf("-d: Delete file\n");
    printf("-R: Rename file (provide new path)\n");
    printf("-b: Write to binary (provide data)\n");
}

int create_file_or_directory(const char *path) {
    struct stat sb;

    // Check if the path already exists
    if (stat(path, &sb) == 0) {
        if (S_ISDIR(sb.st_mode)) {
            return E_OK; // Do nothing if path is a directory
        }
    }

    // Find the last '/' character in the path
    const char *last_slash = strrchr(path, '/');
    if (last_slash == NULL) {
        return errno;
    }

    // Create directory or file based on the '.' after the last '/'
    if (strstr(last_slash, ".") != NULL) {
        // Create file
        int fd = creat(path, S_IRUSR | S_IWUSR);
        if (fd == -1) {
            return errno;
        }
        close(fd);
        return E_OK;
    } else {
        // Create directory
        if (mkdir(path, 0777) == -1) {
            return errno;
        }
        return E_OK;
    }
}

int write_to_file(const char *path, const char *data) {
    // Open the file in write mode, create if not exists, append mode
    int fd = open(path, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        return errno;
    }
    
    // Write data to the file, limited to maximum 50 bytes
    ssize_t bytes_to_write = strlen(data) > MAX_READ_WRITE_BYTES ? MAX_READ_WRITE_BYTES : strlen(data);
    ssize_t bytes_written = write(fd, data, bytes_to_write);
    if (bytes_written == -1) {
        close(fd);
        return errno;
    }
    
    close(fd);
    return E_OK;
}

int write_binary_to_file(const char *path, int start_num) {
    // Validate the user-specified number
    if (start_num < 50 || start_num >= 200) {
        return E_INCORRECT_ARGS;
    }

    // Open the file in write mode, create if not exists, append mode
    int fd = open(path, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        return errno;
    }
    
    // Write binary data to the file, limited to maximum 50 bytes
    unsigned char sequence[MAX_READ_WRITE_BYTES];
    int num = start_num;
    for (int i = 0; i < MAX_READ_WRITE_BYTES; i++) {
        sequence[i] = (unsigned char)(num);
        num += 2; // Increment by 2 to get the next odd number
        if (num >= 200) break;
    }

    ssize_t bytes_written = write(fd, sequence, sizeof(sequence));
    if (bytes_written == -1) {
        close(fd);
        return errno;
    }
    
    close(fd);
    return E_OK;
}

int read_file(const char *path) {
    // Open the file in read mode
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        return errno;
    }

    // Read and print the content of the file, limited to maximum 50 bytes
    char buffer[MAX_READ_WRITE_BYTES];
    ssize_t bytes_read = read(fd, buffer, MAX_READ_WRITE_BYTES);
    if (bytes_read == -1) {
        close(fd);
        return errno;
    }

    buffer[bytes_read] = '\0';
    close(fd);
    printf("Content of file '%s': %s\n", path, buffer);
    return E_OK;
}

int delete_file(const char *path) {
    struct stat sb;

    // Check if the path is a directory
    if (stat(path, &sb) == 0) {
        if (S_ISDIR(sb.st_mode)) {
            // Attempt to remove the directory
            if (rmdir(path) == -1) {
                return errno;
            }
            return E_OK;
        }
    }

    // Delete the file
    if (unlink(path) == -1) {
        return errno;
    }
    return E_OK;
}

int rename_file(const char *oldpath, const char *newpath) {
    // Rename the file
    if (rename(oldpath, newpath) == -1) {
        return errno;
    }
    return E_OK;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        helper();
        return E_OK;
    }

    const char flag = argv[1][1];
    const char *path_name = argv[2];

    switch (flag) {
    case 'c':
        return create_file_or_directory(path_name);
    case 'w':
        if (argc < 4) {
            return E_INCORRECT_ARGS;
        }
        return write_to_file(path_name, argv[3]);
    case 'r':
        return read_file(path_name);
    case 'd':
        return delete_file(path_name);
    case 'R':
        if (argc < 4) {
            return E_INCORRECT_ARGS;
        }
        return rename_file(path_name, argv[3]);
    case 'b':
        if (argc < 4) {
            return E_INCORRECT_ARGS;
        }
        return write_binary_to_file(path_name, atoi(argv[3]));
    default:
        return E_INCORRECT_ARGS;
    }
}