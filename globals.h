// globals.h

#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <string.h>
#include "sq_state.h"
#include "board.h"
#include "message.h"
#include "utils.h"
#include "s_queue.h"
#include "thread_func.h"

// Max number of clients
#define CLIENT_MAX 50
// Minimum number of clients needed to start game
#define CLIENT_MIN 3

// The ith item in this array is the ith client to join's msqid
int* client_loc;
// Board struct to be shared by threads
board* the_board;
// Lock for the board
pthread_mutex_t board_lock;
// Array to store client states
sq_state* state_array;
// Lock for state_array
pthread_mutex_t state_lock;
// Will be set to the msqid of the winning client
int winner;
// This implementation will need a lock for this variable
pthread_mutex_t winner_lock;

int game_running;
// Condition variable on which the clients will wait
// before playing
pthread_cond_t game_running_cond;
// Lock for game_running
pthread_mutex_t game_running_lock;

// Locks a crit-sec in the client thread function
pthread_mutex_t end_lock;

#endif
