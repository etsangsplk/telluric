#include "opts.h"

#include "stdlib.h"
#include "stdio.h"

opts parse_opts(int argc, char* argv[]) {
  opts o;

  if (argc < 3) {
    fprintf(stderr, "not enough arguments");
    exit(EXIT_FAILURE);
  } else {
    o.in_file = argv[1];
    o.out_file = argv[2];
  }

  return o;
}
