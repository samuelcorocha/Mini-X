#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../server/server.h"

void display_message(const msg_t *msg);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <UID> <Server IP> <Server Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int uid = atoi(argv[1]);
    char *server_ip = argv[2];
    int server_port = atoi(argv[3]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock);
        exit(EXIT_FAILURE);
    }

    msg_t msg;
    msg.type = MSG_OI;
    msg.orig_uid = uid;
    msg.dest_uid = 0;
    msg.text_len = 0;

    if (send(sock, &msg, sizeof(msg_t), 0) < 0) {
        perror("send");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (recv(sock, &msg, sizeof(msg_t), 0) <= 0) {
        perror("recv");
        close(sock);
        exit(EXIT_FAILURE);
    }

    while (1) {
        if (recv(sock, &msg, sizeof(msg_t), 0) <= 0) {
            perror("recv");
            break;
        }
        display_message(&msg);
    }

    close(sock);

    return 0;
}

void display_message(const msg_t *msg) {
    printf("Message from %d to %d: %s\n", msg->orig_uid, msg->dest_uid, msg->text);
}
