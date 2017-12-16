#pragma once

#include <stdint.h>

#include "types.h"

typedef struct {
  enum { integral, fractional, vector, matrix } type;
  union {
    int64_t i;
    double f;
    struct {
      double* values;
      int length;
    } m;
  };
} literal;

literal consume_lit(char* c, int* pos, int len);
bool optional_lit(literal* l, char* c, int len);
