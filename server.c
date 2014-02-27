// server.c

#include "globals.h"

int main(int argc, char** argv) {

  if(argc != 4) {
    printf("server <height> <width> <number_acorns>\n");
    return -1;
  }

  //Seed the PRNGgdb
  srand(time(NULL));

  FILE* anchor = fopen(".server_anchor", "w");
  fclose(anchor);

  // Generate system key using anchor file
  key_t server_key = ftok(".server_anchor", 0);
  // Use system key to get a msq for the server
  // Clean it by deleting, then reopening
  int s_inbox = msgget(server_key, IPC_CREAT);
  // Clear the queue from past usage
  msgctl(s_inbox, IPC_RMID, NULL);
  // Reopen and it should be clean
  s_inbox = openQueue(server_key);
  // If server couldn't get an msq, exit
  if(s_inbox == -1) return -1;

  // Initialize global mutexes and conds
  pthread_mutex_init(&board_lock, NULL);
  pthread_cond_init(&game_running_cond, NULL);
  pthread_mutex_init(&game_running_lock, NULL);
  state_array = (sq_state*) malloc(CLIENT_MAX*sizeof(sq_state));
  pthread_mutex_init(&state_lock, NULL);
  pthread_mutex_init(&end_lock, NULL);

  pthread_t* client_threads = (pthread_t*) malloc(CLIENT_MAX*sizeof(pthread_t));
  client_loc = (int*) malloc(CLIENT_MAX*sizeof(int));
  int* throw_ints = (int*) malloc(CLIENT_MAX*sizeof(int));
  winner = -1;

  printf("Starting SQUEAK server...\n");

  // Initialize board; lock first!
  pthread_mutex_lock(&board_lock);
  the_board = createBoard(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
  
  int num_clients = 0;
  // Wait for enough clients to join
  while(num_clients != CLIENT_MIN) {
    msg* join_message = initMessage();
    msgrcv(s_inbox, join_message, MSG_MAX, 0, 0);
    client_loc[num_clients] = join_message->mtype;
    throw_ints[num_clients] = num_clients;

    // Only allow for JOIN messages
    if(!strcmp("JOIN", join_message->mtext)) {
      int made_thread = pthread_create(&client_threads[num_clients], NULL, clientThread, (void*)&throw_ints[num_clients]);
      // Upon successful thread creation
      if(made_thread == 0) {
	initSquirrel(the_board, join_message->mtype);
	state_array[num_clients++] = *initSQState(join_message->mtype);
      }
    }
    
    freeMessage(join_message);
  }
  // Unlock board
  pthread_mutex_unlock(&board_lock);

  printf("All %i clients joined. Starting game.\n", CLIENT_MIN);
  
  // Send the signal to start game
  pthread_mutex_lock(&game_running_lock);
  game_running = 1;
  pthread_cond_broadcast(&game_running_cond);
  pthread_mutex_unlock(&game_running_lock);
  
  // Determine if game running
  pthread_mutex_lock(&game_running_lock);
  int not_done = game_running;
  pthread_mutex_unlock(&game_running_lock);
  
  // Process messages until the game stops running
  while (not_done) {
    msg* new_message = initMessage();
    msgrcv(s_inbox, new_message, MSG_MAX, 0, 0);
    int cur_msqid = new_message->mtype;    
    int found = getStateIndex(client_loc, cur_msqid);
    
    char* msg_copy = strdup(new_message->mtext);
    char** if_new = parseMessage(msg_copy, " ");
    free(msg_copy);
    if(!strcmp(if_new[0], "JOIN")) {
      msg* reject = initMessage();
      strcpy(reject->mtext, "FULL");
      reject->mtype = s_inbox;
      msgsnd(cur_msqid, reject, MSG_MAX, 0);
      freeMessage(reject);
    }
    
    else {  
      // Place message on proper client's queue
      // Lock
      pthread_mutex_lock(&state_array[found].lqueue_lock);
      // Push
      push(state_array[found].local_queue, new_message->mtext);
      // Signal
      pthread_cond_signal(&state_array[found].message_waiting);
      // Unlock
      pthread_mutex_unlock(&state_array[found].lqueue_lock);
      
      freeMessage(new_message);
      
      pthread_mutex_lock(&game_running_lock);
      not_done = game_running;
      pthread_mutex_unlock(&game_running_lock);
    }
    free2DChar(if_new);
  }
  
  // Join all clients
  int z = 0;
  while(z != CLIENT_MIN)
    pthread_join(client_threads[z++], NULL);
  
  // Now destroy board, condition vars and mutexes
  freeBoard(the_board);
  pthread_mutex_destroy(&board_lock);
  int y = 0;
  while(y != CLIENT_MIN)
    deleteSQState(&state_array[y++]);
  free(state_array);
  pthread_mutex_destroy(&state_lock);
  pthread_cond_destroy(&game_running_cond);
  pthread_mutex_destroy(&game_running_lock);
  pthread_mutex_destroy(&end_lock);
  free(client_loc);

  msgctl(s_inbox, IPC_RMID, NULL);
  printf("Server exiting.\n");
  return 0;
}
