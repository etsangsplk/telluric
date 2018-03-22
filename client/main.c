// telluric client
// implements ui, vm, and interface negotiation role

#include "types.h"
#include "options.h"
#include "error.h"

#include "state.h"

#include "display.h"
#include "audio.h"
#include "input.h"
#include "vm.h"

// temporary
void test_state() {
  create_tabset();
}

int main(int argc, char* argv[]) {

  // parse invokation options
  opts o = parse_opts(argc, argv);

  // spawn subsystems
  error e;

  e = state_init(o);
  throw_error(e);

  e = vm_init();
  throw_error(e);

  e = display_init();
  throw_error(e);

  /* e = audio_init(); */
  /* throw_error(e); */

  e = input_init();
  throw_error(e);

  // execution loop
  while (state.should_exit == false) {
    input_poll();
  }

  // kill subsystems
  // vm_close();
  display_close();
  // audio_close();
  input_close();

  return 0;
}

