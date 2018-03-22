#include "vm.h"

#include <sched.h>

#include "program.h"

void* app_thread(void* in) {
  application_state* a = in;

  while(a->closing == false) {
    pthread_mutex_lock(&a->stall);

    execute_main(&a->program, 32);

    // loop over input queue
    // execute_event

    pthread_mutex_unlock(&a->stall);
    sched_yield();
  }

  return NULL;
}

// spawn new in new slot
application_ref new_app(program p, window_ref w) {
  for (counter i = 0; i < state.config.apps_max; i++) {
    if (state.vm.apps[i].populated == false) {
      state.vm.apps[i].program = p;

      pthread_mutex_init(&state.vm.apps[i].stall, NULL);
      pthread_create(&state.vm.apps[i].thread, NULL, &app_thread, &state.vm.apps[i]);
      return i;
    }
  }

  return -1;
}

// pause, store current and spawn new in same slot
// only available in privileged context
void layer_app(program p, application_ref a) {
  pthread_mutex_lock(&state.vm.apps[a].stall);

  state.vm.shadows[a] = state.vm.apps[a];
  state.vm.apps[a].shadowed = true;
  state.vm.apps[a].program = p;

  pthread_mutex_init(&state.vm.apps[a].stall, NULL);
  pthread_create(&state.vm.apps[a].thread, NULL, &app_thread, &state.vm.apps[a]);
}

// replace with parent if layered, kill if not
void close_app(application_ref a) {
  state.vm.apps[a].closing = true;
  pthread_join(state.vm.apps[a].thread, NULL);

  pthread_mutex_destroy(&state.vm.apps[a].stall);
  free_program(state.vm.apps[a].program);

  if(state.vm.apps[a].shadowed) {
    state.vm.apps[a] = state.vm.shadows[a];
    pthread_mutex_unlock(&state.vm.apps[a].stall);
  } else {
    state.vm.apps[a].populated = false;
  }
}

////////////////////////////////////////////////////////////////////////////////
// global states

error vm_init() {
  state.vm.apps = malloc(sizeof(application_state) * state.config.apps_max);
  state.vm.shadows = malloc(sizeof(application_state) * state.config.apps_max);
  return noerr;
}
void vm_close() {
  free(state.vm.apps);
  free(state.vm.shadows);
}

////////////////////////////////////////////////////////////////////////////////
// extern

// swap renderlists, invoke next frame call
void vm_render(application_ref a) {}
