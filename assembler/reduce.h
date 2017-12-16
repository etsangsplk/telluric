#pragma once

#include "parse.h"

typedef struct {
  char** keys;
  char** vals;
  int count;
} meta_section;

typedef struct {
  char* name;
  char** tags;
  int tags_count;
  line* content;
  int content_count;
} data_def;

typedef struct {
  data_def* defs;
  int defs_count;
} data_section;

typedef struct {
  event ev;
  line* instr;
  int instr_count;
} code_section;

typedef struct {
  meta_section meta;
  data_section data;
  code_section* code;
  int code_count;
} reduced_file;

reduced_file reduce_file(sectioned_file s);
