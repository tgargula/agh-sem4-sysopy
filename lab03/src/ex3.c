#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

bool containsPattern(FILE *file, char *pattern) {
    int length = strlen(pattern);
    char *buf = calloc(length + 1, sizeof(char));
    int read;

    while ((read = fread(buf, 1, length, file)) == length) {
        if (!strncmp(buf, pattern, length)) {
            free(buf);
            return true;
        }
        fseek(file, -length + 1, 1);
    }

    free(buf);

    return false;
}

void find(DIR *directory, char *path, char *pattern, long int depth, int currentDepth) {
    struct dirent *entry;

    while (currentDepth <= depth && (entry = readdir(directory)) != NULL) {
        char *newpath = calloc(strlen(path) + strlen(entry->d_name) + 2, sizeof(char));
        strcpy(newpath, path);
        strcat(newpath, "/");
        strcat(newpath, entry->d_name);
        if (entry->d_type == DT_REG) {
            char *extension = strrchr(entry->d_name, '.');
            if (!strcmp(extension, ".txt")) {
                // Search file and add to stack if contains pattern
                FILE *file;
                if ((file = fopen(newpath, "r")) == NULL) {
                    fprintf(stderr, "%s is not a file!\n", newpath);
                    fclose(file);
                    exit(-1);
                }
                if (containsPattern(file, pattern))
                    printf("Pid: %d\tPath: %s\n", getpid(), newpath);
                fclose(file);
            }
        }
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            // Recursively enter directory with other process
            pid_t child_pid = fork();
            if (!child_pid) {
                DIR *newdirectory;
                if ((newdirectory = opendir(newpath)) == NULL) {
                    fprintf(stderr, "%s is not a directory!", newpath);
                    closedir(newdirectory);
                    exit(-1);
                }
                find(newdirectory, newpath, pattern, depth, currentDepth + 1);
                closedir(newdirectory);
                free(newpath);
                exit(0);
            }
            wait(NULL);
        }
        free(newpath);
    }

    if (currentDepth <= depth)
        free(entry);
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Illegal number of arguments! There are required 3 arguments; %d provided\n", argc - 1);
        exit(-1);
    }

    DIR *directory;
    char *path = argv[1];
    char *pattern = argv[2];
    long int depth = strtol(argv[3], &argv[3], 10);

    if ((directory = opendir(path)) == NULL) {
        fprintf(stderr, "Unable to read directory: %s\n", argv[1]);
        exit(-1);
    }

    find(directory, path, pattern, depth, 0);

    closedir(directory);

    return 0;
}