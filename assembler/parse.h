// assembly parsing

#pragma once

#include "stdlib.h"

#include "types.h"

typedef struct {
  char* str;
  int len;
} line;

typedef struct {
  line* lines;
  int lines_count;
} asm_file;

typedef struct {
  char* name;
  line* lines;
  int lines_count;
} asm_section;

typedef struct {
  asm_section* sections;
  int sections_count;
} sectioned_file;


asm_file load_file(char* filepath);
asm_file strip_nonsemantic(asm_file i);
sectioned_file parse_sections(asm_file i);
