#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#include "communication.h"

volatile static int qd = -1;
volatile static int pqd = -1;
volatile static int fqd = -1;

bool inputAvailable();

void init(struct message *message);
void connect(struct message *message);
void list(struct message *message);
void disconnect(struct message *message);
void stop();
void direct_connection(int dqd);

int main(int argc, char **argv) {
    if (atexit(close_queue) == -1) {
        ERROR_MSG("Server: Cannot set an exit handler!\n");
    }

    struct msqid_ds queue;
    key_t publicKey;
    key_t privateKey;

    char *homedir;
    if ((homedir = getenv("HOME")) == NULL) {
        ERROR_MSG("Client: Cannot get $HOME variable!\n");
    }

    if (signal(SIGINT, exit_handler) == SIG_ERR) {
        ERROR_MSG("Client: Cannot link an exit handler! You have to remove a queue manually!\n");
    };

    if ((publicKey = ftok(homedir, PROJECT_ID)) == -1) {
        ERROR_MSG("Client: Cannot get public key!\n");
    }

    if ((qd = msgget(publicKey, 0666)) == -1) {
        ERROR_MSG("Client: Cannot connect to the queue!\n");
    } else {
        OUT_MSG("Client: Successfully connected with the queue: %d\n", qd);
    }

    if ((privateKey = ftok(homedir, getpid())) == -1) {
        ERROR_MSG("Client: Cannot get private key!\n");
    }

    if ((pqd = msgget(privateKey, IPC_CREAT | IPC_EXCL | 0666)) == -1) {
        ERROR_MSG("Client: Cannot create queue!\n");
    } else {
        OUT_MSG("Client: Queue successfully created with id: %d\n", pqd);
    }

    struct message message;
    struct message feedback;

    message.sender_pid = getpid();
    init(&message);

    char command[10];

    printf("Client: Enter request: ");
    fflush(stdout);
    while (1) {
        MSGCTL(pqd, IPC_STAT, &queue);
        if (queue.msg_qnum > 0) {
            printf("%d\n", pqd);
            MSGRCV(pqd, &message, MSG_SIZE, 0, 0);
            if (message.type == CONNECT) {
                printf("\n");
                int dqd = strtol(message.message_text, NULL, 10);
                direct_connection(dqd);
            } else if (message.type == STOP) {
                stop();
            }
        }
        if (inputAvailable()) {
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

void trim(char s[]) {
    int x = strlen(s);
    if (s[x - 1] == '\n') {
        s[x - 1] = '\0';
    }
}

void exit_handler(int _) {
    exit(0);
}

void close_queue() {
    if (pqd > -1) {
        if (msgctl(pqd, IPC_RMID, NULL) == -1) {
            ERROR_MSG("Client: Cannot delete client's queue!\n");
        } else {
            OUT_MSG("Client: Queue successfully closed!\n");
        }
    }
}

void init(struct message *message) {
    message->type = INIT;
    sprintf(message->message_text, "%d", pqd);
    message->sender_pid = getpid();
    if (msgsnd(qd, message, MSG_SIZE, 0) == -1) {
        ERROR_MSG("Client: Cannot send INIT message!\n");
    }
    MSGRCV(pqd, message, MSG_SIZE, 0, 0);
}

void connect(struct message *message) {
    message->type = CONNECT;
    message->sender_pid = getpid();

    printf("Client: Enter client_id: ");
    if (fgets(message->message_text, MSG_SIZE, stdin) == NULL) {
        ERROR_MSG("Client: An error occured while reading message!\n");
    }

    trim(message->message_text);

    // REQUEST TO SERVER
    MSGSND(qd, message, MSG_SIZE, 0);
    MSGRCV(pqd, message, MSG_SIZE, 0, 0);
    if (message->sender_pid == getpid()) {
        OUT_MSG("Client: Cannot connect to the client with id %s", message->message_text);
        OUT_MSG("Client: Enter request: ");
        return;
    }

    fqd = strtol(message->message_text, NULL, 10);
    direct_connection(fqd);
}

void list(struct message *message) {
    message->type = LIST;
    message->sender_pid = getpid();

    if (msgsnd(qd, message, MSG_SIZE, 0) == -1) {
        ERROR_MSG("Client: Cannot send LIST message!\n");
    }
    MSGRCV(pqd, message, MSG_SIZE, 0, 0);
    printf("%s", message->message_text);
}

void disconnect(struct message *message) {
    message->type = DISCONNECT;
    message->sender_pid = getpid();
    sprintf(message->message_text, "%d", pqd);
    MSGSND(qd, message, MSG_SIZE, 0);
    signal(SIGINT, exit_handler);
    OUT_MSG("Client: Enter request: ");
}

void stop() {
    struct message message;
    disconnect(&message);
    message.type = STOP;
    message.sender_pid = getpid();
    if (qd != -1) {
        MSGSND(qd, &message, MSG_SIZE, 0);
    }
    exit(0);
}

void direct_connection(int dqd) {
    signal(SIGINT, SIG_IGN);
    printf("> ");
    fflush(stdout);
    struct msqid_ds queue;
    struct message message;
    while (1) {
        MSGCTL(pqd, IPC_STAT, &queue);
        if (queue.msg_qnum > 0) {
            MSGRCV(pqd, &message, MSG_SIZE, 0, 0);
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
            MSGSND(dqd, &message, MSG_SIZE, 0);
            disconnect(&message);
            break;
        }
        MSGSND(dqd, &message, MSG_SIZE, 0);
        printf("> ");
        fflush(stdout);
    }
}