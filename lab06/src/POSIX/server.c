#include <fcntl.h>
#include <mqueue.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "communication.h"

struct client {
    int pid;
    bool active;
};

struct list {
    struct client *client;
    size_t size;
};

volatile static int publicid = -1;
volatile static struct list clients;

void close_queue();
void exit_handler(int _);

void handle_init(struct message *message);
void handle_connect(struct message *message);
void handle_disconnect(struct message *message);
void handle_stop(struct message *message);
void handle_list(struct message *message);

void removeclient(int pid);
void deactivate(int pid);

int main(void) {
    if (atexit(close_queue) == -1) {
        ERROR_MSG("Server: Cannot set an exit handler!\n");
    }

    if (signal(SIGINT, exit_handler) == SIG_ERR) {
        ERROR_MSG("Server: Cannot link an exit handler! You have to remove a queue manually!\n");
    }

    clients.size = 0;

    struct mq_attr attr;
    attr.mq_maxmsg = MAX_QUEUE_SIZE;
    attr.mq_msgsize = MSG_SIZE;

    if ((publicid = mq_open(PATH, O_RDONLY | O_CREAT | O_EXCL, 0666, &attr)) == -1) {
        ERROR_MSG("Server: Cannot create queue\n");
    } else {
        OUT_MSG("Server: Queue successfully created!\n");
    }

    struct message message;

    while (1) {
        MQ_RECEIVE(publicid, (char *)&message, MSG_SIZE, NULL);

        if (&message != NULL) {
            switch (message.type) {
                case CONNECT:
                    handle_connect(&message);
                    break;
                case INIT:
                    handle_init(&message);
                    break;
                case DISCONNECT:
                    handle_disconnect(&message);
                    break;
                case STOP:
                    handle_stop(&message);
                    break;
                case LIST:
                    handle_list(&message);
                    break;
                default:
                    ERROR_MSG("Server: Received unknown message!\n");
            }
        }
    }

    return 0;
}

void handle_init(struct message *message) {
    OUT_MSG("Server: Successfully connected with the queue of id: %d\n", message->sender_pid);

    char cpath[20];
    sprintf(cpath, "/%d", message->sender_pid);

    int clientid;
    if ((clientid = mq_open(cpath, O_WRONLY)) == -1) {
        ERROR_MSG("Cannot open client's queue!\n");
    }

    struct client client;
    client.active = true;
    client.pid = message->sender_pid;
    clients.client = realloc(clients.client, ++clients.size * sizeof(struct client));
    clients.client[clients.size - 1] = client;

    message->sender_pid = getpid();
    MQ_SEND(clientid, (char *)message, MSG_SIZE, message->type);
}

void handle_connect(struct message *message) {
    int id = strtol(message->message_text, NULL, 10);
    char key1[20];
    sprintf(key1, "/%d", message->sender_pid);
    char key2[20];
    sprintf(key2, "/%d", id);

    deactivate(message->sender_pid);
    deactivate(id);
    OUT_MSG("Server: Successfully connected the client %d with the client %d\n", message->sender_pid, id);

    // SEND MESSAGE WITH QD TO CLIENTS
    int privateid1;
    int privateid2;

    if ((privateid1 = mq_open(key1, O_WRONLY)) == -1) {
        ERROR_MSG("Cannot open client's queue!\n");
    }
    if ((privateid2 = mq_open(key2, O_WRONLY)) == -1) {
        ERROR_MSG("Cannot open client's queue!\n");
    }
    message->sender_pid = getpid();
    sprintf(message->message_text, "%s", key1);
    MQ_SEND(privateid2, (char *)message, MSG_SIZE, message->type);
    sprintf(message->message_text, "%s", key2);
    MQ_SEND(privateid1, (char *)message, MSG_SIZE, message->type);
}

void handle_disconnect(struct message *message) {
    int pid = message->sender_pid;
    for (int i = 0; i < clients.size; i++) {
        if (clients.client[i].pid == pid) {
            clients.client[i].active = true;
            break;
        }
    }
}

void handle_stop(struct message *message) {
    removeclient(message->sender_pid);
}

void handle_list(struct message *message) {
    message->type = LIST;

    char cpath[20];
    sprintf(cpath, "/%d", message->sender_pid);

    strcpy(message->message_text, "––– LIST OF AVAILABLE CLIENTS –––\n\n");
    strcat(message->message_text, "ID\tACTIVE\n");
    for (int i = 0; i < clients.size; i++) {
        char line[128];
        if (clients.client[i].pid != message->sender_pid) {
            if (clients.client[i].active) {
                sprintf(line, "%d\tyes\n", clients.client[i].pid);
            } else {
                sprintf(line, "%d\tno\n", clients.client[i].pid);
            }
            strcat(message->message_text, line);
        }
    }

    strcat(message->message_text, "\n\n");

    printf("%s\n", cpath);

    int clientid;
    if ((clientid = mq_open(cpath, O_WRONLY)) == -1) {
        ERROR_MSG("Cannot open client's queue!\n");
    }
    MQ_SEND(clientid, (char *)message, MSG_SIZE, message->type);
}

void close_queue() {
    if (publicid > -1) {
        if (mq_close(publicid) == -1) {
            ERROR_MSG("Server: Cannot close server's queue!\n");
        } else {
            OUT_MSG("Server: Queue successfully closed!\n");
        }

        if (mq_unlink(PATH) == -1) {
            ERROR_MSG("Server: Cannot delete server's queue!\n");
        } else {
            OUT_MSG("Server: Queue successfully deleted!\n");
        }
    }
}

void removeclient(int pid) {
    int j = -1;
    for (int i = 0; i < clients.size; i++) {
        if (clients.client[i].pid == pid) {
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

void deactivate(int pid) {
    for (int i = 0; i < clients.size; i++) {
        if (clients.client[i].pid == pid) {
            clients.client[i].active = false;
            break;
        }
    }
}

void exit_handler(int _) {
    struct message message;
    message.sender_pid = getpid();
    message.type = STOP;
    for (int i = 0; i < clients.size; i++) {
        char path[20];
        sprintf(path, "/%d", clients.client[i].pid);
        int clientid;
        if ((clientid = mq_open(path, O_WRONLY)) == -1) {
            ERROR_MSG("Cannot open client's queue!\n");
        }
        MQ_SEND(clientid, (char *)&message, MSG_SIZE, message.type);
    }
    exit(0);
}