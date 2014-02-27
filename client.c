// client.c

#include "globals.h"

int main(int argc, char** argv) {

  if(argc != 2) {
    printf("client <player_id>\nThe player ID should be an integer > 0\n");
    return -1;
  }

  key_t client_key = ftok(".server_anchor", atoi(argv[1]));
  key_t server_key = ftok(".server_anchor", 0);
  
  long s_inbox = msgget(server_key, IPC_CREAT);
  long c_inbox = msgget(client_key, IPC_CREAT);
  int b_dim[2];
  msgctl(c_inbox, IPC_RMID, NULL);
  c_inbox = openQueue(client_key);

  if (c_inbox == -1) return -1;
  if (s_inbox == -1) {
    perror("client couldn't get server's mailbox");
    return -1;
  }

  msg* join_msg = initMessage();
  join_msg->mtype = c_inbox;
  strcpy(join_msg->mtext, "JOIN");
  msgsnd(s_inbox, join_msg, MSG_MAX, 0);
  freeMessage(join_msg);

  printf("Waiting on other squirrels...\n");
  msg* start_msg = initMessage();
  msgrcv(c_inbox, start_msg, MSG_MAX, 0, 0);
  char** start_args = parseMessage(start_msg->mtext, "\n");
  freeMessage(start_msg);

  if(!strcmp(start_args[0], "START")) {
    //printf("\033[0;0H\n");
    printf("Welcome! It's time to gather for the winter.\n");
    printBoardInfo(start_args, b_dim);
    free(start_args);

  } else if(!strcmp(start_args[0], "FULL")) {
    printf("Server is full. Try again later.\n");
    return -1;


  } else {
    fprintf(stderr, "start_msg: server is misbehaving; didn't send START\n");
    free(start_args);
    return -1;
  }
  
  int num_nuts = 0, still_move = 1;
  printf("Move UP, DOWN, LEFT, or RIGHT.\n");
  int over = 0;
  while(!over) {
   
    if(still_move) {
      printf("Move: ");
      char* new_move = (char*) malloc(MSG_MAX * sizeof(char));
      
      // Only process good uder inputs
      if(fgets(new_move, MSG_MAX, stdin) != NULL && goodMove(new_move)) {
	msg* send_move = initMessage();
	send_move->mtype = c_inbox;
	strcpy(send_move->mtext, "MOVE ");
	strcat(send_move->mtext, new_move);
	msgsnd(s_inbox, send_move, MSG_MAX, 0);
	freeMessage(send_move);
	
	// Display the prompt until the user enters a valid move
      } else 
	continue;
      
      free(new_move);
    }

    // Pull a message from the queue
    msg* response = initMessage();
    msgrcv(c_inbox, response, MSG_MAX, 0, 0);
    
    char** response_args = parseMessage(response->mtext, " \n");
    freeMessage(response);
    
    // Time to handle cases
    // Over message
    if(!strcmp(response_args[0], "OVER")) {
      over++;
      printf("Last acorn found! Your final total is %d acorns.\n", num_nuts);
      // You're the winner!
      if(!strcmp(response_args[1], "1"))
	printf("Congrats! With %d acorns, you will be hibernating like a king.\n", num_nuts);
      // You lost
      else if(!strcmp(response_args[1], "0"))
	printf("With a meager %d acorns, it will be a long winter...\n", num_nuts);
      // Idk what just happened
      else
	printf("Bad server response. Expected 1 or 0 but got %s\n", response_args[1]);
      
    } 
    
    // Good move message
    else if(!strcmp(response_args[0], "ACKM")) {

      // Found a nut
      if(!strcmp(response_args[1], "1"))
	printf("You found one!\ttotal: %d\n", ++num_nuts);
      // No nut here
      else if(!strcmp(response_args[1], "0"))
	printf("No acorn here...\ttotal: %d\n", num_nuts);
      // Idk what happened
      else {
	fprintf(stderr, "Bad server response. Expected 1 or 0 but got %s\n", response_args[1]);
	return -1;
      }

      /* The thread will send a second char with the ACKM message indicating
	 if the client just took the last nut. This way, the user need not
	 enter another move before seeing that the game is over. */
      if(!strcmp(response_args[2], "1"))
	still_move = 0;

    }
    
    // Bad message
    else 
      fprintf(stderr, "Bad server response. Expected ACKM but got %s\n", response_args[0]);
    
    free2DChar(response_args);

  }
  
  msgctl(c_inbox, IPC_RMID, NULL);
  printf("Thanks for playing!\n");
  return 0;
}
