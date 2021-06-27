#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

static int signals = 0;


void handler(int sig_no, siginfo_t *info, void *context) {
    printf("I received a signal %d from a process with pid: %d ", info->si_signo, info->si_pid);
    if (sig_no == SIGUSR2)
        printf("(SIGUSR2)!\n");
    if (sig_no == SIGUSR1)
        printf("(SIGUSR1)!\n");
    if (sig_no == SIGRTMAX)
        printf("(SIGRTMAX)!\n");
    
    printf("Time:\n");
    printf("user %lf\n", (double) info->si_utime / sysconf(_SC_CLK_TCK));
    printf("sys  %lf\n", (double) info->si_stime / sysconf(_SC_CLK_TCK));
    printf("Error number: %d\n", info->si_errno);
}

void ez_handler(int sig_no, siginfo_t *info, void *context) {
    printf("Received signal %d\n", info->si_signo);
    int value = signals;
    printf("Sum before: %d ", value);
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 50;
    nanosleep(&ts, &ts);
    signals++;
    printf("After: %d\n", signals);
}

int main(void) {

    struct sigaction siginfoact1;
    siginfoact1.sa_flags = SA_SIGINFO;
    siginfoact1.sa_sigaction = ez_handler;
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    siginfoact1.sa_mask = sigset;

    sigaction(SIGUSR2, &siginfoact1, NULL);
    sigaction(SIGUSR1, &siginfoact1, NULL);
    sigaction(SIGRTMAX, &siginfoact1, NULL);

    for (int i = 0; i < 10000; i++) {
        kill(getpid(), SIGUSR1);
    }
    raise(SIGUSR2);

    struct sigaction siginfoact2;
    siginfoact2.sa_flags = SA_SIGINFO | SA_RESETHAND;
    siginfoact2.sa_sigaction = handler;

    sigaction(SIGUSR2, &siginfoact2, NULL);
    sigaction(SIGUSR1, &siginfoact2, NULL);
    sigaction(SIGRTMAX, &siginfoact2, NULL);

    raise(SIGUSR1);
    raise(SIGRTMAX);
    raise(SIGUSR2);
    raise(SIGRTMAX);
    

    return 0;
}