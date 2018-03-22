#include "graphics.h"

#include <math.h>
#include <stdio.h>

#include "state.h"
#include "vm.h"

renderbuf alloc_renderbuf(integral x, integral y) {
  return (renderbuf){ malloc(sizeof(uint32_t) * x * y), x, y };
}

void free_renderbuf(renderbuf b) {
  free(b.buf);
}

void clamp_rect(integral width, integral height, rect* r) {
  if (r->x + r->w > width || r->y + r->h > height) {
    // rescale to visible subset or 0
    r->w = width - r->x;
    r->w = r->w < 0 ? 0 : r->w;
    r->h = height - r->y;
    r->h = r->h < 0 ? 0 : r->w;
  }
}

// todo: rewrite in asm again
// alpha blend, mix alpha
void alpha_blend(uint32_t top, uint32_t* onto) {
	uint32_t Rf, Gf, Bf, Af;
	uint32_t Rb, Gb, Bb, Ab;
	uint32_t R, G, B, A;

  Af =  top & 0xff;
  Bf = (top >>  8) & 0xff;
  Gf = (top >> 16) & 0xff;
  Rf = (top >> 24) & 0xff;

  Ab =  *onto & 0xff;
  Bb = (*onto >>  8) & 0xff;
  Gb = (*onto >> 16) & 0xff;
  Rb = (*onto >> 24) & 0xff;

  R = (Rf * Af)/256 + Rb;
  G = (Gf * Af)/256 + Gb;
  B = (Bf * Af)/256 + Bb;
  A = Af + Ab;

  if (R > 255) R = 255;
  if (G > 255) G = 255;
  if (B > 255) B = 255;
  if (A > 255) A = 255;

  *onto = R << 24 | G << 16 | B << 8 | A;
}

// alpha blend, drop alpha, 24bit result in low position
void alpha_blit(uint32_t top, uint32_t* onto) {
	uint32_t Rf, Gf, Bf, Af;
	uint32_t Rb, Gb, Bb;
	uint32_t R, G, B;

  Af =  top & 0xff;
  Bf = (top >>  8) & 0xff;
  Gf = (top >> 16) & 0xff;
  Rf = (top >> 24) & 0xff;

  Bb = (*onto >>  8) & 0xff;
  Gb = (*onto >> 16) & 0xff;
  Rb = (*onto >> 24) & 0xff;

  R = (Rf * Af)/256 + Rb;
  G = (Gf * Af)/256 + Gb;
  B = (Bf * Af)/256 + Bb;

  if (R > 255) R = 255;
  if (G > 255) G = 255;
  if (B > 255) B = 255;

  *onto = R << 16 | G << 8 | B;
}

void composite(renderbuf src, renderbuf dest, rect bounds) {

  clamp_rect(dest.width, dest.height, &bounds);

  // composite
  if (bounds.w == src.width && bounds.h == src.height) {
    for (counter iy = 0; iy < bounds.h; iy++) {
      for (counter ix = 0; ix < bounds.w; ix++) {
        alpha_blend(src.buf[ix * iy],
                    &dest.buf[(bounds.x + ix) * (bounds.y + iy)]);
      }
    }
  } else {
    // ??? nearest neighbour scale if necessary
  }

  return;
}

// fill rect (screen coords)
void draw_rect(renderbuf b, rect r, color c) {
  clamp_rect(b.width, b.height, &r);

  for (counter j = 0; j < r.h; j++) {
    for (counter i = 0; i < r.w; i++) {
      alpha_blend(c, &b.buf[(i + r.x) * (j + r.y)]);
    }
  }

  return;
}

////////////////////////////////////////////////////////////////////////////////
// window drawables

// fill rect (chargrid coords)
void draw_grid_rect(tabset_ref t, renderbuf b, struct grid_rect r) {
  rect* bounds = &state.display.tabsets[t].chargrid_bounds;

  clamp_rect(bounds->w, bounds->h, &r.pos);
}

void draw_window_elem(tabset_ref t, renderbuf b, drawable d) {
  switch (d.type) {
  case grid_char:
    break;
  case grid_rect:
    draw_grid_rect(t, b, d.rect);
    break;
  case vec_line:
    break;
  case vec_pixel:
    break;
  case vec_bitmap:
    break;
  }

  return;
}

////////////////////////////////////////////////////////////////////////////////
// global drawables

void draw_background() {
  for (integral x = 0; x < state.display.size_x; x++) {
    for (integral y = 0; y < state.display.size_y; x++) {
      state.display.renderer.back.data[(y * state.display.max_x) + x] = 0xff00ffff;
    }
  }
}

void draw_tabset_background() {}

void draw_tabset(struct tabset_surface t) {
  window_ref w = state.display.tabsets[t.target].active_window;

  // swap drawlist
  drawable* d = state.display.windows[w].render.drawlist;
  counter l = state.display.windows[w].render.drawlist_count;

  state.display.windows[w].render.drawlist = NULL;
  state.display.windows[w].render.drawlist_count = 0;

  vm_invoke_render(w);

  for (counter i = 0; i < l; i++) {
    draw_window_elem(t.target,
                     state.display.tabsets[t.target].renderer.windowbuf,
                     d[i]);
  }

  free(d);
  return;
}

void draw_global(global_drawable d) {
  switch (d.type) {
  case tabset:
    draw_tabset(d.tabset);
    break;
  case panel:
    break;
  case menu:
    break;
  }
}

error renderer_init() {
  return noerr;
}

void renderer_close() {
}
