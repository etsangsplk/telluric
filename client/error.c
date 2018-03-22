#include "error.h"

#include <stdio.h>

void throw_error(error e) {
  if (e.err == true) {
    fprintf(stderr, "%s\n", e.description);
    exit(1);
  } else { return; }
};
