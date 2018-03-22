#pragma once

#include "types.h"
#include "error.h"
#include "state.h"

framebuffer wayland_create_subsurface(int w, int h, int x, int y, framebuffer parent);

error wayland_init();
void wayland_loop_init(void (*fn)(uint32_t));
void wayland_loop_run();
void wayland_close();
