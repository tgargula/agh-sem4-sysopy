#ifndef _LIB_2_H
#define _LIB_2_H

#define MAX_LINE_LEN 255

#ifdef sys
    #define TYPE int
    #define STDOUT 1
    #define STDIN 0
    #define OPEN(filename, mode) open(filename, !strcmp(mode, "w") ? O_WRONLY | O_CREAT : !strcmp(mode, "r") ? O_RDONLY : O_RDWR | O_CREAT, 00644)
    #define READ(in, buf, size) read(in, buf, size)
    #define WRITE(in, buf, size) if(write(in, buf, size))
    #define CLOSE(file) close(file)
#elif lib
    #define TYPE FILE *
    #define STDOUT stdout
    #define STDIN stdin 
    #define OPEN(filename, mode) fopen(filename, mode)
    #define READ(in, buf, size) fread(buf, size, 1, in)
    #define WRITE(in, buf, size) fwrite(buf, size, 1, in)
    #define CLOSE(file) fclose(file); 
#else // same as lib
    #define TYPE FILE *
    #define STDOUT stdout
    #define STDIN stdin
    #define OPEN(filename, mode) fopen(filename, mode)
    #define READ(in, buf, size) fread(buf, size, __LONG_LONG_MAX__, in)
    #define WRITE(in, buf, size) fwrite(buf, size, __LONG_LONG_MAX__, in)
    #define CLOSE(file) fclose(file);
#endif

void merge(char *filename1, char *filename2);

void filterWithSign(char *filename, char sign_included);

void filterNumbers(char *inFilename, char *a, char *b, char *c);

void replace(char *inFilename, char *outFilename, char *n1, char *n2);

void breakLines(char *inFilename, char *outFilename, int breakpoint);

#endif