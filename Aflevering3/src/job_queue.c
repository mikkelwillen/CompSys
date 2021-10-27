#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#include "job_queue.h"


int job_queue_init(struct job_queue* job_queue, int capacity) {
  job_queue->capacity = capacity;
  job_queue->current_jobs = 0;
  job_queue->destroyed = 0;
  assert(pthread_mutex_init(&job_queue->mutex, NULL) == 0);
  assert(pthread_cond_init(&job_queue->empty, NULL) == 0);
  assert(pthread_cond_init(&job_queue->fill, NULL) == 0);
  return 0;
}


int job_queue_destroy(struct job_queue* job_queue) {
  if (job_queue != NULL) {
    sleep(3);
    pthread_mutex_lock(&job_queue->mutex);
    while (job_queue->current_jobs != 0) {
      pthread_cond_wait(&job_queue->empty, &job_queue->mutex);
    }
    job_queue->destroyed = 1;
    pthread_cond_broadcast(&job_queue->fill);
    pthread_mutex_unlock(&job_queue->mutex);
    return 0;
  }
  return 1;
}

int job_queue_push(struct job_queue* job_queue, void* data) {
  if (job_queue != NULL) {
    pthread_mutex_lock(&job_queue->mutex);
    while (job_queue->current_jobs + 1 > job_queue->capacity) {
      pthread_cond_wait(&job_queue->empty, &job_queue->mutex);
    }
    struct job* new_job = malloc(sizeof(struct job));
    new_job->next = job_queue->head;
    new_job->data = data;
    job_queue->head = new_job;
    job_queue->current_jobs++;
    pthread_cond_signal(&job_queue->fill);
    pthread_mutex_unlock(&job_queue->mutex);
    return 0;
  }
  return 1;
}

int job_queue_pop(struct job_queue* job_queue, void** data) {
  if (job_queue != NULL) {
    pthread_mutex_lock(&job_queue->mutex);
    while (job_queue->current_jobs <= 0) {
      if (job_queue->destroyed) {
        pthread_mutex_unlock(&job_queue->mutex);
        return -1;
      }
      assert(pthread_cond_wait(&job_queue->fill, &job_queue->mutex) == 0);
    }
    *data = job_queue->head->data;
    struct job* current_head = job_queue->head;
    job_queue->head = job_queue->head->next;
    if (job_queue->current_jobs != 0) {
      job_queue->current_jobs--;
    }
    free(current_head);
    pthread_cond_signal(&job_queue->empty);
    pthread_mutex_unlock(&job_queue->mutex);
    return 0;
  } 
  return 1;
} 
