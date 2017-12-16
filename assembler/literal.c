#include "literal.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "types.h"

literal parse_simple(char* c, int* pos, int len) {
  literal l;
  int n = *pos;
  bool neg = false;

  while (n < len && c[n] != ' ' && c[n] != ']') n++;

  if (c[n] == '-') {
    neg = true;
    n++;
  }

  int d = *pos;

  while (d < n && c[d] != '.') d++;

  char* e = &c[n];

  if (d == n) {
    l.type = integral;
    l.i = strtol(&c[*pos], &e, 10);
    if (neg) l.i *= -1;
  } else {
    l.type = fractional;
    l.f = strtod(&c[*pos], &e);
    if (neg) l.f *= -1.0f;
  }

  // consume trailing space, commas
  // can expect next char to end a chain or be another simple
  *pos = n;
  while (*pos < len && (c[*pos] == ',' || c[*pos] == ' ')) (*pos)++;
  return l;
}

literal parse_vector(char* c, int* pos, int len) {
  int n = *pos + 1;
  literal l, f;
  l.type = vector;
  l.m.length = 0;
  l.m.values = malloc(sizeof(double) * 4);

  while (n < len && c[n] != ']') {
    f = parse_simple(c, &n, len);
    l.m.values[l.m.length] = f.f;
    l.m.length++;
  }

  if (l.m.length != 4) {
    printf("incomplete vector literal, len %d\n", l.m.length);
    exit(1);
  }

  *pos = n + 1;
  while (*pos < len && (c[*pos] == ',' || c[*pos] == ' ')) (*pos)++;

  return l;
}

literal parse_composite(char* c, int* pos, int len) {
  literal o;
  int n = *pos + 1;

  while (n < len && c[n] == ' ') n++;

  if (c[n] == '[') {
    o.type = matrix;
    o.m.values = malloc(sizeof(double) * 16);
    o.m.length = 0;
    literal v;

    while (c[n] != ']') {
      v = parse_vector(c, &n, len);
      memcpy(&o.m.values[o.m.length], v.m.values, sizeof(double) * 4);
      free(v.m.values);
      o.m.length += 4;
    }

    if (o.m.length != 16) {
      printf("incomplete matrix literal\n");
      exit(1);
    }

    *pos = n + 1;
    while (*pos < len && (c[*pos] == ',' || c[*pos] == ' ')) (*pos)++;
  } else {
    o = parse_vector(c, pos, len);
  }

  return o;
}

// parse a literal (not ref, not string) value, and whitespace after it
// and update len to reflect new position
literal consume_lit(char* c, int* pos, int len) {
  if (c[0] == '[')
    return parse_composite(c, pos, len);

  if (c[0] > 47 && c[0] < 59)
    return parse_simple(c, pos, len);

  printf("invalid literal\n");
  exit(0);
}

bool optional_lit(literal* l, char* c, int len) {
  int pos = 0;
  if (c[0] == '[') {
    *l = parse_composite(c, &pos, len);
    return true;
  }

  if (c[0] > 47 && c[0] < 59) {
    *l = parse_simple(c, &pos, len);
    return true;
  }

  return false;
}
