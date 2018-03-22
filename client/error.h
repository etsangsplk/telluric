#pragma once

#include "types.h"

// error reporting primitive
// will be substantially expanded in the future
typedef struct {
  bool err;
  char* description;
} error;

#define noerr (error){false, NULL}

void throw_error(error e);
