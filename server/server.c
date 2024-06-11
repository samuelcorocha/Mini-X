#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include "server.h"

#define PORT 12345
#define MAX_CLIENTS 20

fd_set master_fds, read_fds;
int fd_max;

void handle_client_message(int client_sock);
void handle_new_connection(int listener_sock);
void send_periodic_message(int signum);

int main() {
    int listener_sock;
    struct sockaddr_in server_addr;
    struct itimerval timer;
    struct sigaction sa;

    FD_ZERO(&master_fds);
    FD_ZERO(&read_fds);

    listener_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listener_sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(listener_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(listener_sock);
        exit(EXIT_FAILURE);
    }

    if (listen(listener_sock, 10) < 0) {
        perror("listen");
        close(listener_sock);
        exit(EXIT_FAILURE);
    }

    FD_SET(listener_sock, &master_fds);
    fd_max = listener_sock;

    // Setup the timer
    timer.it_value.tv_sec = 60;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 60;
    timer.it_interval.tv_usec = 0;

    // Setup signal handler for the timer
    sa.sa_handler = send_periodic_message;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
        perror("setitimer");
        exit(EXIT_FAILURE);
    }

    while (1) {
        read_fds = master_fds;

        if (select(fd_max + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i <= fd_max; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == listener_sock) {
                    handle_new_connection(listener_sock);
                } else {
                    handle_client_message(i);
                }
            }
        }
    }

    return 0;
}

void handle_new_connection(int listener_sock) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int new_sock = accept(listener_sock, (struct sockaddr *)&client_addr, &addr_len);

    if (new_sock < 0) {
        perror("accept");
        return;
    }

    FD_SET(new_sock, &master_fds);
    if (new_sock > fd_max) {
        fd_max = new_sock;
    }

    printf("New connection from %s on socket %d\n", inet_ntoa(client_addr.sin_addr), new_sock);
}

void handle_client_message(int client_sock) {
    msg_t msg;
    int nbytes = recv(client_sock, &msg, sizeof(msg_t), 0);

    if (nbytes <= 0) {
        if (nbytes == 0) {
            printf("Socket %d hung up\n", client_sock);
        } else {
            perror("recv");
        }
        close(client_sock);
        FD_CLR(client_sock, &master_fds);
    } else {
        switch (msg.type) {
            case MSG_OI:
                printf("Client %d connected\n", msg.orig_uid);
                send(client_sock, &msg, sizeof(msg_t), 0);
                break;
            case MSG_TCHAU:
                printf("Client %d disconnected\n", msg.orig_uid);
                close(client_sock);
                FD_CLR(client_sock, &master_fds);
                break;
            case MSG_TEXT:
                printf("Message from %d to %d: %s\n", msg.orig_uid, msg.dest_uid, msg.text);
                // Here you would handle message routing to clients
                break;
        }
    }
}

void send_periodic_message(int signum) {
    // Handle sending periodic messages to clients
    printf("Sending periodic message to clients...\n");
    // Implementation here
}
