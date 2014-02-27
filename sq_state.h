// sq_state.h

#ifndef SQ_STATE_H
#define SQ_STATE_H

#include <stdlib.h>
#include <pthread.h>
#include "s_queue.h"

typedef struct {
  int msqid;
  s_queue* local_queue;
  pthread_mutex_t lqueue_lock;
  pthread_cond_t message_waiting;
  int num_nuts;
  pthread_mutex_t num_lock;
} sq_state;

sq_state* initSQState(int);

void deleteSQState(sq_state*);

#endif
