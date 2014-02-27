// message.h

#ifndef MESSAGE_H
#define MESSAGE_H

#include <string.h>
#include <stdlib.h>

//Maximum message length
#define MSG_MAX 500

typedef struct {
  long mtype;
  char mtext[500];
} msg;

msg* initMessage(void);

void freeMessage(msg*);

#endif
