#ifndef SERVER_H
#define SERVER_H

#define MAX_TEXT_LEN 140

typedef struct {
    unsigned short int type;
    unsigned short int orig_uid;
    unsigned short int dest_uid;
    unsigned short int text_len;
    unsigned char text[MAX_TEXT_LEN + 1];
} msg_t;

#define MSG_OI    0
#define MSG_TCHAU 1
#define MSG_TEXT  2

#endif
