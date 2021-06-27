#include <sys/epoll.h>

#include "utils.h"

int local_socket_fd;
int net_socket_fd;
int epoll_fd;
char* path;

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
struct client_t clients[MAX_CLIENT_COUNT];

void handle_exit();
void check_port(u_int16_t port);
void init_clients();
void init_sockets(u_int16_t port, char* path);
void init_single_socket(int* socketfd, int domain, struct sockaddr* addr, socklen_t len);
void init_epoll();
void init_threads();
void* accepting_routine(void* args);
int check_username(int client_fd, struct message_t* msg);
int create_client(int client_fd, struct message_t* msg);
int connect_pending(int client_fd, struct message_t* msg);
void* pinger_routine(void* args);
void game_connection(int player1, int player2);
void send_msg_start(struct message_t* msg, int player1, int player2);
void send_msg_update(struct message_t* msg, int player1, int player2, char* board);
void send_msg_move(struct message_t* msg, int player);
void send_msg_disconnect(struct message_t* msg, int player);
void send_msg_waiting(struct message_t* msg, int player);
void send_msg_end(struct message_t* msg, int player1, int player2, char winner);
char check_board(char* board);
char check_line(char field1, char field2, char field3);
void disconnect_client(int id);

int main(int argc, char** argv) {
    if (argc != 3) {
        ERROR_MSG("Illegal number of arguments! Required 2; provided %d\n", argc - 1);
    }

    atexit(handle_exit);
    sigint_as_exit;

    u_int16_t port = strtol(argv[1], NULL, 10);
    path = argv[2];

    check_port(port);

    init_clients();
    init_sockets(port, path);
    init_epoll();
    init_threads();

    return 1;
}

void handle_exit() {
    close(local_socket_fd);
    close(net_socket_fd);
    close(epoll_fd);
    shutdown(local_socket_fd, SHUT_RDWR);
    shutdown(net_socket_fd, SHUT_RDWR);
    unlink(path);
    synchronized_printf(&print_mutex, "\n");
}

void check_port(u_int16_t port) {
    if (port != 0 && port < 1024) {
        errno = EINVAL;
        ERROR_MSG("Illegal port number!\nPort number must be between 1024 and 65535 or equal to 0! (%d provided)\n", port);
    }
}

void init_clients() {
    for (int i = 0; i < MAX_CLIENT_COUNT; ++i) {
        clients[i].playing = false;
        clients[i].taken = false;
        pthread_mutex_init(&clients[i].mutex, NULL);
    }
}

void init_sockets(u_int16_t port, char* path) {
    struct sockaddr_un addr_unix;
    addr_unix.sun_family = AF_UNIX;
    sprintf(addr_unix.sun_path, "%s", path);

    struct sockaddr_in addr_net;
    addr_net.sin_family = AF_INET;
    addr_net.sin_port = htons(port);
    addr_net.sin_addr.s_addr = INADDR_ANY;

    init_single_socket(&local_socket_fd, AF_UNIX, (struct sockaddr*)&addr_unix, sizeof(addr_unix));
    init_single_socket(&net_socket_fd, AF_INET, (struct sockaddr*)&addr_net, sizeof(addr_net));

    listen(local_socket_fd, MAX_QUEUE_LEN);
    listen(net_socket_fd, MAX_QUEUE_LEN);
    synchronized_printf(&print_mutex, "Listening on UNIX at %s\n", path);
    synchronized_printf(&print_mutex, "Listening on IPV4 on port: %d\n", port);
}

void init_single_socket(int* socketfd, int domain, struct sockaddr* addr, socklen_t len) {
    if ((*socketfd = socket(domain, CONNECTION_TYPE, 0)) == -1) {
        ERROR_MSG("Cannot create socket!\n");
    }
    int optval = 1;
    setsockopt(*socketfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(optval));
    if (bind(*socketfd, addr, len) == -1) {
        ERROR_MSG("Cannot give the socket the local address!\n");
    }
}

void init_epoll() {
    epoll_fd = epoll_create1(0);

    struct epoll_event epoll_event_unix;
    epoll_event_unix.events = EPOLLIN;
    epoll_event_unix.data.fd = local_socket_fd;

    struct epoll_event epoll_event_net;
    epoll_event_net.events = EPOLLIN;
    epoll_event_net.data.fd = net_socket_fd;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, local_socket_fd, &epoll_event_unix);
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, net_socket_fd, &epoll_event_net);
}

