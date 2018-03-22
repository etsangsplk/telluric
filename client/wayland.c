#include "wayland.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <wayland-client.h>

void registry_listener(void* data, struct wl_registry* r, uint32_t id, const char* iface, uint32_t version) {
  if (strcmp(iface, "wl_compositor") == 0)
    state.display.w.compositor = wl_registry_bind(r, id, &wl_compositor_interface, 1);
  if (strcmp(iface, "wl_shell") == 0)
    state.display.w.shell = wl_registry_bind(r, id, &wl_shell_interface, 1);
  if (strcmp(iface, "wl_shm") == 0)
    state.display.w.shm = wl_registry_bind(r, id, &wl_shm_interface, 1);
  if (strcmp(iface, "wl_subcompositor") == 0)
    state.display.w.subcompositor = wl_registry_bind(r, id, &wl_subcompositor_interface, 1);
  if (strcmp(iface, "wl_seat") == 0)
    state.display.w.seat = wl_registry_bind(r, id, &wl_seat_interface, 1);
}

void registry_remover(void* data, struct wl_registry* registry, uint32_t id) {
  printf("registry object removed\n");
}

static const struct wl_registry_listener listen = {registry_listener, registry_remover};

void shell_ping(void* data, struct wl_shell_surface* surf, uint32_t id) {
  wl_shell_surface_pong(surf, id);
}

void shell_configure(void* data, struct wl_shell_surface* surf, uint32_t edges, int32_t w, int32_t h) {
  if (w <= 0 || h <= 0) return; // nonsense resize

  // todo: resize subsurfs

  state.display.renderer.toplevel.bounds = (rect){0, 0, w, h};
}

void shell_popup_done(void* data, struct wl_shell_surface* surf) {}

static const struct wl_shell_surface_listener shell_listener = {
  shell_ping, shell_configure, shell_popup_done
};

// get nonfile for file-backed shm
file get_buf_fd(off_t size) {
  char* path = getenv("XDG_RUNTIME_DIR");
  char* tmpl = "/telluric-XXXXXX";
  char* loc = malloc(strlen(path) + strlen(tmpl));
  strcpy(loc, path);
  strcat(loc, tmpl);

  file fd = mkstemp(loc);
  fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC);
  unlink(loc);

  free(loc);

  ftruncate(fd, size);

  return fd;
}

void buffer_release(void* data, struct wl_buffer* buf) {
  framebuffer* buffer = data;

  buffer->busy = false;
}

static const struct wl_buffer_listener buffer_listener = { buffer_release };

framebuffer create_framebuffer(int width, int height) {
  framebuffer f;

  f.surface =
    wl_compositor_create_surface(state.display.w.compositor);

  int size = width * height * 4;

  file buf_fd = get_buf_fd(size);
  void* data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, buf_fd, 0);

  struct wl_shm_pool* p = wl_shm_create_pool(state.display.w.shm, buf_fd, size);

  f.buffer =
    wl_shm_pool_create_buffer(p,
                              0,
                              width,
                              height,
                              width * 4, // stride
                              WL_SHM_FORMAT_XRGB8888); // xrgb always available

  wl_shm_pool_destroy(p);
  close(buf_fd);

  memset(data, 0xff00ffff, size);

  wl_surface_attach(f.surface, f.buffer, width, height);

  return f;
}

framebuffer create_toplevel(int w, int h) {
  framebuffer f = create_framebuffer(w, h);

  f.shell = wl_shell_get_shell_surface(state.display.w.shell, f.surface);

  wl_shell_surface_add_listener(f.shell, &shell_listener, 0);
  wl_shell_surface_set_toplevel(f.shell);
  wl_shell_surface_set_title(f.shell, "telluric");

  f.type = shell;

  return f;
}

framebuffer wayland_create_subsurface(int w, int h, int x, int y, framebuffer parent) {
  framebuffer f = create_framebuffer(w, h);

  f.subsurface =
    wl_subcompositor_get_subsurface(state.display.w.subcompositor, f.surface, parent.surface);

  wl_subsurface_set_position(f.subsurface, x, y);

  f.type = subsurface;

  return f;
}

void free_buf(framebuffer f) {
  wl_buffer_destroy(f.buffer);

  if(f.subsurface)
    wl_subsurface_destroy(f.subsurface);
  if(f.shell)
    wl_shell_surface_destroy(f.shell);
  if(f.callback)
    wl_callback_destroy(f.callback);

  wl_surface_destroy(f.surface);

  munmap(f.data, f.size);
}

error wayland_init() {
  state.display.w.display = wl_display_connect(NULL);
  state.display.w.registry = wl_display_get_registry(state.display.w.display);
  wl_registry_add_listener(state.display.w.registry, &listen, NULL);

  wl_display_dispatch(state.display.w.display);
  wl_display_roundtrip(state.display.w.display);
  // wait for globals
  wl_display_roundtrip(state.display.w.display);

  state.display.max_x = 1024;
  state.display.max_y = 1024; // todo: set to wayland display bounds

  state.display.renderer.toplevel = create_toplevel(state.display.max_x, state.display.max_y);

  // initial present
  wl_surface_damage(state.display.renderer.toplevel.surface, 0, 0, state.display.max_x, state.display.max_y);
  wl_surface_commit(state.display.renderer.toplevel.surface);

  return noerr;
}

void wayland_subsurface_draw(framebuffer f) {
  wl_surface_damage(f.surface, 0, 0, f.bounds.w, f.bounds.h);
  wl_surface_commit(f.surface);
}

static const struct wl_callback_listener frame_listener;

void wayland_frame(void* fn, struct wl_callback* c, uint32_t time) {

  // draw renderlists immediately
  ((void(*)(uint32_t)) fn)(time);

  framebuffer* f = &state.display.renderer.toplevel;

  if (f->callback)
    wl_callback_destroy(f->callback);

  f->callback = wl_surface_frame(f->surface);
  wl_callback_add_listener(f->callback, &frame_listener, fn);

  wl_surface_damage(f->surface, 0, 0, f->bounds.w, f->bounds.h);
  wl_surface_commit(f->surface);
}

static const struct wl_callback_listener frame_listener = {&wayland_frame};

void wayland_loop_init(void (*fn)(uint32_t)) {
  // todo: does this need different callback/commit ordering like simple-shm?
  wayland_frame(fn, NULL, 0);
}

void wayland_loop_run() {
  wl_display_dispatch(state.display.w.display);
}

void wayland_close() {
  // presume other framebuffers have already been freed

  free_buf(state.display.renderer.toplevel);

  wl_shm_destroy(state.display.w.shm);
  wl_shell_destroy(state.display.w.shell);
  wl_compositor_destroy(state.display.w.compositor);

  wl_registry_destroy(state.display.w.registry);

  wl_display_flush(state.display.w.display);
  wl_display_disconnect(state.display.w.display);
}
