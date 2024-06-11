CC = gcc
CFLAGS = -Wall -g

SERVER_DIR = server
CLIENT_SEND_DIR = client_send
CLIENT_RECEIVE_DIR = client_receive

SERVER_SRC = $(SERVER_DIR)/server.c
CLIENT_SEND_SRC = $(CLIENT_SEND_DIR)/client_send.c
CLIENT_RECEIVE_SRC = $(CLIENT_RECEIVE_DIR)/client_receive.c

SERVER_BIN = $(SERVER_DIR)/server
CLIENT_SEND_BIN = $(CLIENT_SEND_DIR)/client_send
CLIENT_RECEIVE_BIN = $(CLIENT_RECEIVE_DIR)/client_receive

all: $(SERVER_BIN) $(CLIENT_SEND_BIN) $(CLIENT_RECEIVE_BIN)

$(SERVER_BIN): $(SERVER_SRC) $(SERVER_DIR)/server.h
	$(CC) $(CFLAGS) -o $@ $<

$(CLIENT_SEND_BIN): $(CLIENT_SEND_SRC) $(SERVER_DIR)/server.h
	$(CC) $(CFLAGS) -o $@ $<

$(CLIENT_RECEIVE_BIN): $(CLIENT_RECEIVE_SRC) $(SERVER_DIR)/server.h
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(SERVER_BIN) $(CLIENT_SEND_BIN) $(CLIENT_RECEIVE_BIN)