#ifndef __UTILS_H__
#define __UTILS_H__

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_PIXELS 8294400 // 4K resolution
#define LINE_LEN 70
#define MAX_LINE_LEN 3840 * 4
#define BILLION 1000000000L
#define REPORT_FILE "Times.txt"

#define ERROR_MSG(format, ...)              \
    fprintf(stderr, format, ##__VA_ARGS__); \
    perror("Reason:");                      \
    exit(EXIT_FAILURE);

struct timespec start;
struct timespec stop;

enum clock_op {
    START,
    GET,
    STOP
};

struct mtime_t {
    struct timespec start;
    struct timespec stop;
};

void mtime(struct mtime_t *times, enum clock_op op, double *elapsed) {
    switch (op) {
        case START:
            clock_gettime(CLOCK_REALTIME, &times->start);
            break;
        case STOP:
            clock_gettime(CLOCK_REALTIME, &times->stop);
            break;
        case GET:
            *elapsed = (times->stop.tv_sec - times->start.tv_sec) + (double)(times->stop.tv_nsec - times->start.tv_nsec) / BILLION;
            break;
        default:
            errno = EINVAL;
            ERROR_MSG("Unknown operation!\n");
    }
}

struct image {
    char type[70];
    unsigned char cells[MAX_PIXELS];
    int height;
    int width;
    int pixel_max;
};

struct context {
    int threads_no;
    int index;
};

enum options {
    NUMBERS,
    BLOCK
};

#endif