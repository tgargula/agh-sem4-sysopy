#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <unistd.h>

#include "lab1/src/lib/include/lib_merge.h"

void print_time(clock_t start_t, clock_t end_t, long utime, long stime) {
    printf("real %f\n", (double)(end_t - start_t) / sysconf(_SC_CLK_TCK));
    printf("user %f\n", (double)(utime) / sysconf(_SC_CLK_TCK));
    printf("sys  %f\n", (double)(stime) / sysconf(_SC_CLK_TCK));
}

int main(int argc, char **argv) {
    if (system("if [ ! -d tmp ]; then mkdir tmp; fi"));

    struct tms start_t;
    struct tms end_t;

    clock_t start = times(&start_t);
    long utime = 0;
    long stime = 0;

    for (int i = 1; i < argc; i++) {
        pid_t child_pid = fork();
        if (!child_pid) {
            char *seq = argv[i];
            merge(seq, 0);
            times(&end_t);
            utime += end_t.tms_utime - start_t.tms_utime;
            stime += end_t.tms_stime - start_t.tms_stime;
            exit(0);
        }
    }
    wait(NULL);

    clock_t end = times(&end_t);
    print_time(start, end, utime, stime);

    if (system("rm -rf tmp"));

    return 0;
}