#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "job_queue.h"

int job_queue_init(struct job_queue* job_queue, int capacity) {
  job_queue = malloc(sizeof(struct job_queue));
  job_queue->capacity = capacity;
  job_queue->current_jobs = 0;
  return 0;
}


int job_queue_destroy(struct job_queue* job_queue) {
  if (job_queue != NULL) { 
    if (job_queue->current_jobs == 0) {
      free(job_queue);
      return 0;
    }
    return -1;
  }
  return 1;
}

int job_queue_push(struct job_queue* job_queue, void* data) {
  if (job_queue != NULL) {
    if (job_queue->current_jobs < job_queue->capacity) {
      struct job* new_job = malloc(sizeof(struct job));
      new_job->next = job_queue->head;
      new_job->data = data;
      job_queue->head = new_job;
      job_queue->current_jobs++;
      return 0;
    }
    return -1;
  }
  return 1;
}

int job_queue_pop(struct job_queue* job_queue, void** data) {
  if (job_queue != NULL) {
    if (job_queue->current_jobs != 0) {
      data = job_queue->head->data;
      struct job* current_head = job_queue->head;
      job_queue->head = job_queue->head->next;
      job_queue->current_jobs--;
      free(current_head);
      return 0;
    }
    return 1;
  } 
  return -1;
} 
