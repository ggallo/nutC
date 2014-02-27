// utils.h

#ifndef UTILS_H
#define UTILS_H

#include "globals.h"

//A wrapper for opening message queues. NOT ORIGINAL CODE. Taken from:
//http://www.tldp.org/LDP/lpg/node34.html
int openQueue(key_t);

// Create the START message that will be sent to
// clients at the start of the game.
msg* makeStartMsg(board*, int);

// Tokenizes first char array by the chars in the
// second array. Returns a null-terminated char**.
char** parseMessage(char*, char*);

void printBoardInfo(char**, int*);

int goodMove(char*);

int getStateIndex(int*, int);

void free2DChar(char**);

#endif
