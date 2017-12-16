#pragma once

#include <stdint.h>

#include "parse.h"
#include "identify.h"

typedef enum {
  imm_int = 1, // len 8
  imm_float,   // len 8
  imm_vector,  // len 8*4
  imm_matrix,  // len 8*16
  reg_int,     // len 1
  reg_float,   // len 1
  reg_jump,    // len 1
  reg_vector,  // len 2
  reg_matrix,  // len 1
  data_label,  // len 1
  data_index,  // len 2
  bus_addr     // len 1
} operand_type;

typedef struct {
  operand_type type : 8;
  int value_len;
  byte* value;
} instr_operand;

static const int pneumonics_len = 53;

static const char* pneumonics[pneumonics_len] = {
  "and",
  "or",
  "xor",
  "ext",
  "abs",
  "add",
  "sub",
  "mul",
  "div",
  "pow",
  "sqrt",
  "shift",
  "rotate",
  "sin",
  "cos",
  "tan",
  "bound",
  "unnan",
  "norm",
  "dot",
  "round",
  "trun",
  "cast",
  "broadcast",
  "parse",
  "copy",
  "clear",
  "jmp",
  "call",
  "jeq",
  "jneq",
  "jgt",
  "jlt",
  "exit",
  "swap",
  // convenience instrs
  "mov",
  "char",
  "pixel",
  "line",
  "blit",
  "rectblit",
  "read",
  "play",
  "listen",
  "interface",
  "negotiate",
  "accept",
  "transfer",
  "deinterface",
  "connect",
  "packet",
  "stream",
  "disconnect"
};

typedef struct {
  instr_operand return_operand;
  bool has_return;
  byte opcode;
  int operand_count;
  instr_operand* operands;
} instr;

instr parse_instr(data_domains d, line l);
byte parse_opcode(char*c, int len);
bool expand_convenience(instr* os, int* len, instr i);
