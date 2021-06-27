#include "utils.h"

#define USAGE_INFO \
    "expected: <name> <'local'|'net'> </path/to/server/socket|ip:port>\n"

union server_address_t {
    char unix_path[MAX_UNIX_PATH_LEN];
    struct ipv4_attr_t {
        char address[MAX_ADDRESS_LEN];
        u_int16_t port;
    } ipv4_attr;
};

int sockfd;
char symbol;
union addr_union addr;

void init_unix_socket(union server_address_t server_address);
void init_ipv4_socket(union server_address_t server_address);
void handle_async_disconnect();
void handle_disconnect(struct message_t *msg);
void handle_end(struct message_t *msg);
void handle_move(struct message_t *msg);
void handle_name_taken_error(struct message_t *msg);
void handle_ping(struct message_t *msg);
void handle_start(struct message_t *msg);
void handle_update(struct message_t *msg);
void handle_waiting(struct message_t *msg);

int main(int argc, char **argv) {
    if (argc != 4) {
        ERROR_MSG("Illegal number of arguments! 3 required; %d provided!\n%s", argc - 1, USAGE_INFO);
    }

    char *connection_type = alloca(64 * sizeof(char));
    union server_address_t server_address;

    sprintf(connection_type, "%s", argv[2]);

    if (!strcmp(connection_type, "local")) {
        sprintf(server_address.unix_path, "%s", argv[3]);
        init_unix_socket(server_address);
    } else if (!strcmp(argv[2], "net")) {
        sprintf(server_address.ipv4_attr.address, "%s", strtok(argv[3], ":"));
        server_address.ipv4_attr.port = strtol(strtok(NULL, ":"), NULL, 10);
        init_ipv4_socket(server_address);
    } else {
        ERROR_MSG("Illegal second argument! You can use \"local\" or \"net\"\n%s", USAGE_INFO);
    }

    atexit(handle_async_disconnect);
    sigint_as_exit;

    struct message_t msg;
    msg.type = MSG_CONNECT;
    strncpy(msg.data.init.name, argv[1], CLIENT_NAME_LEN);
    sendto(sockfd, &msg, sizeof(msg), 0, &addr.sock, sizeof(addr));

    while (true) {
        read(sockfd, &msg, sizeof(struct message_t));
        switch (msg.type) {
            case MSG_DISCONNECT:
                handle_disconnect(&msg);
                exit(EXIT_SUCCESS);
            case MSG_END:
                handle_end(&msg);
                exit(EXIT_SUCCESS);
            case MSG_MOVE:
                handle_move(&msg);
                break;
            case MSG_NAME_TAKEN_ERROR:
                handle_name_taken_error(&msg);
                break;
            case MSG_PING:
                handle_ping(&msg);
                break;
            case MSG_START:
                handle_start(&msg);
                break;
            case MSG_UPDATE:
                handle_update(&msg);
                break;
            case MSG_WAITING:
                handle_waiting(&msg);
                break;
            default:
                ERROR_MSG("Illegal message type!\n");
        }
    }

    return 0;
}

void init_unix_socket(union server_address_t server_address) {
    
    struct sockaddr_un sockaddr;
    sockaddr.sun_family = AF_UNIX;
    strcpy(sockaddr.sun_path, server_address.unix_path);
    addr.sock_un = sockaddr;

    if ((sockfd = socket(AF_UNIX, CONNECTION_TYPE, 0)) == -1) {
        ERROR_MSG("Cannot create socket!\n");
    }

    if (connect(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) == -1) {
        ERROR_MSG("Cannot open the connection!\n");
    }
}

void init_ipv4_socket(union server_address_t server_address) {
    struct sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(server_address.ipv4_attr.port);

    if (inet_pton(AF_INET, server_address.ipv4_attr.address, &sockaddr.sin_addr) == -1) {
        ERROR_MSG("Cannot convert to the binary network format!\n");
    }

    if ((sockfd = socket(AF_INET, CONNECTION_TYPE, 0)) == -1) {
        ERROR_MSG("Cannot create socket!\n");
    }

    if (connect(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) == -1) {
        ERROR_MSG("Cannot open the connection!\n");
    }
}

void handle_async_disconnect() {
    struct message_t msg;
    msg.type = MSG_DISCONNECT;
    sendto(sockfd, &msg, sizeof(struct message_t), 0, &addr.sock, sizeof(addr));
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
}

void handle_disconnect(struct message_t *msg) {
    printf("Your opponent has left the game...\n");
}

void handle_end(struct message_t *msg) {
    clearline;
    printf("Game ended: ");
    if (msg->data.winner == 'd') {
        printf("DRAW!\n");
    } else if (msg->data.winner == symbol) {
        printf("YOU WON!\n");
    } else {
        printf("YOU LOST!\n");
    }
}

void handle_move(struct message_t *msg) {
    clearline;
    printf("Choose position (1-9): ");
    scanf("%d", &msg->data.move);
    sendto(sockfd, msg, sizeof(msg), 0, &addr.sock, sizeof(addr));
}

void handle_name_taken_error(struct message_t *msg) {
    printf("That name is already taken, exiting...\n");
}

void handle_ping(struct message_t *msg) {
    sendto(sockfd, &msg, sizeof(msg), 0, &addr.sock, sizeof(addr));
}

void handle_start(struct message_t *msg) {
    printf("Started game with user \'%s\'\n", msg->data.init.name);
    printf("You are playing as \'%c\'\n\n", msg->data.init.symbol);
    symbol = msg->data.init.symbol;
}

void handle_update(struct message_t *msg) {
    clearscreen;
    printf(" %c | %c | %c \n", msg->data.board[0], msg->data.board[1], msg->data.board[2]);
    printf(SEPARATOR);
    printf(" %c | %c | %c \n", msg->data.board[3], msg->data.board[4], msg->data.board[5]);
    printf(SEPARATOR);
    printf(" %c | %c | %c \n", msg->data.board[6], msg->data.board[7], msg->data.board[8]);
    printff("Waiting for your opponent...");
}

void handle_waiting(struct message_t *msg) {
    printff("Waiting for another player...");
}
