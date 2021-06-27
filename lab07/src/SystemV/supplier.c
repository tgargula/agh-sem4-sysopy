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

void deliver(struct pizzeria *pizzeria);

void handle_exit() {
    perror("");
    exit(0);
}

int main(void) {
    atexit(handle_exit);
    srand(time(NULL) * getpid());

    key_t key;
    if ((key = ftok(PROJECT_PATH, PROJECT_ID)) == -1) {
        ERROR_MSG("Supplier: Could not get key!\n");
    }

    if ((semid = semget(key, 0, 0)) == -1) {
        ERROR_MSG("Supplier: Could not get semaphore!\n");
    }

    if ((shmid = shmget(key, sizeof(struct pizzeria *), 0)) == -1) {
        ERROR_MSG("Supplier: Could not get shared memory!\n");
    }

    struct pizzeria *pizzeria;
    if ((pizzeria = shmat(shmid, NULL, 0)) == (void *)(-1)) {
        ERROR_MSG("Supplier: Cannot attach shared memory!\n");
    }

    while (1) {
        deliver(pizzeria);
    }

    return 0;
}

void deliver(struct pizzeria *pizzeria) {
    struct sembuf table_on;
    table_on.sem_num = TABLE_ON;
    table_on.sem_op = 0;
    table_on.sem_flg = 0;

    struct sembuf table_ready;
    table_ready.sem_num = TABLE_READY;
    table_ready.sem_op = 0;
    table_ready.sem_flg = 0;    

    table_on.sem_op = 1;
    table_ready.sem_op = -1;
    semop(semid, (struct sembuf[]){table_on, table_ready}, 2);

    int index;
    int pizza_no;
    for (int i = 0; ; i++) {
        index = (i + pizzeria->table_queue_start) % TABLE_CAP;
        if (pizzeria->table[index] != FREE) {
            pizzeria->table_queue_start++;
            pizzeria->table_queue_start %= TABLE_CAP;
            pizza_no = pizzeria->table[index] - 1;
            pizzeria->tables_taken--;
            pizzeria->table[index] = FREE;
            break;
        }
    }

    print_timestamp(getpid());
    printf("Pobieram pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole %d\n", pizza_no, pizzeria->ovens_taken, pizzeria->tables_taken);
    
    randsleep(4, 5);

    print_timestamp(getpid());
    printf("Dostarczam pizze: %d\n", pizza_no);

    randsleep(4, 5);

}