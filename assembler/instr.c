#include "instr.h"

#include <stdio.h>
#include <string.h>

#include "literal.h"
#include "identify.h"

int first_char(char* c, int len) {
  for (int i = 0; i < len; i++)
    if (c[i] != ' ' && c[i] != '\t')
      return i;

  return -1;
}

int first_space(char* c, int len) {
  for (int i = 0; i < len; i++)
    if (c[i] == ' ')
      return i;

  // eol counts as space
  return len;
}

byte parse_opcode(char* c, int len) {
  for (int i = 0; i < pneumonics_len; i++) {
    int n = strlen(pneumonics[i]);

    if (n > len) continue;

    if (strncmp(pneumonics[i], c, n) == 0)
      return i;
  }

  return 255;
}

bool parse_reg(instr_operand* o, char* c, int len) {
  printf("len: %d\n", len);
  if (len < 2) return false;

  switch (c[0]) {
  case 'i':
    o->type = reg_int;
    break;
  case 'f':
    o->type = reg_float;
    break;
  case 'p':
    o->type = reg_jump;
    break;
  case 'm':
    o->type = reg_matrix;
    break;
  }

  int reg_num = 0;

  if (c[1] > 48 && c[1] < 57) {
    reg_num = c[1] - 48;
  } else return false;

  if (len == 2) {
    o->value_len = 1;
    o->value = calloc(o->value_len, sizeof(*o->value));
    o->value[0] = reg_num;
    return true;
  }

  int vec_idx = 0;

  // check for vector index
  if (o->type == reg_matrix && len == 5)
    if (c[3] == '[' && c[5] == ']')
      if (c[4] > 47 && c[4] < 52) {
        o->type = reg_vector;
        vec_idx = c[4] - 48;

        o->value_len = 2;
        o->value = calloc(o->value_len, sizeof(*o->value));
        o->value[0] = reg_num;
        o->value[1] = vec_idx;

        return true;
      }

  return false;
}

bool parse_lit(instr_operand* o, char* c, int len) {
  literal l;

  bool s = optional_lit(&l, c, len);

  if (s) {
    o->type = l.type;

    switch (l.type) {
    case integral:
      o->type = imm_int;
    case fractional:
      o->type = imm_float;
      o->value_len = 8;
      o->value = calloc(o->value_len, sizeof(*o->value));
      break;
    case vector:
      o->type = imm_vector;
      o->value_len = 8*4;
      o->value = calloc(o->value_len, sizeof(*o->value));
      memcpy(o->value, l.m.values, o->value_len);
      free(l.m.values);
      break;
    case matrix:
      o->type = imm_matrix;
      o->value_len = 8*16;
      o->value = calloc(o->value_len, sizeof(*o->value));
      memcpy(o->value, l.m.values, o->value_len);
      free(l.m.values);
      break;
    }

    return true;
  } else {
    return false;
  }
}

bool parse_label(data_domains d, instr_operand* o, char* c, int len) {
  if (c[0] != ':') return false;

  int n = 0;
  while (n < len && c[n] != '[') n++;

  printf("parsing label %s\n", c);

  o->type = n == len ? data_label : data_index;

  int id = get_ref_id(d, c, n);
  if (id == -1) return false;

  if (n == len) {
    o->value_len = 1;
    o->value = calloc(o->value_len, sizeof(*o->value));
    o->value[0] = id;
    return true;
  } else {
    if (n+2 > len) return false;

    int e = n;
    while (e < len && c[e] != ']') e++;

    char* ee = &c[e];
    long idx = strtol(&c[n+1], &ee, 10);

    if (idx > 63) return false;

    o->value_len = 2;
    o->value = calloc(o->value_len, sizeof(*o->value));
    o->value[0] = id;
    o->value[1] = idx;
    return true;
  }
}

// parse and consume operand
instr_operand parse_operand(data_domains d, char* c, int* n, int len) {
  instr_operand o;

  int next = *n + first_space(&c[*n], len-*n);
  int lim = next < len ? len - next - 1 : len - *n;

  printf("next: %d\n", next);
  if (next < len)
    next += first_char(&c[next], len);
  printf("next: %d\n", next);

  if (parse_reg(&o, &c[*n], lim)) {}
  else if (parse_lit(&o, &c[*n], lim)) {}
  else if (parse_label(d, &o, &c[*n], lim)) {}
  else {
    printf("invalid operand\n");
    exit(1);
  }

  *n = next;
  return o;
}

instr parse_instr(data_domains d, line l) {
  instr o;
  o.operand_count = 0;

  printf("compiling instr line:\n");
  printf("%s\n", l.str);

  int n = first_char(l.str, l.len);

  if (n == -1) {
    printf("attempted to compile empty line\n");
    exit(1);
  }

  int e = first_space(&l.str[n], l.len-n);

  // todo: check for return operand
  o.has_return = false;

  byte opcode = parse_opcode(&l.str[n], e);
  printf("opcode: %d\n", opcode);

  if (opcode == 255) {
    printf("invalid instruction op:\n%s\n", l.str);
    exit(1);
  }

  n += e;

  if (first_char(&l.str[n], l.len-n) == -1) {
    // no args
    printf("no arguments\n");
    return o;
  }

  n += first_char(&l.str[n], l.len-n);

  o.operands = calloc(8, sizeof(*o.operands));
  while (n < l.len) {
    printf("n: %d - %c\n", n, l.str[n]);
    o.operands[o.operand_count] = parse_operand(d, l.str, &n, l.len);
    o.operand_count++;
  }

  return o;
}

bool expand_convenience(instr* os, int* len, instr i) {
  // todo: map convenience to bus address and activators
  return false;
}
