#include <sys/times.h>

#ifndef _BENCHMARK_H
#define _BENCHMARK_H

struct btimes {
    struct tms tms;
    clock_t real;
};

struct btimes measureTime();

void printTime(char *filename, struct btimes startTimes);

#endif