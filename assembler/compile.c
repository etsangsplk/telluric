#include "compile.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

instr_encoded encode_instr(instr in) {
  instr_encoded o;

  int total_len = 1; // doesn't include hdr

  byte* operands = NULL;
  int operands_len = 0;
  for (int i = 0; i < in.operand_count; i++) {
    operands = realloc(operands, operands_len + in.operands[i].value_len + 1);

    operands[operands_len] = in.operands[i].type;
    memcpy(&operands[operands_len+1], in.operands[i].value, in.operands[i].value_len);

    operands_len += in.operands[i].value_len + 1;
  }
  total_len += operands_len;

  byte* return_operand;
  int return_operand_len;
  if (in.has_return) {
    o.hdr.has_return = true;
    return_operand = malloc(in.return_operand.value_len + 1);
    return_operand[0] = in.return_operand.type;
    memcpy(&return_operand[1], in.return_operand.value, in.return_operand.value_len);
    return_operand_len = in.return_operand.value_len + 1;
    total_len += return_operand_len;
  }

  o.hdr.length = total_len;

  int i = 0;
  o.bytes = malloc(total_len);
  if (in.has_return) {
    i += return_operand_len;
    memcpy(o.bytes, return_operand, i);
  }
  o.bytes[i] = in.opcode;
  i++;
  if (operands_len > 0) {
    memcpy(&o.bytes[i], operands, operands_len);
  }

  return o;
}

object compile_code(code_section c, data_domains d) {
  object o;
  o.ev = c.ev;

  o.instrs = malloc(sizeof(instr_encoded) * c.instr_count * 2); // double space for possible expansions
  o.instrs_length = 0;

  printf("compiling event %d\n", c.ev);

  for (int i = 0; i < c.instr_count; i++) {
    instr parsed = parse_instr(d, c.instr[i]);
    instr* expanded = NULL;
    int len = 0;

    if (expand_convenience(expanded, &len, parsed)) {
      for (int j = 0; j < len; j++) {
        o.instrs[o.instrs_length] = encode_instr(expanded[j]);
        o.instrs_length++;
      }
    } else {
      o.instrs[o.instrs_length] = encode_instr(parsed);
      o.instrs_length++;
    }
  }

  o.instrs = realloc(o.instrs, sizeof(instr_encoded) * o.instrs_length);
  return o;
}
