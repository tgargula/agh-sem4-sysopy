#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <errno.h>
#include <signal.h>

#define STOP 5
#define DISCONNECT 4
#define LIST 3
#define CONNECT 2
#define INIT 1

#define MAX_QUEUE_SIZE 10
#define MSG_SIZE 2048
#define PATH "/server"

#define VERBOSE 1
#define OUT_MSG(format, ...)               \
    {                                      \
        if (VERBOSE) {                     \
            printf(format, ##__VA_ARGS__); \
            fflush(stdout);                \
        }                                  \
    }
#define ERROR_MSG(format, ...)                      \
    {                                               \
        if (VERBOSE) {                              \
            fprintf(stderr, format, ##__VA_ARGS__); \
            fprintf(stderr, "errno %d: ", errno);   \
            perror("");                             \
        }                                           \
        exit(-1);                                   \
    }

#define MQ_RECEIVE(__mqdes, __msq_ptr, __msg_len, __msg_prio)          \
    if (mq_receive(__mqdes, __msq_ptr, __msg_len, __msg_prio) == -1) { \
        ERROR_MSG("Server: Cannot receive message!\n");                \
    }

#define MQ_SEND(__mqdes, __msq_ptr, __msg_len, __msg_prio)          \
    if (mq_send(__mqdes, __msq_ptr, __msg_len, __msg_prio) == -1) { \
        ERROR_MSG("Client: Cannot send INIT message\n");            \
    }

#define MQ_GETATTR(__mqdes, __mqstat)                                \
    if (mq_getattr(__mqdes, __mqstat) == -1) {                       \
        ERROR_MSG("Client: Cannot read public queue parameters!\n"); \
    }

struct message {
    long type;
    pid_t sender_pid;
    char message_text[MSG_SIZE];
};

void close_queue();
void exit_handler(int);
void handle_init(struct message *message);
void handle_connect(struct message *message);
void handle_disconnect(struct message *message);
void handle_list(struct message *message);
void handle_stop(struct message *message);

void trim(char s[]);

#endif