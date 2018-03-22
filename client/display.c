#include "display.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>
#include <wayland-client.h>

#include "util.h"

#include "state.h"
#include "vm.h"
#include "graphics.h"
#include "wayland.h"

////////////////////////////////////////////////////////////////////////////////
// windows

window_ref create_window(application_ref app) {
  state.display.windows_count += 1;
  window_ref w = state.display.windows_count - 1;

  // todo rework resize to work in place
  state.display.windows = resize_array(state.display.windows_count,
                                       sizeof(window_state),
                                       state.display.windows);

  state.display.windows[w].app = app;

  return w;
}

void delete_window(window_ref window) {
  delete_elem(state.display.windows_count,
              sizeof(window_state),
              state.display.windows, window);

  state.display.windows_count -= 1;
}

////////////////////////////////////////////////////////////////////////////////
// tabs

// temporary, fills as much space as possible
void resize_tabset(tabset_ref t) {
  rect d = {
    0, 0,
    state.display.size_x - (state.display.size_x % state.config.character_width),
    state.display.size_y - (state.display.size_y % state.config.character_height)
  };
  rect c = {
    0, 0,
    d.w / state.config.character_width,
    d.h / state.config.character_height,
  };

  state.display.tabsets[t].display_bounds = d;
  state.display.tabsets[t].chargrid_bounds = c;

  return;
}

tabset_ref create_tabset() {
  state.display.tabsets_count += 1;
  tabset_ref t = state.display.tabsets_count - 1;
  state.display.tabsets = resize_array(state.display.tabsets_count,
                                       sizeof(tabset_state),
                                       state.display.tabsets);

  resize_tabset(t);

  // create initial window
  state.display.tabsets[t].windows = alloc_array(1, sizeof(window_ref));
  state.display.tabsets[t].windows[0] = create_window(0);

  return t;
}

void delete_tabset(tabset_ref t) {

  // kill windows
  for (counter i = 0; i < state.display.tabsets[t].windows_count; i++) {
    delete_window(state.display.tabsets[t].windows[i]);
  }

  // remove from drawlist
  for (counter i = 0; i < state.display.renderer.drawlist_count; i++) {
    if (state.display.renderer.global_drawlist[i].type != tabset)
      continue;

    if (state.display.renderer.global_drawlist[i].tabset.target == t) {
      delete_elem(state.display.renderer.drawlist_count,
                  sizeof(global_drawable),
                  state.display.renderer.global_drawlist,
                  i);
      state.display.renderer.drawlist_count -= 1;
    }
  }
}

void move_window_tabset(window_ref window, tabset_ref from, tabset_ref to) {}

////////////////////////////////////////////////////////////////////////////////
// render loop

// temporary only renders tabsets
void update_global_drawlist(){
  state.display.renderer.global_drawlist =
    resize_array(state.display.tabsets_count,
                 sizeof(global_drawable),
                 state.display.renderer.global_drawlist);

  for (counter i = 0; i < state.display.tabsets_count; i++) {
  }
}

void display_frame(uint32_t time) {
  if (state.display.should_close) return;

  // draw_background();
  // update_global_drawlist();

  // for (counter i = 0; i < state.display.renderer.drawlist_count; i++) {
  //   draw_global(state.display.renderer.global_drawlist[i]);
  // }
}

void* display_thread(void* null) {

  wayland_init();
  renderer_init();

  wayland_loop_init(&display_frame);

  while (!state.display.should_close)
    wayland_loop_run();

  for (counter i = 0; i < state.display.tabsets_count; i++) {
    delete_tabset(i);
  }

  free(state.display.renderer.global_drawlist);

  renderer_close();
  wayland_close();

  return null;
}

////////////////////////////////////////////////////////////////////////////////
// initialization

error display_init() {
  //pthread_create(&state.display.thread, NULL, &display_thread, NULL);

  display_thread(NULL);

  return noerr;
}

void display_resize(int x, int y) {
  state.display.size_x = x;
  state.display.size_y = y;
}

void display_close() {
  state.display.should_close = true;

  pthread_join(state.display.thread, NULL);
}
