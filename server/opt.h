#pragma once

// server configuration

#include "types.h"

typedef struct {
  int max_bandwidth; // bytes per second

  double association_threshold; // vector distance for creating hard associations
  double cooling_factor;
  double rotator_speed;

  counter threads; // job pool size
  counter interrupt_delay;

  char* state_file; // db location

  counter neighbour_limit;
  counter client_limit;
  counter session_limit;
  counter pending_limit;
} opts;

opts parse_opts(int argc, char** argv);
