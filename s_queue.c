// s_queue.c


#include "s_queue.h"

s_queue* initSQueue(void) {
  s_queue* new_queue = (s_queue*) malloc(sizeof(s_queue));
  new_queue->head = NULL;
  new_queue->size = 0;
  return new_queue;
}

void push(s_queue* queue, char* astr) {
  s_node* new_node = (s_node*) malloc(sizeof(s_node));
  new_node->str = strdup(astr);

  new_node->next = queue->head;
  queue->head = new_node;

  queue->size++;
}

char* pop(s_queue* queue) {
  if(queue->size == 0) 
    return NULL;

  char* to_return = NULL;
  s_node* temp = queue->head;
  s_node* to_delete = NULL;
  while(temp->next && temp->next->next)
    temp = temp->next;

  if(!temp->next) {
    to_return = strdup(temp->str);
    deleteSNode(temp);
    queue->head = NULL;

  } else {
    to_delete = temp->next;
    temp->next = NULL;
    to_return = strdup(to_delete->str);
    deleteSNode(to_delete);
  }

  queue->size--;
  return to_return;
}

int len(s_queue* queue) {
  return queue->size;
}

void deleteSQueue(s_queue* queue) {
  s_node* temp = queue->head;
  while(temp) {
    s_node* to_delete = temp;
    temp = temp->next;
    deleteSNode(to_delete);
  }
  queue = NULL;
}

void deleteSNode(s_node* node) {
  free(node->str);
  node = NULL;
}


  
