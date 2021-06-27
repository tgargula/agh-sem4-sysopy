#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#include "communication.h"

struct client {
    int pqd;
    int id;
    bool active;
};

struct list {
    struct client *client;
    size_t size;
};

volatile static int qd = -1;
volatile static int id = 0;
volatile static struct list clients;

int find(int id);
void deactivate(int id);
void removeclient(int id);

int main(void) {
    if (atexit(close_queue) == -1) {
        ERROR_MSG("Server: Cannot set an exit handler!\n");
    }

    if (signal(SIGINT, exit_handler) == SIG_ERR) {
        ERROR_MSG("Server: Cannot link an exit handler! You have to remove a queue manually!\n");
    };

    char *homedir;
    if ((homedir = getenv("HOME")) == NULL) {
        ERROR_MSG("Client: Cannot get $HOME variable!\n");
    }

    clients.size = 0;

    key_t key;
    if ((key = ftok(homedir, PROJECT_ID)) == -1) {
        ERROR_MSG("Server: Cannot get public key!\n");
    }

    struct msqid_ds queue;

    if ((qd = msgget(key, IPC_CREAT | IPC_EXCL | 0666)) == -1) {
        ERROR_MSG("Server: Cannot create queue!\n");
    } else {
        OUT_MSG("Server: Queue successfully created with id: %d\n", qd);
    }

    struct message message;
    while (1) {
        if (msgctl(qd, IPC_STAT, &queue) == -1) {
            ERROR_MSG("Server: Cannot get current state of public queue!\n");
        }

        if (msgrcv(qd, &message, MSG_SIZE, 0, 0) < 0) {
            ERROR_MSG("Server: Cannot receive message!\n");
        }

        if (&message != NULL) {
            switch (message.type) {
                case INIT:
                    handle_init(&message);
                    break;
                case CONNECT:
                    handle_connect(&message);
                    break;
                case LIST:
                    handle_list(&message);
                    break;
                case DISCONNECT:
                    handle_disconnect(&message);
                    break;
                case STOP:
                    handle_stop(&message);
                    break;
                default:
                    ERROR_MSG("Server: Received unknown message");
                    break;
            }
        }
    }

    return 0;
}

void handle_init(struct message *message) {
    OUT_MSG("Server: Successfully connected with the queue of id: %s\n", message->message_text);
    int pqd = strtol(message->message_text, NULL, 10);

    struct client client;
    client.active = true;
    client.id = message->sender_pid;
    client.pqd = pqd;
    clients.client = realloc(clients.client, ++clients.size * sizeof(struct client));
    clients.client[clients.size - 1] = client;

    struct message feedback;
    feedback.type = INIT;
    sprintf(feedback.message_text, "%d", message->sender_pid);
    feedback.sender_pid = getpid();
    if (msgsnd(pqd, &feedback, MSG_SIZE, 0) == -1) {
        ERROR_MSG("Server: Cannot send feedback message!\n");
    }
}

void handle_connect(struct message *message) {
    int id = strtol(message->message_text, NULL, 10);
    int pqd = find(id);
    int fqd = find(message->sender_pid);
    if (pqd == -1 || fqd == -1) {
        MSGSND(fqd, message, MSG_SIZE, 0);
        return;
    }

    deactivate(id);
    deactivate(message->sender_pid);

    OUT_MSG("Server: Successfully connected the client %d with the client %d\n", id, message->sender_pid);

    // SEND MESSAGE WITH QD TO CLIENTS
    sprintf(message->message_text, "%d", pqd);
    message->sender_pid = getpid();
    MSGSND(fqd, message, MSG_SIZE, 0);
    sprintf(message->message_text, "%d", fqd);
    MSGSND(pqd, message, MSG_SIZE, 0);
}

void handle_disconnect(struct message *message) {
    int id = message->sender_pid;
    for (int i = 0; i < clients.size; i++) {
        if (clients.client[i].id == id) {
            clients.client[i].active = true;
            break;
        }
    }
}

void handle_list(struct message *message) {
    struct message feedback;
    feedback.type = LIST;
    feedback.sender_pid = getpid();

    int pqd = find(message->sender_pid);

    strcpy(feedback.message_text, "––– LIST OF AVAILABLE CLIENTS –––\n\n");
    strcat(feedback.message_text, "ID\tPQD\tACTIVE\n");
    for (int i = 0; i < clients.size; i++) {
        char line[128];
        if (clients.client[i].id != message->sender_pid) {
            if (clients.client[i].active) {
                sprintf(line, "%d\t%d\tyes\n", clients.client[i].id, clients.client[i].pqd);
            } else {
                sprintf(line, "%d\t%d\tno\n", clients.client[i].id, clients.client[i].pqd);
            }
            strcat(feedback.message_text, line);
        }
    }

    strcat(feedback.message_text, "\n\n");

    MSGSND(pqd, &feedback, MSG_SIZE, 0);
}

void handle_stop(struct message *message) {
    removeclient(message->sender_pid);
}

int find(int id) {
    int pqd = -1;
    for (int i = 0; i < clients.size; i++) {
        if (clients.client[i].id == id) {
            if (!clients.client[i].active) {
                OUT_MSG("Server: Client with id %d is busy!\n", clients.client[i].id);
                return -1;
            }
            pqd = clients.client[i].pqd;
            break;
        }
    }
    if (pqd == -1) {
        OUT_MSG("Server: Cannot find client's queue descriptor!\n");
    }
    return pqd;
}

void deactivate(int id) {
    for (int i = 0; i < clients.size; i++) {
        if (clients.client[i].id == id) {
            clients.client[i].active = false;
            break;
        }
    }
}

void removeclient(int id) {
    int j = -1;
    for (int i = 0; i < clients.size; i++) {
        if (clients.client[i].id == id) {
            j = i;
            break;
        }
    }

    if (j != -1) {
        for (int i = j; i < clients.size - 1; i++) {
            clients.client[i] = clients.client[i + 1];
        }
        clients.size--;
        clients.client = realloc(clients.client, sizeof(struct client) * clients.size);
    }
}

void close_queue() {
    if (qd > -1) {
        if (msgctl(qd, IPC_RMID, NULL) == -1) {
            ERROR_MSG("Server: Cannot delete server's queue!\n");
        } else {
            OUT_MSG("Server: Queue successfully closed!\n");
        }
    }
}

void exit_handler(int _) {
    struct message message;
    message.sender_pid = getpid();
    message.type = STOP;
    for (int i = 0; i < clients.size; i++) {
        MSGSND(clients.client[i].pqd, &message, MSG_SIZE, 0);
    }
    exit(0);
}