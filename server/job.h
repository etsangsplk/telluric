#pragma once

#include <time.h>
#include <pthread.h>

#include "types.h"
#include "error.h"
#include "opt.h"
#include "protocol.h"
#include "time.h"

typedef struct {
  enum {
    neighbour_packet,
    client_packet,
    disconnect_event
  } type;
  ipv6_address origin;
  message_packed packet;
} job;

typedef struct {
  time_t when;
  job j;
} scheduled_job;

error job_queue_init();
void job_queue_close();

void job_add(job j);
void job_schedule(job j, counter delay);

void job_interrupt();
