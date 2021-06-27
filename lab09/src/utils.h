#ifndef __UTILS_H__
#define __UTILS_H__

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ELVES_NO 10
#define REINDEERS_NO 9
#define ELVES_REQ 3
#define REINDEERS_REQ 9
#define MAX_JOBS 3

#define ERROR_MSG(format, ...)              \
    fprintf(stderr, format, ##__VA_ARGS__); \
    perror("Reason:");                      \
    exit(EXIT_FAILURE);

#define synchronized(mutex, codeblock) \
    pthread_mutex_lock(mutex);         \
    codeblock                          \
    pthread_mutex_unlock(mutex);

#define asynchronized(mutex, codeblock) \
    pthread_mutex_unlock(mutex);        \
    codeblock                           \
    pthread_mutex_lock(mutex);

int randint(int from, int to) {
    return rand() % (to - from) + from;
}

void randsleep(int from, int to) {
    usleep(randint(from * 1000000, to * 1000000));
}

#endif
