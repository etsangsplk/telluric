#pragma once

#include <stdint.h>

typedef enum {false, true} bool;

typedef union {
  uint32_t u;
  float f;
  struct {
    uint32_t mantissa : 23;
    uint32_t exponent : 8;
    uint32_t sign     : 1;
  };
} fp32;

typedef union {
  uint16_t u;
  struct {
    uint16_t mantissa : 10;
    uint16_t exponent : 5;
    uint16_t sign     : 1;
  };
} fp16;

typedef uint8_t byte;

typedef int counter;
typedef int file;

typedef counter neighbour_connection_ref;
typedef counter client_connection_ref;

typedef counter session_ref;

fp16 f2h(fp32 in);
fp32 h2f(fp16 in);

typedef struct {
  unsigned int i : 24;
} ident;

typedef byte telluric_char[4];

typedef struct {
  char* name;
  ident id;
} host_address;
