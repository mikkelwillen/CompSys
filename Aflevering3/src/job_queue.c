#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "job_queue.h"

int job_queue_init(struct job_queue* job_queue, int capacity) {
  if (job_queue != NULL) {
    job_queue = malloc(sizeof(struct job_queue));
    job_queue->capacity = capacity;
    job_queue->current_jobs = 0;
    job_queue->job_stack = malloc(sizeof(struct job) * capacity);
    return 0;
  }
  return 1;
}


int job_queue_destroy(struct job_queue* job_queue) {
  if (job_queue != NULL) { 
    if (job_queue->current_jobs == 0) {
      free(job_queue->job_stack);
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
      new_job->data = data;
      job_queue->job_stack[job_queue->current_jobs] = new_job;
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
      data = job_queue->job_stack[job_queue->current_jobs - 1]->data;
      free(job_queue->job_stack[job_queue->current_jobs - 1]);
      job_queue->current_jobs--;
      return 0;
    } 
    return 1;
  } 
  return -1;
} 
