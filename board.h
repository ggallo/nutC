// board.h
#ifndef BOARD_H
#define BOARD_H

#include <stdlib.h>
#include <string.h>

typedef struct {
  int** grid;
  int num_acorns;
  int** client_pos;
  int height;
  int width;
  pthread_mutex_t board_lock;
} board;

//Initializes the board; places acorns
board* createBoard(int, int, int);

/* Initialize the squirrel on the board in a random location.
   Uses its msqid as an identifier */ 
void initSquirrel(board*, int);

//try to take a nut from space; return 0 on success, 1 else
int takeanut(board*, int, int);

/* Moves client with msqid <int> on <board*> in
   the direction indicated by <char*>. Returns
   the return value of takeanut() after passing
   in the client's new location to that function. */
int moveSquirrel(board*, int, char*);

// Print board at the server. Good for debugging.
void printBoard(board*);

// Deallocate board and all its parts.
void freeBoard(board*);

#endif