void init_threads() {
    pthread_t pinger;
    pthread_create(&pinger, NULL, pinger_routine, NULL);
    pthread_detach(pinger);
    while (true) {
        struct epoll_event events[2];
        int available = epoll_wait(epoll_fd, events, 2, -1);
        for (int i = 0; i < available; ++i) {
            pthread_t acceptor;
            pthread_create(&acceptor, NULL, accepting_routine, (void*)&events[i].data.fd);
            pthread_detach(acceptor);
        }
    }
}

void* accepting_routine(void* args) {
    int* socket_fd = (int*)args;
    int client_fd = accept(*socket_fd, NULL, NULL);

    struct message_t msg;
    read(client_fd, &msg, sizeof(struct message_t));

    if (msg.type != MSG_CONNECT) {
        errno = EINVAL;
        ERROR_MSG("Illegal message type! Should be MSG_CONNECT!\n");
    }

    synchronized(&clients_mutex, {
        if (check_username(client_fd, &msg) == -1) {
            return NULL;
        }

        int id;
        if ((id = create_client(client_fd, &msg)) == -1) {
            ERROR_MSG("The server is full... Aborting...\n");
        }

        if (connect_pending(id, &msg) == -1) {
            return NULL;
        }
    });

    msg.type = MSG_WAITING;
    write(client_fd, &msg, sizeof(struct message_t));
    return NULL;
}

int check_username(int client_fd, struct message_t* msg) {
    for (int i = 0; i < MAX_CLIENT_COUNT; i++) {
        if (clients[i].taken && !strncmp(msg->data.init.name, clients[i].name, CLIENT_NAME_LEN)) {
            write(client_fd, &(struct message_t){.type = MSG_NAME_TAKEN_ERROR},
                  sizeof(struct message_t));
            pthread_mutex_unlock(&clients_mutex);
            synchronized_printf(&print_mutex, "Duplicate name, rejecting connection...\n");
            return -1;
        }
    }
    return 0;
}

int create_client(int client_fd, struct message_t* msg) {
    for (int i = 0; i < MAX_CLIENT_COUNT; i++) {
        if (!clients[i].taken) {
            clients[i].taken = true;
            clients[i].playing = true;
            clients[i].socket_fd = client_fd;
            strncpy(clients[i].name, msg->data.init.name, CLIENT_NAME_LEN);
            synchronized_printf(&print_mutex, "Client got succesfully assigned id %d\n", i);
            return i;
        }
    }
    return -1;
}

int connect_pending(int id, struct message_t* msg) {
    for (int i = 0; i < MAX_CLIENT_COUNT; ++i) {
        if (clients[i].taken && !clients[i].playing) {
            clients[i].playing = true;
            pthread_mutex_unlock(&clients_mutex);
            game_connection(i, id);
            return -1;
        }
    }
    clients[id].playing = false;
    return 0;
}

void* pinger_routine(void* args) {
    int fds = epoll_create1(0);
    struct message_t msg;
    msg.type = MSG_PING;
    struct epoll_event epoll_event;
    while (true) {
        sleep(1);
        for (int i = 0; i < MAX_CLIENT_COUNT; ++i) {
            synchronized(&clients[i].mutex, {
                if (clients[i].taken) {
                    epoll_event.events = EPOLLIN;
                    epoll_event.data.fd = clients[i].socket_fd;
                    epoll_ctl(fds, EPOLL_CTL_ADD, clients[i].socket_fd, &epoll_event);
                    write(clients[i].socket_fd, &msg, sizeof(msg));
                    int available = epoll_wait(fds, &epoll_event, 1, TIMEOUT);
                    epoll_event.events = EPOLLIN;
                    epoll_event.data.fd = clients[i].socket_fd;
                    epoll_ctl(fds, EPOLL_CTL_DEL, clients[i].socket_fd, &(epoll_event));
                    if (available == -1) {
                        synchronized_printf(&print_mutex, "Client {id: %d, name: %s} got timed out... Disconnecting...", i, clients[i].name);
                        disconnect_client(i);
                    } else {
                        read(clients[i].socket_fd, &msg, sizeof(msg));
                    }
                }
            });
        }
    }
    return NULL;
}

