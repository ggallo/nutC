// message.c

#include "message.h"
#include "globals.h"

msg* initMessage(void) {
  msg* new_msg = (msg*) malloc(sizeof(msg));
  return new_msg;
}

void freeMessage(msg* message) {
  free(message);
}

