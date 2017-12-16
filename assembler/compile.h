#pragma once

#include "instr.h"
#include "reduce.h"
#include "identify.h"

typedef struct {
  bool has_return : 1;
  byte length     : 7;
} header;

typedef struct {
  header hdr;
  byte* bytes;
} instr_encoded;

typedef struct {
  event ev;
  instr_encoded* instrs;
  int instrs_length;
} object;

object compile_code(code_section c, data_domains d);
