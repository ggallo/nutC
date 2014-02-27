// thread_func.c

#include "thread_func.h"

void* clientThread(void* state_pos) {
  int state_index = *((int*)state_pos);
  int my_msqid = client_loc[state_index];

  pthread_mutex_lock(&game_running_lock);
  while(game_running == 0)
    pthread_cond_wait(&game_running_cond, &game_running_lock);
  pthread_mutex_unlock(&game_running_lock);

  pthread_mutex_lock(&board_lock);
  msg* start = makeStartMsg(the_board, my_msqid);
  pthread_mutex_unlock(&board_lock);
  msgsnd(my_msqid, start, MSG_MAX, 0);
  freeMessage(start);
  
  pthread_mutex_lock(&game_running_lock);
  int not_done = game_running;
  pthread_mutex_unlock(&game_running_lock);

  while(not_done) {
    int got_nut = 0;
    int no_nuts = 0;
    
    pthread_mutex_lock(&state_array[state_index].lqueue_lock);
    while(len(state_array[state_index].local_queue) == 0)
      pthread_cond_wait(&state_array[state_index].message_waiting, &state_array[state_index].lqueue_lock);
    char* new_message = pop(state_array[state_index].local_queue);
    pthread_mutex_unlock(&state_array[state_index].lqueue_lock);
    
    // Read and process message from queue
    // Use locks where needed!
    char** message_text = parseMessage(new_message, " \n");
    if(!strcmp("MOVE", message_text[0])) {
      pthread_mutex_lock(&board_lock);
      got_nut = moveSquirrel(the_board, my_msqid, message_text[1]);
      no_nuts = (the_board->num_acorns == 0);
      pthread_mutex_lock(&state_array[state_index].num_lock);
      state_array[state_index].num_nuts += got_nut;
      pthread_mutex_unlock(&state_array[state_index].num_lock);
      pthread_mutex_unlock(&board_lock);
    }
    
    // Send response to client
    char* response = (char*) malloc(MSG_MAX*sizeof(char));
    sprintf(response, "ACKM %d %d", got_nut, no_nuts);
    msg* msg_response = initMessage();
    msg_response->mtype = 1;
    strcpy(msg_response->mtext, response);
    msgsnd(my_msqid, msg_response, MSG_MAX, 0);
    freeMessage(msg_response);

    // Game is over; last nut taken
    // This is a crit-sec. Only one thread should process this segment

    pthread_mutex_lock(&game_running_lock);
    int should_check = game_running;
    pthread_mutex_unlock(&game_running_lock);

    /* Game is over; last nut taken
       This is a crit-sec. Only one thread should process this segment
       Two clients could come to find the game should be over.
       So, a thread will also check if the game is still running.
       If not, some other thread has already turned the game off. */
    pthread_mutex_lock(&end_lock);
    if(no_nuts && should_check) {

      // Turn the game "off"
      pthread_mutex_lock(&game_running_lock);
      game_running = 0;
      pthread_mutex_unlock(&game_running_lock);

      // Set the winner variable to the appropriate msqid
      int j = 0;
      int best_total = 0;
      while(j != CLIENT_MIN) {
	pthread_mutex_lock(&state_array[j].num_lock);
	if(best_total < state_array[j].num_nuts) {
	  best_total = state_array[j].num_nuts;
	  winner = state_array[j].msqid;
	}
	pthread_mutex_unlock(&state_array[j].num_lock);
	j++;
      }

    }
    pthread_mutex_unlock(&end_lock);

    pthread_mutex_lock(&game_running_lock);
    not_done = game_running;
    pthread_mutex_unlock(&game_running_lock);
  }

  pthread_mutex_lock(&winner_lock);
  int i_won = (winner == my_msqid);
  pthread_mutex_unlock(&winner_lock);

  // Send an over message to the client
  msg* end_message = initMessage();
  end_message->mtype = 1;
  sprintf(end_message->mtext, "OVER %d", i_won);
  msgsnd(my_msqid, end_message, MSG_MAX, 0);
  freeMessage(end_message);

  return NULL;
}
