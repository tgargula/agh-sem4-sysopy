#include <fcntl.h>
#include <mqueue.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "communication.h"

volatile static int publicid = -1;
volatile static int privateid = -1;

void close_queue();
void exit_handler(int _);

void init(struct message *message);
void connect(struct message *message);
void list(struct message *message);
void disconnect(struct message *message);
void stop();
void direct_connection(char *key);

bool inputAvailable();

void trim(char s[]);

int main(void) {
    if (atexit(close_queue) == -1) {
        ERROR_MSG("Client: Cannot set an exit handler!\n");
    }

    if (signal(SIGINT, exit_handler) == SIG_ERR) {
        ERROR_MSG("Client: Cannot link an exit handler! You have to remove a queue manually!\n");
    }

    struct mq_attr attr;
    attr.mq_maxmsg = MAX_QUEUE_SIZE;
    attr.mq_msgsize = MSG_SIZE;

    if ((publicid = mq_open(PATH, O_RDWR, 0666, &attr)) == -1) {
        ERROR_MSG("Client: Cannot connect to server's queue!\n");
    } else {
        OUT_MSG("Client: Successfully connected to server's queue\n");
    }

    char ppath[10];
    sprintf(ppath, "/%d", getpid());

    if ((privateid = mq_open(ppath, O_RDONLY | O_CREAT | O_EXCL, 0666, &attr)) == -1) {
        ERROR_MSG("Client: Cannot create queue\n");
    } else {
        OUT_MSG("Client: Queue successfully created!\n");
    }

    struct message message;

    init(&message);

    struct mq_attr state;

    printf("Client: Enter request: ");
    fflush(stdout);
    while (1) {
        MQ_GETATTR(privateid, &state);
        if (state.mq_curmsgs > 0) {
            MQ_RECEIVE(privateid, (char *)&message, MSG_SIZE, NULL);
            if (message.type == CONNECT) {
                printf("\n");
                direct_connection(message.message_text);
            } else if (message.type == STOP) {
                stop();
            }
        }

        if (inputAvailable()) {
            char command[10];
            if (fgets(command, 10, stdin) == NULL) {
                OUT_MSG("Client: An error occurred while reading command. Reinitializing...");
                continue;
            }

            trim(command);

            if (!strcmp(command, "CONNECT")) {
                connect(&message);
            } else if (!strcmp(command, "LIST")) {
                list(&message);
                OUT_MSG("Client: Enter request: ");
            } else if (!strcmp(command, "DISCONNECT")) {
                OUT_MSG("Client: Client is not connected!\n");
            } else if (!strcmp(command, "STOP")) {
                stop();
            } else {
                OUT_MSG("Client: Unknown command\n");
                continue;
            }
        } else {
            usleep(1000);
        }
    }

    return 0;
}

void trim(char s[]) {
    int x = strlen(s);
    if (s[x - 1] == '\n') {
        s[x - 1] = '\0';
    }
}

void close_queue() {
    if (privateid > -1) {
        if (mq_close(privateid) == -1) {
            ERROR_MSG("Client: Cannot close queue!\n");
        } else {
            OUT_MSG("Client: Queue successfully closed!\n");
        }

        char ppath[10];
        sprintf(ppath, "/%d", getpid());

        if (mq_unlink(ppath) == -1) {
            ERROR_MSG("Client: Cannot delete queue!\n");
        } else {
            OUT_MSG("Client: Queue successfully deleted!\n");
        }
    }
}

void exit_handler(int _) {
    exit(0);
}

bool inputAvailable() {
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return (FD_ISSET(0, &fds));
}

void init(struct message *message) {
    message->type = INIT;
    message->sender_pid = getpid();
    sprintf(message->message_text, "%d", privateid);
    MQ_SEND(publicid, (char *)message, MSG_SIZE, 0);
    MQ_RECEIVE(privateid, (char *)message, MSG_SIZE, NULL);
}

void connect(struct message *message) {
    message->type = CONNECT;
    message->sender_pid = getpid();

    printf("Client: Enter client_id: ");
    if (fgets(message->message_text, MSG_SIZE, stdin) == NULL) {
        ERROR_MSG("Client: An error occured while reading message!\n");
    }

    MQ_SEND(publicid, (char *)message, MSG_SIZE, 0);
    MQ_RECEIVE(privateid, (char *)message, MSG_SIZE, NULL);

    if (message->sender_pid == getpid()) {
        OUT_MSG("Client: Cannot connect to the client!\n");
        OUT_MSG("Client: Enter request: ");
        return;
    }

    char key[20];
    direct_connection(message->message_text);
}

void list(struct message *message) {
    message->type = LIST;
    message->sender_pid = getpid();

    MQ_SEND(publicid, (char *)message, MSG_SIZE, 0);
    MQ_RECEIVE(privateid, (char *)message, MSG_SIZE, NULL);
    printf("%s", message->message_text);
}

void disconnect(struct message *message) {
    message->type = DISCONNECT;
    message->sender_pid = getpid();
    MQ_SEND(publicid, (char *)message, MSG_SIZE, 0);
    signal(SIGINT, exit_handler);
    OUT_MSG("Client: Enter request: ");
}

void stop() {
    struct message message;
    disconnect(&message);
    message.type = STOP;
    message.sender_pid = getpid();
    if (publicid != -1) {
        MQ_SEND(publicid, (char *)&message, MSG_SIZE, 0);
    }
    exit(0);
}

void direct_connection(char *key) {
    printf("%s\n", key);
    signal(SIGINT, SIG_IGN);
    printf("> ");
    fflush(stdout);
    struct mq_attr state;
    struct message message;
    int clientid = mq_open(key, O_WRONLY);
    while (1) {
        MQ_GETATTR(privateid, &state);
        if (state.mq_curmsgs > 0) {
            MQ_RECEIVE(privateid, (char *)&message, MSG_SIZE, NULL);
            if (!strcmp(message.message_text, "DISCONNECT\n")) {
                printf("\n");
                disconnect(&message);
                break;
            }
            printf("\r%s", message.message_text);
            printf("> ");
            fflush(stdout);
        }

        if (!inputAvailable()) {
            usleep(10000);
            continue;
        }
        if (fgets(message.message_text, MSG_SIZE, stdin) == NULL) {
            OUT_MSG("Client: An error occurred while reading message. Reinitializing...");
            continue;
        }
        if (!strcmp(message.message_text, "DISCONNECT\n")) {
            MQ_SEND(clientid, (char *)&message, MSG_SIZE, 0);
            disconnect(&message);
            break;
        }
        MQ_SEND(clientid, (char *)&message, MSG_SIZE, 0);
        printf("> ");
        fflush(stdout);
    }
}
