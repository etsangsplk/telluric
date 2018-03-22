#include "opt.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void help_text() {
  printf("telluric server\n");

  printf("\nserver:\n");
  printf("\t-db         [path]  db location\n");
  printf("\t-max        [int]   upper bandwidth target in kilobytes/sec\n");
  printf("\t-threads    [int]   thread pool size\n");

  printf("\ntraining speeds:\n");
  printf("\t-rotate     [float] rate route embeddings are displaced\n");
  printf("\t-cool       [float] embedding dissipation factor\n");
  printf("\t-associate  [float] threshold for hard association\n");

  printf("\nlimits:\n");
  printf("\t-clients    [int]   interface client limit\n");
  printf("\t-neighbours [int]   internode neighbour limit\n");
  printf("\t-sessions   [int]   global session limit\n");
  printf("\t-pending    [int]   pending connection limit\n");

  exit(0);
}

static const opts defaults = {
  .max_bandwidth = 2 * 1024 * 1024,
  .association_threshold = 0.05f,
  .cooling_factor = 0.9f,
  .rotator_speed = 0.05f,
  .threads = 8,
  .interrupt_delay = 5,
  .state_file = "./db",
  .neighbour_limit = 32,
  .client_limit = 128,
  .session_limit = 1024,
  .pending_limit = 8
};

opts parse_opts(int argc, char** argv) {
  opts o = defaults;

  if (argc > 1) {
    counter i = 1;

    while (i < argc) {
      if (i + 1 > argc || strncmp("-", argv[i+1], 1) == 0) {
        printf("missing argument for %s, ignoring\n", argv[i]);
        i += 1;
        continue;
      }

      if (strcmp("-db", argv[i]) == 0) {
        o.state_file = argv[i+1];
      } else if (strcmp("-max", argv[i]) == 0) {
        o.max_bandwidth = atoi(argv[i+1]);
      } else if (strcmp("-threads", argv[i]) == 0) {
        o.threads = atoi(argv[i+1]);
      } else if (strcmp("-rotate", argv[i]) == 0) {
        o.rotator_speed = strtod(argv[i+1], NULL);
      } else if (strcmp("-cool", argv[i]) == 0) {
        o.cooling_factor = strtod(argv[i+1], NULL);
      } else if (strcmp("-associate", argv[i]) == 0) {
        o.association_threshold = strtod(argv[i+1], NULL);
      } else if (strcmp("-clients", argv[i]) == 0) {
        o.client_limit = atoi(argv[i+1]);
      } else if (strcmp("-neighbours", argv[i]) == 0) {
        o.neighbour_limit = atoi(argv[i+1]);
      } else if (strcmp("-sessions", argv[i]) == 0) {
        o.session_limit = atoi(argv[i+1]);
      } else if (strcmp("-pending", argv[i]) == 0) {
        o.pending_limit = atoi(argv[i+1]);
      }

      i += 2;
    }
  }

  return o;
}
