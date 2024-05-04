#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>

#define E_OK 0
#define E_INCORRECT_ARGS -1
#define MAX_READ_WRITE_BYTES 50
#define DEFAULT_LOG_FILE "my_bfm_log.txt"

void helper() {
    printf("Usage: ./my_bfm -<flag> <path> [data]\n");
    printf("Flags:\n");
    printf("-c: Create file or directory\n");
    printf("-s: Add text to the end of a text file (provide text within quotes)\n");
    printf("-e: Add even numbers to the end of a binary file (provide starting even number)\n");
    printf("-d: Delete file or directory\n");
    printf("-r: Rename file or directory\n");
    printf("-l: Log file (provide path)\n");
}

void log_action(const char *log_file, const char *action, const char *path) {
    time_t current_time;
    struct tm *local_time;
    char time_string[50];

    // Get current time
    current_time = time(NULL);
    local_time = localtime(&current_time);

    // Format the time string
    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", local_time);

    // Open log file in append mode
    int log_fd = open(log_file, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (log_fd == -1) {
        return;
    }

    // Log the action with timestamp and path
    dprintf(log_fd, "[%s] %s: %s\n", time_string, action, path);

    close(log_fd);
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
    if (start_num >= 200) {
        return E_INCORRECT_ARGS;
    }

    // Open the file in write mode, create if not exists, append mode
    int fd = open(path, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        return errno;
    }
    
    // Write binary data to the file, limited to maximum 50 bytes
    unsigned char sequence[MAX_READ_WRITE_BYTES];
    int num;
    if (start_num <52){
        num = 52;
    } 
    else if (start_num%2 !=0) {
        num = start_num + 1;}
        else {
        num = start_num;}
    for (int i = 0; i < MAX_READ_WRITE_BYTES; i++) {
        sequence[i] = (unsigned char)(num);
        num += 2; // Increment by 2 to get the next even number
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

int delete_file_or_directory(const char *path, const char *log_file) {
    struct stat sb;

    // Check if the path is a directory
    if (stat(path, &sb) == 0) {
        if (S_ISDIR(sb.st_mode)) {
            // Attempt to remove the directory
            DIR *dir = opendir(path);
            if (dir == NULL) {
                return errno;
            }

            struct dirent *entry;
            while ((entry = readdir(dir)) != NULL) {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                    continue;
                }

                char entry_path[PATH_MAX];
                snprintf(entry_path, PATH_MAX, "%s/%s", path, entry->d_name);
                int ret = delete_file_or_directory(entry_path, log_file);
                if (ret != E_OK) {
                    closedir(dir);
                    return ret;
                }
            }

            closedir(dir);

            if (rmdir(path) == -1) {
                return errno;
            }
            // Log directory deletion
            log_action(log_file, "Deleted directory", path);
            return E_OK;
        }
    }

    // Delete the file
    if (unlink(path) == -1) {
        return errno;
    }
    // Log file deletion
    log_action(log_file, "Deleted file", path);
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
    if (argc < 2) {
        helper();
        return E_OK;
    }

    const char *log_file = DEFAULT_LOG_FILE;

    // Iterate over command-line arguments starting from the second argument (after the program name)
    for (int i = 1; i < argc; i++) {
        const char flag = argv[i][1];
        const char *path_name = argv[i + 1];

        // If the current flag is for the log file, store the log file path
        if (strcmp(argv[i], "-l") == 0) {
            log_file = path_name;
            i++; // Increment i to skip the next argument (the log file path)
            continue; // Move to the next argument
        }

        // Handle each flag individually
        switch (flag) {
            case 'c':
                if (create_file_or_directory(path_name) != E_OK) {
                    return errno;
                }
                log_action(log_file, "Created", path_name);
                break;
            case 's':
                if (argc < i + 3) {
                    return E_INCORRECT_ARGS;
                }
                if (write_to_file(path_name, argv[i + 2]) != E_OK) {
                    return errno;
                }
                log_action(log_file, "Text Added", path_name);
                break;
            case 'e':
                if (argc < i + 3) {
                    return E_INCORRECT_ARGS;
                }
                if (write_binary_to_file(path_name, atoi(argv[i + 2])) != E_OK) {
                    return errno;
                }
                log_action(log_file, "Binary Added", path_name);
                break;
            case 'd':
                if (delete_file_or_directory(path_name, log_file) != E_OK) {
                    return errno;
                }
                log_action(log_file, "Deleted", path_name);
                break;
            case 'r':
                if (argc < i + 3) {
                    return E_INCORRECT_ARGS;
                }
                if (rename_file(path_name, argv[i + 2]) != E_OK) {
                    return errno;
                }
                log_action(log_file, "Renamed", path_name);
                break;
            default:
                return E_INCORRECT_ARGS;
        }

        // Increment i to skip the next argument (the path), as it has already been processed
        i++;
    }

    return E_OK;
}
