#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../server/server.h"

void send_message(int sock, unsigned short int orig_uid, unsigned short int dest_uid, const char *text);

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

    char text[141];
    unsigned short int dest_uid;

    while (1) {
        printf("Enter destination UID (0 for broadcast): ");
        scanf("%hu", &dest_uid);
        printf("Enter message: ");
        scanf(" %[^\n]", text);

        send_message(sock, uid, dest_uid, text);
    }

    msg.type = MSG_TCHAU;
    msg.orig_uid = uid;
    msg.dest_uid = 0;
    msg.text_len = 0;
    send(sock, &msg, sizeof(msg_t), 0);

    close(sock);

    return 0;
}

void send_message(int sock, unsigned short int orig_uid, unsigned short int dest_uid, const char *text) {
    msg_t msg;
    msg.type = MSG_TEXT;
    msg.orig_uid = orig_uid;
    msg.dest_uid = dest_uid;
    msg.text_len = strlen(text) + 1;
    strncpy((char *)msg.text, text, MAX_TEXT_LEN);

    if (send(sock, &msg, sizeof(msg_t), 0) < 0) {
        perror("send");
    }
}
