// utils.c

#include "utils.h"

int openQueue(key_t keyval) {
  int qid;
  
  if((qid = msgget(keyval, (IPC_CREAT | IPC_EXCL | 0666))) == -1) {
     perror("openQueue");
     return(-1);
  }
  
  return(qid);
}

msg* makeStartMsg(board* board, int server_msqid) {
  msg* start = initMessage();
  start->mtype = server_msqid;

  sprintf(start->mtext, "START\n%d %d\n", board->height, board->width);
  int i, j;
  for(i=0; i < board->height; i++) {
    for(j=0; j < board->width; j++) {
      if(board->grid[i][j]) {
	char temp[10];
	sprintf(temp, "%d %d %d\n", i, j, board->grid[i][j]);
	strcat(start->mtext, temp);
      }
    }
  }

  strcat(start->mtext, "\n");
  return start;
}

char** parseMessage(char* message, char* tokenize) {
  char** argList = (char**) malloc(MSG_MAX * sizeof(char*));

  //Tokenize the message and copy tokens into array
  int i = 0;
  char* text = strtok(message, tokenize);
  while(text) {
    argList[i++] = strdup(text);
    text = strtok(NULL, tokenize);
  }

  // Last element is NULL
  argList[i] = NULL;
  // Resize the argument list
  argList = (char**) realloc(argList, (i+1)*sizeof(char*));
  return argList;
}

void printBoardInfo(char** start_msg, int* b_dim) {

  
  char** dim = parseMessage(start_msg[1], " ");
  printf("The forrest is %s rows by %s columns\n", dim[0], dim[1]);
  b_dim[0] = atoi(dim[0]);
  b_dim[1] = atoi(dim[1]);
  printf("Position:\tNumber\n");
  int i = 2;
  while(start_msg[i]) {
    char** loc = parseMessage(start_msg[i++], " ");
    printf("(%s, %s):\t%s acorns\n", loc[0], loc[1], loc[2]);
    free(loc);
  }
  printf("\n");
}

int goodMove(char* move) {
  if(strcmp(move, "UP\n") && strcmp(move, "DOWN\n") && strcmp(move, "LEFT\n")\
     && strcmp(move, "RIGHT\n"))
    return 0;
  return 1;
}

int getStateIndex(int* client_loc, int msqid) {
  int i = 0;
  while(i < CLIENT_MIN && client_loc[i] != msqid)
    i++;
  if(i == CLIENT_MIN) return -1;
  return i;
}

void free2DChar(char** to_free) {
  int i = 0;
  while(to_free[i])
    free(to_free[i++]);
  free(to_free);
}
