// network server
// implements routing and interface role

#include "error.h"

#include <stdlib.h>

#include "opt.h"
#include "state.h"
#include "network.h"
#include "job.h"
#include "time.h"

int main(int argc, char* argv[]) {
  srand(get_timestamp().i);

  state_init(parse_opts(argc, argv));
  net_init();
  job_queue_init();

  while (state.exiting == false) {
    job_interrupt();
  }
}
