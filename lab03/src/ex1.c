#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int n = strtol(argv[1], &argv[1], 10);
    pid_t pid = getpid();
    printf("PID glownego programu: %d\n", pid);

    for (int i = 0; i < n; i++) {
        pid_t child_pid = fork();
        if (!child_pid) {
            printf("Proces o pid: %d ma rodzica o pid: %d\n", (int)getpid(), (int)getppid());
            exit(0);
        }
        wait(NULL);
    }

    return 0;
}