#pragma once

#include "types.h"

typedef union {
  uint32_t i;
  struct {
    unsigned int subseconds : 6;
    unsigned int seconds    : 25;
    unsigned int yearity    : 1;
  };
} timestamp;

typedef struct {
  unsigned int subseconds : 6;
  unsigned int seconds    : 25;
} interval;

timestamp get_timestamp();

bool range_timestamp(timestamp t, interval delta);
