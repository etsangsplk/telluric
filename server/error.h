#pragma once

#include "types.h"

typedef struct {
  byte code;
  const char* message;
} error;

static const error noerr = {0, ""};

void throw_error(error e);
