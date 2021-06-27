#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>
#include "src/lib/include/lib_benchmark.h"

struct btimes measureTime() {
    struct btimes measurement;
    measurement.real = times(&measurement.tms);
    return measurement;
}

void printTime(char *filename, struct btimes startTimes) {
    struct btimes endTimes = measureTime();
    FILE *file = fopen(filename, "a");
    fprintf(file, "real %f\n", (double)(endTimes.real - startTimes.real) / sysconf(_SC_CLK_TCK));
    fprintf(file, "user %f\n", (double)(endTimes.tms.tms_utime - startTimes.tms.tms_utime) / sysconf(_SC_CLK_TCK));
    fprintf(file, "sys  %f\n", (double)(endTimes.tms.tms_stime - startTimes.tms.tms_stime) / sysconf(_SC_CLK_TCK));
    fclose(file);
}