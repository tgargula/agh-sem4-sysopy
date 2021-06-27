#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define KILL 0
#define SIGQUEUE 1
#define SIGRT 2

static volatile int received = 0;
static int to_send = -1;
static sigset_t blocked;

void handler(int signo) {
    if (signo == SIGUSR1) {
        received = received + 1;
        printf("Otrzymałem sygnał SIGUSR1!\n");
        sigsuspend(&blocked);
    } else if (signo == SIGUSR2) {
        printf("Liczba otrzymanych sygnałów: %d\n", received);
        printf("Liczba procesów do wysłania: %d\n", to_send);
        exit(0);
    }
}

void rt_handler(int signo) {
    if (signo == SIGRTMAX) {
        received = received + 1;
        printf("I received SIGRTMAX signal!\n");
        sigsuspend(&blocked);
    } else if (signo == SIGRTMIN) {
        printf("I received SIGRTMIN signal!\n");
        printf("Liczba otrzymanych sygnałów: %d\n", received);
        printf("Liczba procesów do wysłania: %d\n", to_send);
        exit(0);
    }
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Incorrect number of arguments! Received %d; required %d\n", argc - 1, 3);
        return -1;
    }

    int mode = -1;

    if (!strcmp(argv[3], "KILL")) mode = KILL;
    if (!strcmp(argv[3], "SIGQUEUE")) mode = SIGQUEUE;
    if (!strcmp(argv[3], "SIGRT")) mode = SIGRT;

    if (mode == -1) {
        fprintf(stderr, "Incorrect mode!\n\tAvailable modes: KILL, SIGQUEUE, SIGRT\n");
        return -1;
    }

    sigfillset(&blocked);

    int catcher_pid = strtol(argv[1], &argv[1], 10);
    to_send = strtol(argv[2], &argv[2], 10);

    if (mode == SIGRT) {
        signal(SIGRTMAX, rt_handler);
        signal(SIGRTMIN, rt_handler);
        sigdelset(&blocked, SIGRTMAX);
        sigdelset(&blocked, SIGRTMIN);
    } else {
        signal(SIGUSR1, handler);
        signal(SIGUSR2, handler);
        sigdelset(&blocked, SIGUSR1);
        sigdelset(&blocked, SIGUSR2);
    }

    if (mode == KILL) {
        for (int i = 0; i < to_send; i++) {
            kill(catcher_pid, SIGUSR1);
            printf("Wysłałem sygnał SIGUSR1!\n");
        }
        kill(catcher_pid, SIGUSR2);
        printf("Wysłałem sygnał SIGUSR2!\n");
    }

    if (mode == SIGQUEUE) {
        union sigval value;
        value.sival_ptr = NULL;
        for (int i = 0; i < to_send; i++) {
            sigqueue(catcher_pid, SIGUSR1, value);
            printf("Wysłałem sygnał SIGUSR1!\n");
        }
        sigqueue(catcher_pid, SIGUSR2, value);
        printf("Wysłałem sygnał SIGUSR2!\n");
    }

    if (mode == SIGRT) {
        for (int i = 0; i < to_send; i++) {
            kill(catcher_pid, SIGRTMAX);
            printf("Wysłałem sygnał SIGRTMAX!\n");
        }
        kill(catcher_pid, SIGRTMIN);
        printf("Wysłałem sygnał SIGRTMIN!\n");
    }

    sigsuspend(&blocked);

    return 0;
}