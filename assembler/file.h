#pragma once

#include "types.h"

#include "identify.h"
#include "compile.h"
#include "data.h"

typedef struct {
  uint32_t len;
  byte* instrs;
} program_section;

typedef struct {
  uint32_t len;
  byte flags;
  byte* data;
} storage_section;

typedef struct {
  byte id;
  uint32_t offset;
} offset_index;

typedef struct {
  uint64_t magic;
  byte flags;

  byte storage_count;
  offset_index* storage_indices;
  storage_section* storage;

  byte code_indices_len;
  offset_index* code_indices;
  program_section* code;
} program_file;

static const uint64_t telluric_filesig = 0xE59CB0E99BBB;

program_file compile_file(data_domains d, meta_section m, object* objs, int objs_len, data_object* dats, int dats_len);
byte* collapse_file(program_file p, int* len);
