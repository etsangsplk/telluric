#include "font.h"

#include <string.h>

#include "util.h"

font read_font(char* file) {
  byte* f = NULL;
  read_file(file, f);

  // todo validate file length is at least 3

  font font;

  font.dimensions.narrow = f[0];
  font.dimensions.wide = f[0] * 2;
  font.dimensions.height = f[1];

  font.dimensions.size =
    (font.dimensions.wide / 8 + (font.dimensions.wide % 8 ? 1 : 0))
    * font.dimensions.height;

  font.codepoint_count = f[2];
  font.codepoint_map = malloc(f[2] * 3);
  font.glyphs = calloc(f[2], font.dimensions.size);

  byte* glyph = f + 3;
  size_t glyph_size = 2 + font.dimensions.size;

  // todo validate file length is 3 + f[2] * glyph_size

  for (counter i = 0; i < font.codepoint_count; i++) {
    font.codepoint_map[i].codepoint[0] = glyph[0];
    font.codepoint_map[i].codepoint[1] = glyph[1];
    font.codepoint_map[i].glyph = &font.glyphs[i * font.dimensions.size];
    memcpy(&font.glyphs[i * font.dimensions.size], &glyph[3], font.dimensions.size);

    glyph += glyph_size;
  }

  return font;
}

void free_font(font f) {
  free(f.codepoint_map);
  free(f.glyphs);
}

byte* codepoint(byte c[2], font f) {
  for (counter i = 0; i < f.codepoint_count; i++) {
    if (c[0] != f.codepoint_map[i].codepoint[0]) continue;
    if (c[1] != f.codepoint_map[i].codepoint[1]) continue;
    return f.codepoint_map[i].glyph;
  }

  return NULL;
}
