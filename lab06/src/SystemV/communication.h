#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <errno.h>
#include <signal.h>

#define STOP 1
#define DISCONNECT 2
#define LIST 3
#define CONNECT 4
#define INIT 5

#define PROJECT_ID 0x02137
#define MSG_SIZE 2048

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

#define MSGSND(__msqid, __msgp, __msgsz, __msgflg)            \
    if (msgsnd(__msqid, __msgp, __msgsz, __msgflg) == -1) {   \
        ERROR_MSG("Server: Cannot send feedback message!\n"); \
    }

#define MSGRCV(__msqid, __msgp, __msgsz, __msgtyp, __msgflg)        \
    if (msgrcv(__msqid, __msgp, __msgsz, __msgtyp, __msgflg) < 0) { \
        ERROR_MSG("Server: Cannot receive message!\n");             \
    }

#define MSGCTL(__msqid, __cmd, __buf)                                         \
    if (msgctl(__msqid, __cmd, __buf) == -1) {                                \
        if (__cmd == IPC_STAT)                                                \
            ERROR_MSG("Server: Cannot get current state of public queue!\n"); \
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