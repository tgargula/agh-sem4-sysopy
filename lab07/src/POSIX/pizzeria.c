#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "utils.h"

pid_t *suppliers;
pid_t *cooks;
sem_t *oven_place;
sem_t *oven_in;
sem_t *table_place;
sem_t *table_on;
sem_t *table_ready;
int fd;
int addr;

void rmsemshm();
void handle_stop(int signo);

int main(int argc, char **argv) {
    atexit(rmsemshm);
    signal(SIGINT, handle_stop);

    int seed = 42;
    srand(seed);

    if (argc != 3) {
        ERROR_MSG("Invalid number of arguments. Required: 2, provided: %d\n", argc - 1);
    }

    int N = strtol(argv[1], &argv[1], 10);
    int M = strtol(argv[2], &argv[2], 10);

    SEM_CREATE(oven_place, OVEN_PLACE, O_CREAT | O_EXCL, 0666, 1);
    SEM_CREATE(oven_in, OVEN_IN, O_CREAT | O_EXCL, 0666, OVEN_CAP);
    SEM_CREATE(table_place, TABLE_PLACE, O_CREAT | O_EXCL, 0666, 1);
    SEM_CREATE(table_on, TABLE_ON, O_CREAT | O_EXCL, 0666, TABLE_CAP);
    SEM_CREATE(table_ready, TABLE_READY, O_CREAT | O_EXCL, 0666, 0);

    if ((fd = shm_open(SHM_PATH, O_CREAT | O_EXCL | O_RDWR, 0666)) == -1) {
        ERROR_MSG("Cannot create shared memory!\n");
    }

    if ((addr = ftruncate(fd, sizeof(struct pizzeria *))) == -1) {
        ERROR_MSG("Cannot allocate shared memory!\n");
    }

    cooks = calloc(N + 1, sizeof(pid_t));
    for (int i = 0; i < N; i++) {
        pid_t child_pid = fork();
        if (child_pid == 0) {
            execlp("./target/POSIX/cook", "target/POSIX/cook", NULL);
        }
        cooks[i] = child_pid;
    }
    cooks[N] = 0;

    suppliers = calloc(M + 1, sizeof(pid_t));
    for (int i = 0; i < M; i++) {
        pid_t child_pid = fork();
        if (child_pid == 0) {
            execlp("./target/POSIX/supplier", "target/POSIX/supplier", NULL);
        }
        suppliers[i] = child_pid;
    }
    suppliers[M] = 0;

    for (int i = 0; i < N + M; i++) {
        wait(NULL);
    }

    return 0;
}

void rmsemshm() {
    SEM_CLOSE(oven_place);
    SEM_CLOSE(oven_in);
    SEM_CLOSE(table_place);
    SEM_CLOSE(table_on);
    SEM_CLOSE(table_ready);

    SEM_UNLINK(OVEN_PLACE);
    SEM_UNLINK(OVEN_IN);
    SEM_UNLINK(TABLE_PLACE);
    SEM_UNLINK(TABLE_ON);
    SEM_UNLINK(TABLE_READY);
    if (shm_unlink(SHM_PATH) == -1) {
        ERROR_MSG("Cannot unlink shared memory!\n");
    }

    free(cooks);
    free(suppliers);
}

void handle_stop(int signo) {
    for (int i = 0; cooks[i]; i++) {
        kill(cooks[i], SIGINT);
    }
    for (int i = 0; suppliers[i]; i++) {
        kill(suppliers[i], SIGINT);
    }
    exit(0);
}
