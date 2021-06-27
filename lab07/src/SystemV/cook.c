#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "utils.h"

int semid;
int shmid;

void make_pizza();

void handle_exit() {
    perror("");
    exit(0);
}

int main(void) {
    atexit(handle_exit);
    srand(time(NULL) * getpid());

    key_t key;
    if ((key = ftok(PROJECT_PATH, PROJECT_ID)) == -1) {
        ERROR_MSG("Cook: Could not get key!\n");
    }

    if ((semid = semget(key, 0, 0)) == -1) {
        ERROR_MSG("Cook: Could not get semaphore!\n");
    }

    if ((shmid = shmget(key, sizeof(struct pizzeria *), 0)) == -1) {
        ERROR_MSG("Cook: Could not get shared memory!\n");
    }

    struct pizzeria *pizzeria;
    if ((pizzeria = shmat(shmid, NULL, 0)) == (void *)(-1)) {
        ERROR_MSG("Cook: Cannot attach shared memory!\n");
    }

    while (1) {
        make_pizza(pizzeria);
    }

    return 0;
}

void make_pizza(struct pizzeria *pizzeria) {
    int pizza_no = randint(0, 10);

    struct sembuf oven_place;
    oven_place.sem_num = OVEN_PLACE;
    oven_place.sem_op = 0;
    oven_place.sem_flg = 0;

    struct sembuf oven_in;
    oven_in.sem_num = OVEN_IN;
    oven_in.sem_op = 0;
    oven_in.sem_flg = 0;

    struct sembuf table_place;
    table_place.sem_num = TABLE_PLACE;
    table_place.sem_op = 0;
    table_place.sem_flg = 0;

    struct sembuf table_on;
    table_on.sem_num = TABLE_ON;
    table_on.sem_op = 0;
    table_on.sem_flg = 0;

    struct sembuf table_ready;
    table_ready.sem_num = TABLE_READY;
    table_ready.sem_op = 0;
    table_ready.sem_flg = 0;

    // Preparing pizza
    print_timestamp(getpid());
    printf("Przygotowuje pizze %d\n", pizza_no);
    randsleep(1, 2);

    // Putting pizza in the oven
    oven_place.sem_op = -1;
    oven_in.sem_op = -1;
    semop(semid, (struct sembuf[]){oven_place, oven_in}, 2);

    int index;
    pizzeria->ovens_taken++;
    for (int i = 0;; i++) {
        if (pizzeria->oven[i] == FREE) {
            index = i;
            pizzeria->oven[i] = TAKEN;
            break;
        }
    }

    print_timestamp(getpid());
    printf("Dodalem pizze: %d. Liczba pizz w piecu: %d\n", pizza_no, pizzeria->ovens_taken);

    oven_place.sem_op = 1;
    semop(semid, &oven_place, 1);

    randsleep(4, 5);

    // Pulling pizza out of the oven

    oven_place.sem_op = -1;
    semop(semid, &oven_place, 1);

    pizzeria->ovens_taken--;
    pizzeria->oven[index] = FREE;

    oven_in.sem_op = 1;
    oven_place.sem_op = 1;
    semop(semid, (struct sembuf[]){oven_place, oven_in}, 2);

    // Placing pizza on the table

    table_place.sem_op = -1;
    table_on.sem_op = -1;
    semop(semid, (struct sembuf[]){table_place, table_on}, 2);

    for (int i = 0;; i++) {
        if (pizzeria->table[i] == FREE) {
            index = i;
            pizzeria->table[i] = pizza_no + 1; // 0 means free
            pizzeria->tables_taken++;
            break;
        }
    }

    print_timestamp(getpid());
    printf("Wyjmuje pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d\n", pizza_no, pizzeria->ovens_taken, pizzeria->tables_taken);

    table_place.sem_op = 1;
    table_ready.sem_op = 1;
    semop(semid, (struct sembuf[]){table_place, table_ready}, 2);
}