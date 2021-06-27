#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define KILL 0
#define SIGQUEUE 1
#define SIGRT 2

static volatile int received = 0;
static sigset_t blocked;
static int mode = -1;

void sendback(pid_t sender_pid) {
    if (mode == SIGQUEUE) {
        union sigval value;
        value.sival_ptr = NULL;
        for (int i = 0; i < received; i++) {
            sigqueue(sender_pid, SIGUSR1, value);
        }
        sigqueue(sender_pid, SIGUSR2, value);
    } else {
        int signal1 = mode == SIGRT ? SIGRTMAX : SIGUSR1;
        int signal2 = mode == SIGRT ? SIGRTMIN : SIGUSR2;
        for (int i = 0; i < received; i++) {
            kill(sender_pid, signal1);
        }
        kill(sender_pid, signal2);
    }
    exit(0);
}

void receive(int signo, siginfo_t *info, void *context) {
    if (signo == SIGUSR1) {
        received++;
        printf("Otrzymałem sygnał SIGUSR1!\n");
        sigsuspend(&blocked);
    } else if (signo == SIGUSR2) {
        printf("Otrzymałem %d sygnał(y/ów). Odsyłam je z powrotem!\n", received);
        pid_t sender_pid = info->si_pid;
        sendback(sender_pid);
    }
}

void rt_receive(int signo, siginfo_t *info, void *context) {
    if (signo == SIGRTMAX) {
        received++;
        printf("Otrzymałem sygnał SIGRTMAX!\n");
        sigsuspend(&blocked);
    } else if (signo == SIGRTMIN) {
        printf("Otrzymałem %d sygnał(y/ów). Odsyłam je z powrotem!\n", received);
        pid_t sender_pid = info->si_pid;
        sendback(sender_pid);
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Incorrect number of arguments! Received %d; required %d!\n", argc - 1, 1);
    }

    if (!strcmp(argv[1], "KILL")) mode = KILL;
    if (!strcmp(argv[1], "SIGQUEUE")) mode = SIGQUEUE;
    if (!strcmp(argv[1], "SIGRT")) mode = SIGRT;

    if (mode == -1) {
        perror("Incorrect mode!\n\tAvailable modes: KILL, SIGQUEUE, SIGRT\n");
    }

    printf("Catcher with pid: %d has started!\n", getpid());

    struct sigaction act;
    sigfillset(&blocked);

    int signal1 = mode == SIGRT ? SIGRTMAX : SIGUSR1;
    int signal2 = mode == SIGRT ? SIGRTMIN : SIGUSR2;

    sigdelset(&blocked, signal1);
    sigdelset(&blocked, signal2);
    act.sa_sigaction = mode == SIGRT ? rt_receive : receive;
    act.sa_mask = blocked;
    act.sa_flags = SA_SIGINFO;
    sigaction(signal1, &act, NULL);
    sigaction(signal2, &act, NULL);

    sigsuspend(&blocked);

    return 0;
}