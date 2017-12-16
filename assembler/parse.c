#include "parse.h"

// assembly parser, structured similarly to parser combinators, but with no concern for efficiency

#include <stdio.h>
#include <string.h>

asm_file load_file(char* filepath) {
  int upper = 128;
  asm_file o;
  o.lines = malloc(sizeof(line) * upper);
  o.lines_count = 0;

  FILE* f = fopen(filepath, "r");

  ssize_t len;
  char* str = malloc(8);
  size_t len_ = 8;

  while ((len = getline(&str, &len_, f)) != -1) {
    o.lines[o.lines_count].str = malloc(len);
    memcpy(o.lines[o.lines_count].str, str, len);
    o.lines[o.lines_count].len = len;

    o.lines_count++;

    if (o.lines_count == upper) {
      upper <<= 1;
      o.lines = realloc(o.lines, sizeof(line) * upper);
    }
  }

  o.lines = realloc(o.lines, sizeof(line) * o.lines_count);

  fclose(f);
  return o;
}

asm_file strip_nonsemantic(asm_file in) {
  asm_file o;
  o.lines_count = 0;
  o.lines = malloc(sizeof(line) * in.lines_count);

  for (int i = 0; i < in.lines_count; i++) {

    // strip comments
    int l = 0;
    while (l < in.lines[i].len) {
      if (in.lines[i].str[l] == ';') break;
      if (in.lines[i].str[l] == '\n') break;
      l++;
    }

    // strip hanging whitespace
    while (l > 0) {
      if (in.lines[i].str[l] != ' ') break;
      if (in.lines[i].str[l] != '\t') break;
      l--;
    }

    // cull blank lines
    if (l == 0) {
      free(in.lines[i].str);
      continue;
    }

    o.lines[o.lines_count].str = calloc(sizeof(char), l + 1);
    o.lines[o.lines_count].len = l;
    memcpy(o.lines[o.lines_count].str, in.lines[i].str, l);
    free(in.lines[i].str);

    o.lines_count++;
  }

  free(in.lines);
  return o;
}

asm_section valid_section(char* name, int len) {
  asm_section s;

  if (strncmp("meta", name, len) == 0) {
    s.name = "meta";
  } else if (strncmp("data", name, len) == 0) {
    s.name = "data";
  } else if (strncmp("main", name, len) == 0) {
    s.name = "main";
  } else if (strncmp("close", name, len) == 0) {
    s.name = "close";
  } else if (strncmp("render", name, len) == 0) {
    s.name = "render";
  } else if (strncmp("resize", name, len) == 0) {
    s.name = "resize";
  } else if (strncmp("network", name, len) == 0) {
    s.name = "network";
  } else {
    s.name = "null";
  }

  return s;
}

sectioned_file parse_sections(asm_file in) {
  int c = 0;
  sectioned_file o;
  o.sections = malloc(sizeof(asm_section) * 128);
  o.sections_count = 0;

  while (c < in.lines_count-1) {
    // assume first line is section
    asm_section s =
      valid_section(in.lines[c].str+1, in.lines[c].len-1);

    int e = c + 1;

    while (e < in.lines_count && in.lines[e].str[0] != '.') e++;

    if (strcmp(s.name, "null") == 0) {
      printf("unrecognized or malformed section: ");
      puts(in.lines[c].str);
      c = e;
      continue;
    }

    c++; // skip header

    s.lines_count = e - c;
    s.lines = malloc(sizeof(line) * s.lines_count);
    memcpy(s.lines, &in.lines[c], sizeof(line) * s.lines_count);

    o.sections[o.sections_count] = s;
    o.sections_count++;
    c = e;
  }

  o.sections = realloc(o.sections, sizeof(asm_section) * o.sections_count);

  return o;
}
