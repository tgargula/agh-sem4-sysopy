#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 5) {
        fprintf(stderr, "Illegal number of arguments! 4 required; %d provided!\n", argc - 1);
        exit(-1);
    }

    char *pipepath = argv[1];
    char *rowno = argv[2];
    char *filename = argv[3];
    int n = strtol(argv[4], &argv[4], 10);

    FILE *file = fopen(filename, "r");
    int fd = open(pipepath, O_WRONLY);

    char *buf = calloc(n + 1, sizeof(char));
    int reader;

    while ((reader = fread(buf, sizeof(char), n, file)) > 0) {
        sleep(2);
        if (reader < n)
            buf[reader] = '\0';
        flock(fd, LOCK_EX);
        write(fd, rowno, strlen(rowno) * sizeof(char));
        write(fd, "\\", sizeof(char));
        write(fd, buf, reader * sizeof(char));
        write(fd, "\\", sizeof(char));
        flock(fd, LOCK_UN);
        fflush(stdout);
    }

    close(fd);
    fclose(file);

    return 0;
}