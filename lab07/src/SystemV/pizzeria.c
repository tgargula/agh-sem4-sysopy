#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "utils.h"

int semid;
int shmid;

pid_t *suppliers;
pid_t *cooks;

void handle_stop(int signo);
void rmsemshm();
void make_pizza();

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

    key_t key;
    if ((key = ftok(PROJECT_PATH, PROJECT_ID)) == -1) {
        ERROR_MSG("Pizzeria: Could not create key!\n");
    }

    if ((semid = semget(key, SEMSZ, IPC_CREAT | IPC_EXCL | 0666)) == -1) {
        ERROR_MSG("Pizzeria: Could not create semaphore!\n");
    }

    if ((shmid = shmget(key, sizeof(struct pizzeria *), IPC_CREAT | IPC_EXCL | 0666)) == -1) {
        ERROR_MSG("Pizzeria: Could not create shared memory!\n");
    }

    if (semctl(semid, OVEN_PLACE, SETVAL, 1) == -1) {
        ERROR_MSG("Pizzeria: Could not set OVEN_PLACE semaphore!\n");
    }

    if (semctl(semid, OVEN_IN, SETVAL, OVEN_CAP) == -1) {
        ERROR_MSG("Pizzeria: Could not set OVEN_IN semaphore!\n");
    }

    if (semctl(semid, TABLE_PLACE, SETVAL, 1) == -1) {
        ERROR_MSG("Pizzeria: Could not set TABLE_PLACE semaphore!\n");
    }

    if (semctl(semid, TABLE_ON, SETVAL, TABLE_CAP) == -1) {
        ERROR_MSG("Pizzeria: Could not set TABLE_ON semaphore!\n");
    }

    if (semctl(semid, TABLE_READY, SETVAL, 0) == -1) {
        ERROR_MSG("Pizzeria: Could not set TABLE_READY semaphore!\n");
    }

    cooks = calloc(N+1, sizeof(pid_t));
    for (int i = 0; i < N; i++) {
        pid_t child_pid = fork();
        if (child_pid == 0) {
            execlp("./target/SystemV/cook", "target/SystemV/cook", NULL);
        }
        cooks[i] = child_pid;
    }
    cooks[N] = 0;

    suppliers = calloc(M+1, sizeof(pid_t));
    for (int i = 0; i < M; i++) {
        pid_t child_pid = fork();
        if (child_pid == 0) {
            execlp("./target/SystemV/supplier", "target/SystemV/supplier", NULL);
        }
        suppliers[i] = child_pid;
    }
    suppliers[M] = 0;

    for (int i = 0; i < N + M; i++) {
        wait(NULL);
    }

    return 0;
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

void rmsemshm() {
    if (semctl(semid, 0, IPC_RMID) == -1) {
        ERROR_MSG("Could not remove semaphore!\n");
    }
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        ERROR_MSG("Could not remove shared memory!\n");
    }
    free(cooks);
    free(suppliers);
}