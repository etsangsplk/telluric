#include "job.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "util.h"
#include "state.h"
#include "protocol.h"

void job_read(job* j) {
  pthread_mutex_lock(&state.queue.readlock);

  if (wait_read(state.queue.fifo, 1)) {
    read(state.queue.fifo, j, sizeof(job));
  }

  pthread_mutex_unlock(&state.queue.readlock);
}

void neighbour_dispatch() {}
void client_dispatch() {}

void* job_thread(void* null) {
  job* j = NULL;

  while(state.queue.working) {
    // block if interrupted
    pthread_mutex_lock(&state.queue.interrupt);
    pthread_mutex_unlock(&state.queue.interrupt);

    job_read(j);

    if(j != NULL) {
      if (j->type == disconnect_event) {}
      else
        dispatch(j->packet.proto, j->origin, j->packet);
    }

    j = NULL;
  }

  return null;
}

error job_queue_init() {
  state.queue.working = true;

  state.queue.fifo = mkfifo("./jobs", S_IRUSR | S_IWUSR);

  pthread_mutex_init(&state.queue.readlock, NULL);
  pthread_mutex_init(&state.queue.interrupt, NULL);

  state.queue.threads = malloc(sizeof(pthread_t) * state.config.threads);
  state.queue.threads_count = state.config.threads;

  state.queue.scheduled_max = state.config.threads * 16;
  state.queue.scheduled =
    malloc(sizeof(scheduled_job) * state.queue.scheduled_max);
  state.queue.scheduled_count = 0;

  for (counter i = 0; i < state.queue.threads_count; i++) {
    pthread_create(&state.queue.threads[i], NULL, &job_thread, NULL);
  }

  return noerr;
}

void job_queue_close() {
  state.queue.working = false;

  for (counter i = 0; i < state.queue.threads_count; i++) {
    pthread_join(state.queue.threads[i], NULL);
  }

  pthread_mutex_destroy(&state.queue.readlock);
  pthread_mutex_destroy(&state.queue.interrupt);

  close(state.queue.fifo);
  unlink("./jobs");

  free(state.queue.threads);
  free(state.queue.scheduled);
}

void job_add(job j) {
  write(state.queue.fifo, &j, sizeof(job));
}

void job_interrupt() {
  // sleep until next interrupt period
  sleep(state.config.interrupt_delay);

  pthread_mutex_lock(&state.queue.interrupt);

  // todo dispatch to regular interrupt tasks

  struct timespec c;
  // clock_gettime(CLOCK_MONOTONIC, &c);

  for (counter i = 0; i < state.queue.scheduled_count; i++) {
    if (state.queue.scheduled[i].when == 0) continue;
    if (state.queue.scheduled[i].when > c.tv_sec) continue;

    write(state.queue.fifo, &state.queue.scheduled[i].j, sizeof(job));
    state.queue.scheduled[i].when = 0;
  }
}

// jobs are scheduled for arbitrary times, but only dispatched
// during the interrupt that follows their time proc
void job_schedule(job j, counter delay) {
  if (state.queue.scheduled_count >= state.queue.scheduled_max) return;

  struct timespec c;
  // clock_gettime(CLOCK_MONOTONIC, &c);

  time_t w = c.tv_sec + delay;

  for (counter i = 0; i < state.queue.scheduled_max; i++) {
    if (state.queue.scheduled[i].when != 0) continue;

    state.queue.scheduled[i].when = w;
    state.queue.scheduled[i].j = j;
    state.queue.scheduled_count += 1;
    break;
  }
}

