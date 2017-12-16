#include "reduce.h"

#include <stdio.h>
#include <string.h>

meta_section reduce_meta(asm_section s) {
  meta_section o;
  o.keys = malloc(sizeof(char*) * s.lines_count);
  o.vals = malloc(sizeof(char*) * s.lines_count);
  o.count = 0;

  for (int i = 0; i < s.lines_count; i++) {
    int c = 0;
    while (c < s.lines[i].len && s.lines[i].str[c] != '=') c++;

    if (c == s.lines[i].len) {
      printf("invalid meta\n");
      continue;
    }

    o.keys[o.count] = calloc(sizeof(char), c);
    memcpy(o.keys[o.count], s.lines[i].str, sizeof(char) * c);

    c++; // omit =

    o.vals[o.count] = calloc(sizeof(char), s.lines[i].len - c + 1);
    memcpy(o.vals[o.count], s.lines[i].str + c, s.lines[i].len - c);

    o.count++;

    printf("meta key: %s, meta val: %s\n", o.keys[i], o.vals[i]);
  }

  o.keys = realloc(o.keys, sizeof(char*) * o.count);
  o.vals = realloc(o.vals, sizeof(char*) * o.count);

  return o;
}

data_section reduce_data(asm_section s) {
  data_section o;
  o.defs = malloc(sizeof(data_def) * s.lines_count);
  o.defs_count = 0;

  int c = 0;

  while (c < s.lines_count) {
    // assume first line is def
    data_def d;

    // tag endpoint
    int t = c;
    while (t < s.lines_count - 1 && s.lines[t + 1].str[0] == '[') t++;

    // data endpoint
    int e = t + 1;
    while (e < s.lines_count && s.lines[e].str[0] != ':') e++;
    d.content_count = e - t - 1;

    // name
    int n = s.lines[c].len - 1;

    d.name = malloc(sizeof(char) * n + 1);
    memcpy(d.name, &s.lines[c].str[1], sizeof(char) * n);

    // tags
    d.tags = malloc(sizeof(char*) * t);
    d.tags_count = 0;

    while (t > c) {

      n = 0;
      while (n < s.lines[t].len && s.lines[t].str[n] != ']') n++;

      if (n == s.lines[t].len) {
        printf("skipping malformed tag in def %s\n", d.name);
        t--;
        continue;
      }

      d.tags[d.tags_count] = malloc(sizeof(char) * s.lines[t].len - 1);
      memcpy(d.tags[d.tags_count], &s.lines[t].str[1], n - 1);

      d.tags_count++;
      t--;
    }

    d.content = malloc(sizeof(line) * (d.content_count));
    memcpy(d.content, &s.lines[e - d.content_count], sizeof(line) * d.content_count);

    o.defs[o.defs_count] = d;
    o.defs_count++;
    c = e;
  }

  o.defs = realloc(o.defs, sizeof(data_def) * o.defs_count);
  return o;
}

code_section reduce_code(asm_section s) {
  code_section c;
  if (strcmp(s.name, "main") == 0) {
    c.ev = main_fn;
  } else if (strcmp(s.name, "close") == 0) {
    c.ev = close_fn;
  } else if (strcmp(s.name, "render") == 0) {
    c.ev = render_fn;
  } else if (strcmp(s.name, "resize") == 0) {
    c.ev = resize_fn;
  } else if (strcmp(s.name, "network") == 0) {
    c.ev = network_fn;
  }

  c.instr = s.lines;
  c.instr_count = s.lines_count;

  return c;
}

reduced_file reduce_file(sectioned_file s) {
  reduced_file o;
  o.code = malloc(sizeof(code_section) * s.sections_count);
  o.code_count = 0;
  for (int i = 0; i < s.sections_count; i++) {
    if (strcmp(s.sections[i].name, "meta") == 0) {
      printf("reducing meta\n");
      o.meta = reduce_meta(s.sections[i]);
    } else if (strcmp(s.sections[i].name, "data") == 0) {
      printf("reducing data\n");
      o.data = reduce_data(s.sections[i]);
    } else {
      printf("reducing code section %s\n", s.sections[i].name);
      o.code[o.code_count] = reduce_code(s.sections[i]);
      o.code_count++;
    }
  }

  o.code = realloc(o.code, sizeof(code_section) * o.code_count);
  return o;
}
