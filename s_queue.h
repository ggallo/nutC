// s_queue.h

#ifndef S_QUEUE_H
#define S_QUEUE_H

#include <string.h>
#include <stdlib.h>

typedef struct s_node {
  char* str;
  struct s_node* next;
} s_node;

typedef struct {
  s_node* head;
  int size;
} s_queue;



// Initialize a string queue
s_queue* initSQueue(void);

// Push a string onto the queue
void push(s_queue*, char*);

// Take the next item off the queue
char* pop(s_queue*);

// Get the length of the queue
int len(s_queue*);

// Delete the queue
void deleteSQueue(s_queue*);

// Helper function for deleting queues and popping
void deleteSNode(s_node*);

#endif
