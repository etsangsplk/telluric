// canonical parser for telluric bitmap font format

#pragma once

#include "types.h"

// telluric fonts are simple arrays of bit vectors
// they support the utf8 bmp, and are indexed by byte pairs
// narrow characters (latin) are constrained to be exactly half the width of wide characters (cjk)
// wide characters must have an even-numbered width

typedef struct {
  byte codepoint[2];
  byte* glyph;
} codemap;

typedef struct {
  struct {
    integral wide;          // wide-glyph width, in pixels
    integral narrow;        // narrow-glyph width, in pixels
    integral height;        // glyph height, in pixels

    size_t size;            // size of glyph, in bytes
  } dimensions;

  codemap* codepoint_map;      // array of (codepoint[0], codepoint[1], glyph offset)
  counter codepoint_count;  // size of codepoint array / 3
  byte* glyphs;             // bitmap
} font;

typedef struct {
  byte narrow_width;
  byte height;
  byte glyph_count;
  struct {
    byte codepoint[2];
    byte* glyph;
  } glyph;
} font_file;

////////////////////////////////////////////////////////////////////////////////

font read_font(char* file);
void free_font(font f);

// get pointer to glyph
byte* codepoint(byte c[2], font f);
