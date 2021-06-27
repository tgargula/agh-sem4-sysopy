#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NONE -1
#define FORK 0
#define EXEC 1

void info_handler(int signum) {
    printf("I received a signal %d!\n", signum);
}

int main(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        printf("%s ", argv[i]);
    }

    printf("\n");

    sigset_t signals;
    sigset_t pending;
    sigemptyset(&signals);
    sigaddset(&signals, SIGUSR1);
    bool print_pending = false;

    int mode = NONE;

    if (argc > 1 && !strcmp(argv[1], "FORK"))
        mode = FORK;
    if (argc > 1 && !strcmp(argv[1], "EXEC"))
        mode = EXEC;

    if (mode == NONE) {
        for (int i = 1; i < argc; i++) {
            if (!strcmp(argv[i], "pending"))
                print_pending = true;
        }
    } else {
        for (int i = 2; i < argc; i++) {
            if (!strcmp(argv[i], "ignore")) {
                signal(SIGUSR1, SIG_IGN);
            } else if (!strcmp(argv[i], "handler")) {
                signal(SIGUSR1, info_handler);
            } else if (!strcmp(argv[i], "mask")) {
                sigset_t blocked;
                sigemptyset(&blocked);
                sigaddset(&blocked, SIGUSR1);
                if (sigprocmask(SIG_BLOCK, &blocked, &signals) < 0)
                    perror("Signal could not be blocked!\n");
            } else if (!strcmp(argv[i], "pending")) {
                print_pending = true;
            } else {
                fprintf(stderr, "There is no command: %s\n", argv[i]);
            }
        }

        // PARENT
        printf("Parent:\n");

        raise(SIGUSR1);
    }

    if (print_pending) {
        sigpending(&pending);
        printf("Is SIGUSR1 in pending signals? %d\n", sigismember(&pending, SIGUSR1));
    }

    if (mode == FORK) {
        // Create child
        pid_t child_pid = fork();
        if (!child_pid) {
            // CHILD
            printf("CHILD:\n");

            if (print_pending) {
                sigpending(&pending);
                printf("Is SIGUSR1 in pending signals? %d\n", sigismember(&pending, SIGUSR1));
            }

            exit(0);
        }

        wait(NULL);
    }

    if (mode == EXEC) {
        char **args = calloc(argc, sizeof(char *));
        args[0] = calloc(strlen(argv[0]) + 1, sizeof(char));
        strcpy(args[0], argv[0]);
        for (int i = 2; i < argc; i++) {
            args[i - 1] = calloc(strlen(argv[i]) + 1, sizeof(char));
            strcpy(args[i - 1], argv[i]);
        }
        args[argc] = NULL;
        execv(args[0], args);
    }

    return 0;
}