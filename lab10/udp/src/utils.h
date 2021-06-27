#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define ERROR_MSG(format, ...)              \
    fprintf(stderr, format, ##__VA_ARGS__); \
    perror("Reason:");                      \
    exit(EXIT_FAILURE);

#define synchronized(mutex, codeblock) \
    pthread_mutex_lock(mutex);         \
    codeblock                          \
        pthread_mutex_unlock(mutex);

#define asynchronized(mutex, codeblock) \
    pthread_mutex_unlock(mutex);        \
    codeblock                           \
        pthread_mutex_lock(mutex);

void exit_success() {
    exit(EXIT_SUCCESS);
}

#define printff(format, ...)       \
    printf(format, ##__VA_ARGS__); \
    fflush(stdout)

#define synchronized_printf(mutex, format, ...) \
    synchronized(mutex, {                       \
        printf(format, ##__VA_ARGS__);         \
    });

#define clearscreen printf("\e[1;1H\e[2J")
#define clearline printff("\r                               \r")
#define sigint_as_exit                 \
    {                                  \
        struct sigaction act;          \
        sigemptyset(&act.sa_mask);     \
        act.sa_flags = 0;              \
        act.sa_handler = exit_success; \
        sigaction(SIGINT, &act, NULL); \
    }

#define CLIENT_NAME_LEN 16
#define CONNECTION_TYPE SOCK_DGRAM
#define MAX_CLIENT_COUNT 32
#define TIMEOUT 1000
#define MAX_QUEUE_LEN 10
#define BOARD_SIZE 9
#define SEPARATOR "–––+–––+–––\n"
#define MAX_UNIX_PATH_LEN 64
#define MAX_ADDRESS_LEN 64
#define TICK1 'O'
#define TICK2 'X'

#define check(retval)                             \
    ({                                            \
        int error = retval;                       \
        if (error == -1) perror(strerror(errno)); \
        assert(error != -1);                      \
        error;                                    \
    })

struct message_t {
    union message_data_t {
        struct init_t {
            char name[CLIENT_NAME_LEN];
            char symbol;
        } init;
        char board[BOARD_SIZE];
        char winner;
        int move;
    } data;
    enum message_type_t {
        MSG_CONNECT,
        MSG_DISCONNECT,
        MSG_END,
        MSG_MOVE,
        MSG_NAME_TAKEN_ERROR,
        MSG_PING,
        MSG_START,
        MSG_UPDATE,
        MSG_WAITING
    } type;
};

union addr_union {
    struct sockaddr sock;
    struct sockaddr_un sock_un;
    struct sockaddr_in sock_in;
};

struct client_t {
    union addr_union addr;
    socklen_t socklen;
    bool taken;
    bool playing;
    char name[CLIENT_NAME_LEN];
    int socket_fd;
    pthread_mutex_t mutex;
};

struct accepting_routine_args {
    socklen_t socklen;
    struct message_t msg;
    union addr_union addr;
    int socket_fd;
};