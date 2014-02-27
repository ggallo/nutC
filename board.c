// board.c

#include "board.h"
#include "globals.h"

board* createBoard(int height, int width, int num_acorns) {
  board* newBoard = (board*) malloc(sizeof(board));
  newBoard->grid = (int**) malloc(height * sizeof(int*));
  newBoard->client_pos = (int**) malloc(CLIENT_MAX * sizeof(int*));
  memset(newBoard->client_pos, 0, CLIENT_MAX);
  newBoard->num_acorns = num_acorns;
  newBoard->height = height;
  newBoard->width = width;

  //Create rows and columns
  int i;
  for(i=0; i < height; i++) {
    newBoard->grid[i] = (int*) malloc(width * sizeof(int));
    memset(newBoard->grid[i], 0, width);
  }

  //Place acorns randomly on the board
  int j;
  for(j=0; j < num_acorns; j++)
    newBoard->grid[rand() % height][rand() % width]++;
  
  return newBoard;
}

int takeanut(board* board, int row, int col) {
  // If a nut exists, take one!
  if(board->grid[row][col] > 0) {
    board->grid[row][col]--;
    board->num_acorns--;
    return 1;
  }
  // No nut in this space
  return 0;
}

void initSquirrel(board* board, int msqid) {

  // Place info in first available position in position store
  int i = 0;
  while(board->client_pos[i]) 
    i++;
  board->client_pos[i] = (int*) malloc(3 * sizeof(int));

  // Enter the client's msqid, row, and column
  board->client_pos[i][0] = msqid;
  board->client_pos[i][1] = rand() % board->height;
  board->client_pos[i][2] = rand() % board->width;
  
}

int moveSquirrel(board* board, int msqid, char* direction) {
  
  int** temp = board->client_pos;
  while(temp[0][0] != msqid)
    temp++;
  
  if(!strcmp(direction, "UP")) {
    if(temp[0][1] == 0) temp[0][1] = board->height - 1;
    else             temp[0][1]--;
  
  } else if(!strcmp(direction, "DOWN")) {
    if(temp[0][1] == board->height - 1) temp[0][1] = 0;
    else                             temp[0][1]++;
  
  } else if(!strcmp(direction, "LEFT")) {
    if(temp[0][2] == 0) temp[0][2] = board->width - 1;
    else             temp[0][2]--;
  
  } else if(!strcmp(direction, "RIGHT")) {
    if(temp[0][2] == board->width - 1) temp[0][2] = 0;
    else                            temp[0][2]++;

  } else {
    fprintf(stderr, "error: squirrel cant move %s\n", direction);
    return -1;
  }

  return takeanut(board, temp[0][1], temp[0][2]);
}

void printBoard(board* board) {
  int i, j;
  for(i=0; i<board->height; i++) {
    for(j=0; j<board->width; j++) {
      printf("%d ", board->grid[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

void freeBoard(board* board) {
  int i;
  for(i=0; i < board->height; i++)
    free(board->grid[i]);
  i = 0;
  while( board->client_pos[i])
    free(board->client_pos[i++]);
  free(board->grid);
  free(board->client_pos);
  free(board);
}

   
