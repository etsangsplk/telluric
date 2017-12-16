#include "data.h"

#include <string.h>
#include <stdio.h>

#include "literal.h"

byte set_data_flags(data_def d) {
  byte o = 0;

  for (int i = 0; i < d.tags_count; i++) {
    if (strcmp(d.tags[i], "shared") == 0)
      o |= 0b00000000;
    if (strcmp(d.tags[i], "bitmap") == 0)
      o |= 0b00000000;
    if (strcmp(d.tags[i], "sound") == 0)
      o |= 0b00000000;
  }

  return o;
}

size_t compile_lit(literal l, byte** o) {
  switch(l.type) {
  case integral:
    *o = malloc(sizeof(int64_t));
    **o = l.i;
    return sizeof(int64_t);
  case fractional:
    *o = malloc(sizeof(double));
    **o = l.f;
    return sizeof(double);
  case vector:
  case matrix:
    *o = malloc(sizeof(double) * l.m.length);
    memcpy(*o, l.m.values, l.m.length);
    free(l.m.values);
    return sizeof(double) * l.m.length;
  }
}

data_object compile_data(byte id, data_def d) {
  data_object o;
  o.id = id;
  o.flags = set_data_flags(d);
  o.length = 0;
  o.content = NULL;

  printf("compiling data %d\n", id);

  for (int i = 0; i < d.content_count; i++) {
    printf("content line %d\n", i);
    printf("%s\n", d.content[i].str);
    int pos = 0;
    byte* s = NULL;

    while (pos < d.content[i].len) {
      literal l = consume_lit(d.content[i].str, &pos, d.content[i].len);
      size_t size = compile_lit(l, &s);

      o.content = realloc(o.content, o.length + size);
      printf("copying %ld lit bytes\n", size);
      memcpy(&o.content[o.length], s, size);

      o.length += size;
      free(s);
    }
  }

  printf("finished compiling data %d\n", id);

  return o;
}
