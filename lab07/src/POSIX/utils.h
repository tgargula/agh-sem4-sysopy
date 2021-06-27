#ifndef __UTILS_H__
#define __UTILS_H__

#define ERROR_MSG(format, ...)                  \
    {                                           \
        fprintf(stderr, format, ##__VA_ARGS__); \
        perror("Reason");                       \
        exit(-1);                               \
    }

#define OVEN_CAP 5
#define TABLE_CAP 5

#define OVEN_PLACE "/pizzeria_oven_place"
#define OVEN_IN "/pizzeria_oven_in"
#define TABLE_PLACE "/pizzeria_table_place"
#define TABLE_ON "/pizzeria_table_on"
#define TABLE_READY "/pizzeria_table_ready"
#define SHM_PATH "/pizzeria"

#define print_timestamp(pid) \
    printf("(%ld %d)\t", time(NULL), pid);

#define SEM_CLOSE(__sem) \
    if (sem_close(__sem) == -1) ERROR_MSG("Cannot close semaphore!\n");

#define SEM_UNLINK(__path) \
    if (sem_unlink(__path) == -1) ERROR_MSG("Cannot unlink semaphore!\n");

#define SEM_CREATE(__sem, __path, __flags, __mode, __size) \
    if ((__sem = sem_open(__path, __flags, __mode, __size)) == SEM_FAILED) ERROR_MSG("Cannot create semaphore!\n");

#define SEM_OPEN(__sem, __path, __flags) \
    if ((__sem = sem_open(__path, __flags)) == SEM_FAILED) ERROR_MSG("Cannot open semaphore!\n");

enum state {
    FREE,
    TAKEN
};

struct pizzeria {
    int oven[OVEN_CAP];
    int ovens_taken;
    int table[TABLE_CAP];
    int tables_taken;
    int table_queue_start;
};

int randint(int from, int to) {
    return rand() % (to - from) + from;
}

void randsleep(int from, int to) {
    usleep(randint(from * 1000000, to * 1000000));
}

#endif