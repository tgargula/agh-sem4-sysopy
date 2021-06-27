#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "utils.h"

sem_t *oven_place;
sem_t *oven_in;
sem_t *table_place;
sem_t *table_on;
sem_t *table_ready;
struct pizzeria *pizzeria;

void make_pizza(struct pizzeria *pizzeria);
void handle_exit();

int main(void) {
    atexit(handle_exit);
    srand(time(NULL) * getpid());

    SEM_OPEN(oven_place, OVEN_PLACE, O_RDWR);
    SEM_OPEN(oven_in, OVEN_IN, O_RDWR);
    SEM_OPEN(table_place, TABLE_PLACE, O_RDWR);
    SEM_OPEN(table_on, TABLE_ON, O_RDWR);
    SEM_OPEN(table_ready, TABLE_READY, O_RDWR);

    int fd;
    if ((fd = shm_open(SHM_PATH, O_RDWR, 0)) == -1) {
        ERROR_MSG("Cannot get shared memory!\n");
    }

    if ((pizzeria = (struct pizzeria *)mmap(NULL, sizeof(struct pizzeria), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == (void *)-1) {
        ERROR_MSG("Cannot attach shared memory!\n");
    }

    while (1) {
        make_pizza(pizzeria);
    }

    return 0;
}

void handle_exit() {
    SEM_CLOSE(oven_place);
    SEM_CLOSE(oven_in);
    SEM_CLOSE(table_place);
    SEM_CLOSE(table_on);
    SEM_CLOSE(table_ready);

    if (munmap(pizzeria, sizeof(struct pizzeria)) == -1) {
        ERROR_MSG("Cannot detach shared memory!\n");
    }
}

void make_pizza(struct pizzeria *pizzeria) {
    int pizza_no = randint(0, 10);

    // Preparing pizza
    print_timestamp(getpid());
    printf("Przygotowuje pizze %d\n", pizza_no);
    randsleep(1, 2);

    // Putting pizza in the oven
    sem_wait(oven_in);
    sem_wait(oven_place);

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

    sem_post(oven_place);

    randsleep(4, 5);

    // Pulling pizza out of the oven

    sem_post(oven_place);

    pizzeria->ovens_taken--;
    pizzeria->oven[index] = FREE;

    sem_post(oven_place);
    sem_post(oven_in);

    // Placing pizza on the table

    sem_wait(table_on);
    int tables_taken = pizzeria->tables_taken;
    sem_wait(table_place);

    for (int i = 0;; i++) {
        if (pizzeria->table[i] == FREE) {
            index = i;
            pizzeria->table[i] = pizza_no + 1;  // 0 means free
            pizzeria->tables_taken++;
            break;
        }
    }

    print_timestamp(getpid());
    printf("Wyjmuje pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d\n", pizza_no, pizzeria->ovens_taken, tables_taken + 1);

    sem_post(table_ready);
    sem_post(table_place);
}