#include <pthread.h>
#include <stdbool.h>
#include <signal.h>

#include "utils.h"

bool santa_busy = false;
int waiting_reindeers = 0;
int waiting_elves = 0;
pthread_t workshop[ELVES_REQ];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_santa = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_elves = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_reindeers = PTHREAD_COND_INITIALIZER;
pthread_t santa;
pthread_t elves[ELVES_NO];
pthread_t reindeers[REINDEERS_NO];

struct sigaction sa;

void stophandler();
void close_threads();
void *start_routine_santa(void *context);
void *start_routine_reindeer(void *context);
void *start_routine_elf(void *context);

int main(void) {
    sigset_t sigset;
    sigfillset(&sigset);
    sigprocmask(SIG_SETMASK, &sigset, NULL);

    atexit(&stophandler);

    pthread_create(&santa, NULL, start_routine_santa, NULL);
    for (int i = 0; i < ELVES_NO; i++) {
        pthread_create(&elves[i], NULL, start_routine_elf, NULL);
    }
    for (int i = 0; i < REINDEERS_NO; i++) {
        pthread_create(&reindeers[i], NULL, start_routine_reindeer, NULL);
    }

    sigemptyset(&sigset);
    sigprocmask(SIG_SETMASK, &sigset, NULL);
    

    pthread_join(santa, NULL);

    close_threads();

    return 0;
}

void stophandler(void) {
    printf("KOńćże\n");
    pthread_cancel(santa);
    pthread_join(santa, NULL);
    close_threads();
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_santa);
    pthread_cond_destroy(&cond_elves);
    pthread_cond_destroy(&cond_reindeers);
}

void close_threads() {
    for (int i = 0; i < REINDEERS_NO; i++) {
        pthread_cancel(reindeers[i]);
        pthread_join(reindeers[i], NULL);
    }
    for (int i = 0; i < ELVES_NO; i++) {
        pthread_cancel(elves[i]);
        pthread_join(elves[i], NULL);
    }
}

void *start_routine_santa(void *context) {
    int jobs = 0;
    while (jobs < MAX_JOBS) {
        synchronized(&mutex, {
            while (waiting_reindeers < REINDEERS_REQ && waiting_elves < ELVES_REQ) {
                printf("Mikołaj: Zasypiam\n");
                pthread_cond_wait(&cond_santa, &mutex);
                printf("Mikołaj: Budzę się...\n");
            }
            santa_busy = true;
            if (waiting_reindeers == REINDEERS_REQ) {
                printf("Mikołaj: Jadę zawieźć prezenty\n");
                asynchronized(&mutex, { randsleep(2, 4); });
                waiting_reindeers = 0;
                jobs++;
                pthread_cond_broadcast(&cond_reindeers);
            }
            if (jobs == MAX_JOBS) {
                printf("Mikołaj: Kończę pracę na dzisiaj!\n");
                break;
            }
            if (waiting_elves == ELVES_REQ) {
                printf("Mikołaj: rozwiązuję problemy elfów");
                for (int i = 0; i < ELVES_REQ; i++) {
                    printf(" %ld", workshop[i]);
                    workshop[i] = 0;
                }
                printf("\n");
                asynchronized(&mutex, { randsleep(1, 2); });
                waiting_elves = 0;
                pthread_cond_broadcast(&cond_elves);
            }
            santa_busy = false;
        });
    }
    return NULL;
}

void *start_routine_reindeer(void *context) {
    pthread_t id = pthread_self();
    while (1) {
        randsleep(5, 10);
        synchronized(&mutex, {
            printf("Renifer: czeka %d reniferów na Mikołaja, %ld\n", ++waiting_reindeers, id);
            if (waiting_reindeers == REINDEERS_REQ) {
                printf("Renifer: wybudzam Mikołaja, %ld\n", id);
                pthread_cond_signal(&cond_santa);
                pthread_cond_broadcast(&cond_reindeers);
            }
            while (waiting_reindeers > 0) {
                pthread_cond_wait(&cond_reindeers, &mutex);
            }
        });
    }
    return NULL;
}

void *start_routine_elf(void *context) {
    pthread_t id = pthread_self();
    while (1) {
        randsleep(2, 5);
        synchronized(&mutex, {
            if (waiting_elves == ELVES_REQ) {
                printf("Elf: Czekam na powrót elfów, %ld\n", id);
                while (waiting_elves == ELVES_REQ) {
                    pthread_cond_wait(&cond_elves, &mutex);
                }
            }
            int i;
            if (waiting_elves < ELVES_REQ) {
                i = waiting_elves++;
                workshop[i] = id;
                printf("Elf: czeka %d elfów na Mikołaja, %ld\n", waiting_elves, id);
            }
            if (waiting_elves == ELVES_REQ) {
                printf("Elf: Wybudzam Mikołaja, %ld\n", id);
                pthread_cond_broadcast(&cond_elves);
                pthread_cond_signal(&cond_santa);
            }
            if (pthread_equal(workshop[i], id)) {
                pthread_cond_wait(&cond_elves, &mutex);
                printf("Elf: Mikołaj rozwiązuje problem, %ld\n", id);
            }
        });
    }
    return NULL;
}
