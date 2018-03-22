// client state centralization
// all the non-stack-scoped variables go here
// all of them

#pragma once

#include <pthread.h>
#include <wayland-client.h>
#include <sqlite3.h>

#include "types.h"
#include "options.h"
#include "graphics.h"

typedef counter store_id;

typedef struct {
  integral character_height;
  integral character_width;
  const char* font_spec;

  const char* store_path;
  counter apps_max;
} configuration;

typedef struct {} storage;

////////////////////////////////////////////////////////////////////////////////
// execution

typedef struct {
  counter bytestrings_count;
  byte* bytestring_masks;
} memory_info;
typedef struct {} program_memory;

typedef struct {
  counter entry_offset;
  counter limit_offset;
} event_span;

typedef struct {
  byte flags;
  event_span main;
  event_span close;
  event_span render;
  event_span resize;
  event_span network;
} section_indices;

typedef struct {
  byte* instr;
  event_span bounds;
  pthread_mutex_t interrupt;
  counter cycle_limit;
} trace;

// runtime program representation
typedef struct {
  byte* bytecode;
  section_indices sections;
  memory_info mem;

  byte** pool;
  // signature
  // jitted artifacts should go here
} program;

typedef struct {
  bool populated;
  program program;

  window_ref window;
  bool shadowed;

  pthread_mutex_t stall;
  bool closing;
} application_state;

typedef struct {
  // canonical list
  application_state* apps;
  application_state* shadows;

  pthread_t* threadpool;
  counter thread_count;
} global_vm_state;

////////////////////////////////////////////////////////////////////////////////
// display

typedef struct {
  window_ref window;

  drawable* drawlist;
  counter drawlist_count;
} window_renderer_state;

typedef struct {
  application_ref app;
  window_renderer_state render;
} window_state;

typedef struct {
  enum { shell, subsurface } type;

  struct wl_surface* surface;
  struct wl_subsurface* subsurface;
  struct wl_shell_surface* shell;
  struct wl_buffer* buffer;
  struct wl_callback* callback;

  uint32_t* data;
  integral size;
  bool busy;
  rect bounds;
} framebuffer;

typedef struct {
  tabset_ref tabset;

  framebuffer render;
} tabset_renderer_state;

typedef struct {
  rect display_bounds;
  rect chargrid_bounds;

  window_ref* windows;
  counter windows_count;

  window_ref active_window;
  tabset_renderer_state renderer;
} tabset_state;

typedef struct {
  struct wl_display* display;
  struct wl_registry* registry;
  struct wl_compositor* compositor;
  struct wl_shell* shell;
  struct wl_shm* shm;
  struct wl_subcompositor* subcompositor;
  struct wl_seat* seat;
} wayland_state;

typedef struct {
  global_drawable* global_drawlist;
  counter drawlist_count;

  framebuffer toplevel;
} renderer_state;

typedef struct {
  renderer_state renderer;
  wayland_state w;

  int max_x;
  int max_y;

  // canonical list
  tabset_state* tabsets;
  counter tabsets_count;

  // canonical list
  window_state* windows;
  counter windows_count;

  pthread_t thread;
  bool should_close;
} display_state;

////////////////////////////////////////////////////////////////////////////////
// audio

typedef struct {
  uint32_t playback_device;
  uint32_t record_device;
} audio_state;

////////////////////////////////////////////////////////////////////////////////
// input

typedef struct {
  bool drag_state;
  window_ref focused;
} input_state;

typedef struct {
  storage store;
  configuration config;
  display_state display;
  global_vm_state vm;

  sqlite3* db;

  bool should_render;
  bool should_exit;
} client_state;

// main state object is static
client_state state;

error state_init(opts opts);