void game_connection(int player1, int player2) {
    synchronized_printf(&print_mutex, "Players: %d and %d have started the game!\n", player1, player2);

    char board[BOARD_SIZE] = "         ";
    struct message_t msg;
    char winner;
    int round = 0;
    int players[2] = {player1, player2};

    send_msg_start(&msg, player1, player2);
    send_msg_update(&msg, player1, player2, board);

    while (!(winner = check_board(board))) {
        send_msg_move(&msg, players[round % 2]);
        if (msg.type == MSG_DISCONNECT) {
            synchronized(&clients[players[round % 2]].mutex, {
                disconnect_client(players[round % 2]);
            });
            send_msg_disconnect(&msg, players[(round + 1) % 2]);
            send_msg_waiting(&msg, players[(round + 1) % 2]);
            return;
        }
        if (msg.data.move < 1 || msg.data.move > 9 || board[msg.data.move - 1] != ' ') {
            continue;
        }
        board[msg.data.move - 1] = round % 2 ? TICK2 : TICK1;
        send_msg_update(&msg, player1, player2, board);
        round++;
    }

    send_msg_end(&msg, player1, player2, winner);
}

void send_msg_start(struct message_t* msg, int player1, int player2) {
    msg->type = MSG_START;

    msg->data.init.symbol = TICK1;
    strncpy(msg->data.init.name, clients[player2].name, CLIENT_NAME_LEN);
    write(clients[player1].socket_fd, msg, sizeof(struct message_t));

    msg->data.init.symbol = TICK2;
    strncpy(msg->data.init.name, clients[player1].name, CLIENT_NAME_LEN);
    write(clients[player2].socket_fd, msg, sizeof(struct message_t));
}

void send_msg_update(struct message_t* msg, int player1, int player2, char* board) {
    msg->type = MSG_UPDATE;
    strncpy(msg->data.board, board, BOARD_SIZE);
    write(clients[player1].socket_fd, msg, sizeof(struct message_t));
    write(clients[player2].socket_fd, msg, sizeof(struct message_t));
}

void send_msg_move(struct message_t* msg, int player) {
    msg->type = MSG_MOVE;
    synchronized(&clients[player].mutex, {
        write(clients[player].socket_fd, msg, sizeof(struct message_t));
        read(clients[player].socket_fd, msg, sizeof(struct message_t));
    })
}

void send_msg_disconnect(struct message_t* msg, int player) {
    msg->type = MSG_DISCONNECT;
    write(clients[player].socket_fd, msg, sizeof(struct message_t));
    disconnect_client(player);
}

void send_msg_waiting(struct message_t* msg, int player) {
    msg->type = MSG_WAITING;
    write(clients[player].socket_fd, msg, sizeof(struct message_t));
}

void send_msg_end(struct message_t* msg, int player1, int player2, char winner) {
    msg->type = MSG_END;
    msg->data.winner = winner;
    write(clients[player1].socket_fd, msg, sizeof(struct message_t));
    write(clients[player2].socket_fd, msg, sizeof(struct message_t));
    disconnect_client(player1);
    disconnect_client(player2);
}

char check_board(char* board) {
    char winner = 0;

    // check columns
    winner += check_line(board[0], board[3], board[6]);
    winner += check_line(board[1], board[4], board[7]);
    winner += check_line(board[2], board[5], board[8]);

    if (!winner) return winner;

    // check rows
    winner += check_line(board[0], board[1], board[2]);
    winner += check_line(board[3], board[4], board[5]);
    winner += check_line(board[6], board[7], board[8]);

    if (!winner) return winner;

    // check diagonals
    winner += check_line(board[0], board[4], board[8]);
    
    if (!winner) return winner;

    winner += check_line(board[2], board[4], board[6]);

    bool full = true;
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i] == ' ') {
            full = false;
            break;
        }
    }

    return full ? 'd' : winner;
}

char check_line(char field1, char field2, char field3) {
    return field1 != ' ' && field1 == field2 && field2 == field3 ? field1 : 0;
}

void disconnect_client(int id) {
    clients[id].taken = false;
    close(clients[id].socket_fd);
    synchronized_printf(&print_mutex, "Disconnected client = {id: %d, name: %s}\n", id, clients[id].name);
}
