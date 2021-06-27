#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sighandler(int signo, siginfo_t *info, void *context) {
    if (signo == SIGUSR1) {
        printf("Otrzymałem sygnał SIGUSR1. Otrzymana wartość: %s\n", (char *)info->si_value.sival_ptr);
    } 
    if (signo == SIGUSR2) {
        printf("Otrzymałem sygnał SIGUSR2. Otrzymana wartość: %s\n", (char *)info->si_value.sival_ptr);
    }
}


int main(int argc, char* argv[]) {

    if(argc != 3){
        printf("Not a suitable number of program parameters\n");
        return 1;
    }

    struct sigaction action;
    action.sa_sigaction = &sighandler;
    action.sa_flags = SA_SIGINFO;

    //zablokuj wszystkie sygnaly za wyjatkiem SIGUSR1 i SIGUSR2
    //zdefiniuj obsluge SIGUSR1 i SIGUSR2 w taki sposob zeby proces potomny wydrukowal
    //na konsole przekazana przez rodzica wraz z sygnalami SIGUSR1 i SIGUSR2 wartosci

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    sigaction(SIGUSR1, &action, NULL);
    sigaction(SIGUSR2, &action, NULL);


    int child = fork();
    if (child == 0) {
        sleep(1);
    }
    else {
        //wyslij do procesu potomnego sygnal przekazany jako argv[2]
        //wraz z wartoscia przekazana jako argv[1]

        int signal_val = strtol(argv[2], &argv[2], 10);
        
        union sigval sval;
        sval.sival_ptr = argv[1];

        // Zamiast sval.sival_ptr można użyć również sival_int, jednak zdecydowałem się
        // zaimplementować bardziej ogólną wersję.
        
        sigqueue(child, signal_val, sval);
    }

    return 0;
}
