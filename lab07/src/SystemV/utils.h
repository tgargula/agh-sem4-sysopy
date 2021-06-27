#ifndef __UTILS_H__
#define __UTILS_H__

#define ERROR_MSG(format, ...)                  \
    {                                           \
        fprintf(stderr, format, ##__VA_ARGS__); \
        perror("Reason");                       \
        exit(-1);                               \
    }

#define print_timestamp(pid) \
    printf("(%ld %d)\t", time(NULL), pid);

#define PROJECT_PATH getenv("HOME")
#define PROJECT_ID 0x01234

#define OVEN_CAP 5
#define TABLE_CAP 5

int randint(int from, int to);

#define SEMSZ 5  // Size of specs
enum specs {
    OVEN_PLACE,
    OVEN_IN,
    TABLE_PLACE,
    TABLE_ON,
    TABLE_READY
};

enum state {
    FREE,
    TAKEN
};

union semun {
    int val;               /* Value for SETVAL */
    struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* Array for GETALL, SETALL */
    struct seminfo *__buf; /* Buffer for IPC_INFO
                                (Linux-specific) */
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