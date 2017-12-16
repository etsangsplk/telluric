#pragma once

#include "types.h"
#include "reduce.h"

typedef struct {
  byte id;
  byte flags;
  uint32_t length;
  byte* content;
} data_object;

data_object compile_data(byte id, data_def d);
