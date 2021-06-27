#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "src/lib/include/lib_library.h"

char *fgetline(TYPE file) {
    char *line = calloc(MAX_LINE_LEN, sizeof(char));
    char buf = '\0';

    while(buf != '\n' && READ(file, &buf, 1) > 0)
        strcat(line, &buf);
    
    return line;
}

int numberSize(int number) {
    int i = 1;
    while (number / 10 > 0) {
        number /= 10;
        i++;
    }
    return i;
}

int stringSize(char *string) {
    int i = 0;
    while (string[i] && string[i++] != '\n');
    return i;
}

void merge(char *filename1, char *filename2) {
    TYPE f1 = OPEN(filename1, "r");
    TYPE f2 = OPEN(filename2, "r");
    char *line;
    bool eof1 = false;
    bool eof2 = false;

    while (!eof1 || !eof2) {
        if (!eof1 && (line = fgetline(f1))[0] == '\0')
            eof1 = true;
        else
            WRITE(STDOUT, line, MAX_LINE_LEN);
        if (!eof2 && (line = fgetline(f2))[0] == '\0')
            eof2 = true;
        else
            WRITE(STDOUT, line, MAX_LINE_LEN);
    }

    CLOSE(f1);
    CLOSE(f2);
    free(line);
}

void filterWithSign(char *filename, char sign) {
    TYPE file = OPEN(filename, "r");
    char *line;

    while ((line = fgetline(file))[0]) {
        if (strchr(line, sign))
            WRITE(STDOUT, line, MAX_LINE_LEN);
        free(line);
    }

    CLOSE(file);
    free(line);
}

void filterNumbers(char *inFilename, char *a, char *b, char *c) {
    TYPE file = OPEN(inFilename, "r");
    TYPE outB = OPEN(b, "w");
    TYPE outC = OPEN(c, "w");
    char *buf;

    int evenCounter = 0;

    while ((buf = fgetline(file))[0]) {
        int number = atoi(buf);
        if (!(number % 2))
            evenCounter++;
        if (number > 10 && (number / 10 % 10 == 0 || number / 10 % 10 == 7))
            WRITE(outB, buf, numberSize(number) + 1);
        if ((int) round(sqrt((double) number)) * (int) round(sqrt((double) number)) == number)
            WRITE(outC, buf, numberSize(number) + 1);
        free(buf);
    }

    CLOSE(file);
    CLOSE(outB);
    CLOSE(outC);
    free(buf);

    char *line = calloc(40, sizeof(char));
    strcpy(line, "Liczb parzystych jest ");
    char parsed[12];
    sprintf(parsed, "%d", evenCounter);
    strcat(line, parsed);
    strcat(line, "\n");

    TYPE outA = OPEN(a, "w");
    WRITE(outA, line, 23 + numberSize(evenCounter));
    CLOSE(outA);
    free(line);

    chmod(a, 0664);
    chmod(b, 0664);
    chmod(c, 0664);
}

void replace(char *inFilename, char *outFilename, char *n1, char *n2) {
    TYPE in = OPEN(inFilename, "r");
    TYPE out = OPEN(outFilename, "w");

    int patternSize = stringSize(n1);
    int replacementSize = stringSize(n2);
    int diff = replacementSize - patternSize;
    char *line;

    while((line = fgetline(in))[0]) {
        int ctr = 0;
        int lineSize = stringSize(line);
        for (int i = 0; i < lineSize - patternSize; i++)
            if (!strncmp(&line[i], n1, patternSize)) ctr++;

        int bufSize = lineSize + diff * ctr;
        char *buf = calloc(bufSize, sizeof(char));
        for (int i = 0; i < lineSize - 1; i++) {
            if (!strncmp(&line[i], n1, patternSize)) {
                strncat(buf, n2, replacementSize);
                i += patternSize - 1;
            } else {
                strncat(buf, &line[i], 1);
            }
        }
        buf[bufSize - 1] = '\n';
        WRITE(out, buf, bufSize);
        free(buf);
        free(line);
    }

    free(line);
    CLOSE(in);
    CLOSE(out);
}

void breakLines(char *inFilename, char *outFilename, int breakpoint) {
    TYPE in = OPEN(inFilename, "r");
    TYPE out = OPEN(outFilename, "w");

    char *line = calloc(MAX_LINE_LEN, sizeof(char));

    while ((line = fgetline(in))[0]) {
        int size = stringSize(line);
        int breaks_num = size / breakpoint;
        if (breaks_num) {
            char *buf = calloc(size + breaks_num + 2, sizeof(char));
            for (int i = 0; i <= breaks_num; i++) {
                char *brokenLine = calloc(52, sizeof(char));
                strncpy(brokenLine, &line[50 * i], 50);
                strcat(brokenLine, "\n");
                strcat(buf, brokenLine);
                free(brokenLine);
            }
            WRITE(out, buf, size + breaks_num);
            free(buf);
        } else
            WRITE(out, line, size);
        free(line);
    }

    free(line);
    CLOSE(in);
    CLOSE(out);
    chmod(outFilename, 0664);
}

// int main(void) {

//     // merge("test.txt", "test.txt");
//     // filterWithSign("test.txt", 'e');
//     // filterNumbers("dane.txt", "a.txt", "b.txt", "c.txt");
//     // breakLines("lamane.txt", "out.txt", 50);
//     replace("test.txt", "replace.txt", "e", "test");

//     return 0;

// }