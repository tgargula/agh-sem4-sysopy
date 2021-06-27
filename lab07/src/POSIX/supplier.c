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

sem_t *table_on;
sem_t *table_ready;
struct pizzeria *pizzeria;

void handle_exit();

void deliver(struct pizzeria *pizzeria);

int main(void) {
    atexit(handle_exit);
    srand(time(NULL) * getpid());

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
        deliver(pizzeria);
    }

    return 0;
}

void deliver(struct pizzeria *pizzeria) {  

    sem_wait(table_ready);
    sem_post(table_on);

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

void handle_exit() {
    SEM_CLOSE(table_on);
    SEM_CLOSE(table_ready);

    if (munmap(pizzeria, sizeof(struct pizzeria)) == -1) {
        ERROR_MSG("Cannot detach shared memory!\n");
    }
}