#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct fileinfo {
    int *len;
    int lines;
};

int readtosep(int fd, char *data) {
    int len = 0;
    char *buf = malloc(sizeof(char));
    while (read(fd, buf, sizeof(char)) > 0 && *buf != '\\') {
        data = (char *)realloc(data, ++len);
        data[len - 1] = *buf;
    }
    data = (char *)realloc(data, ++len);
    data[len - 1] = '\0';
    return len - 1;
}

void fcpn(FILE *dest, FILE *src, int from, int to) {
    fseek(src, from, SEEK_SET);
    char *buf = malloc(sizeof(char));
    for (int i = 0; !feof(src) && i < to - from; i++) {
        fread(buf, sizeof(char), 1, src);
        fwrite(buf, sizeof(char), 1, dest);
    }
}

long fsize(FILE *file) {
    long pos = ftell(file);
    fseek(file, 0, SEEK_END);
    long result = ftell(file);
    fseek(file, pos, SEEK_SET);
    return result;
}

void writeaddline(FILE *original, char *tmpfilename, struct fileinfo *fileinfo, char *text, int rowno) {
    // add necessary lines
    FILE *copy = fopen(tmpfilename, "r+");
    setvbuf(copy, NULL, _IONBF, 0);
    fseek(original, 0, SEEK_END);
    for (int i = fileinfo->lines; i <= rowno; i++) {
        fwrite("\n", sizeof(char), 1, original);
        fileinfo->len = realloc(fileinfo->len, ++fileinfo->lines * sizeof(int));
        fileinfo->len[i] = 0;
    }
    rewind(original);

    // calculate position
    int pos = 0;
    for (int i = 0; i < rowno; i++) {
        pos += fileinfo->len[i];
    }
    pos += rowno;

    // insert text using tmp file
    fcpn(copy, original, 0, pos);
    fwrite(text, sizeof(char), strlen(text), copy);
    fcpn(copy, original, pos, fsize(original));

    rewind(original);
    fcpn(original, copy, 0, fsize(copy));
    fileinfo->len[rowno - 1] += strlen(text);

    fclose(copy);
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Illegal number of arguments. 3 reguired; %d provided!\n", argc - 1);
    }

    char *pipepath = argv[1];
    char *filename = argv[2];
    int n = strtol(argv[3], &argv[3], 10);

    int fd = open(pipepath, O_RDONLY);
    FILE *file = fopen(filename, "w+");
    static struct fileinfo fileinfo;
    fileinfo.lines = 1;
    fileinfo.len = malloc(sizeof(int));
    fileinfo.len[0] = 0;
    setvbuf(file, NULL, _IONBF, 0);

    char *tmpfilename = calloc(strlen(filename) + 5, sizeof(char));
    strcpy(tmpfilename, filename);
    strcat(tmpfilename, ".tmp");

    char *buf = calloc(2, sizeof(char));
    int len;
    do {
        // Read line number
        char *data = malloc(sizeof(char));
        len = readtosep(fd, data);
        int rowno = strtol(data, &data, 10);

        // Read text
        char *text = malloc(sizeof(char));
        len = readtosep(fd, text);

        // Add text to file
        writeaddline(file, tmpfilename, &fileinfo, text, rowno);

    } while (len > 0);

    close(fd);
    fclose(file);

    return 0;
}