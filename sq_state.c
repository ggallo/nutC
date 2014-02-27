// sq_state.c

#include "sq_state.h"

sq_state* initSQState(int msqid) {
  sq_state* new_sq = (sq_state*) malloc(sizeof(sq_state));
  new_sq->msqid = msqid;
  new_sq->local_queue = initSQueue();
  pthread_mutex_init(&new_sq->lqueue_lock, NULL);
  pthread_cond_init(&new_sq->message_waiting, NULL);
  pthread_mutex_init(&new_sq->num_lock, NULL);
  new_sq->num_nuts = 0;
  return new_sq;
}

void deleteSQState(sq_state* sq) {
  deleteSQueue(sq->local_queue);
  pthread_mutex_destroy(&sq->lqueue_lock);
  pthread_cond_destroy(&sq->message_waiting);
  pthread_mutex_destroy(&sq->num_lock);
  sq = NULL;
}


