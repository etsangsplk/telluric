#pragma once

#include "types.h"
#include "error.h"

typedef uint32_t pixel;
typedef uint32_t color;

struct grid_char {
  integral x;
  integral y;
  integral codepoints; // uint64 interpreted as uint8[8]
  color c;
};

struct grid_rect {
  rect pos;
  color c;
};

struct vec_line {};
struct vec_pixel {};
struct vec_bitmap {};

typedef struct {
  enum {
    grid_char,
    grid_rect,
    vec_line,
    vec_pixel,
    vec_bitmap
  } type;

  union {
    struct grid_char character;
    struct grid_rect rect;
    struct vec_line line;
    struct vec_pixel pixel;
    struct vec_bitmap bitmap;
  };
} drawable;

struct tabset_surface {
  tabset_ref target;
  rect position;
};

typedef struct {
  enum {
    tabset,
    panel,
    menu
  } type;
  union {
    struct tabset_surface tabset;
  };
} global_drawable;

typedef struct {
  pixel* buf; /// rgba8

  integral height;
  integral width;
} renderbuf;

error renderer_init();
void renderer_close();
void renderer_resize();

void renderer_newframe(fractional dt);

void draw_background();
void draw_global(global_drawable d);
