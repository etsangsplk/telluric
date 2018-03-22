// client global type definitions

#pragma once

#include <stdint.h>
#include <stdlib.h>

typedef enum {false, true} bool;

typedef int counter;
typedef int file;

// storage primitive
typedef uint8_t byte;

// ix - signed integer
typedef int64_t integral;
// fx - double-precision float
typedef double fractional;
// mx[n] - 4-vector
typedef fractional vector[4];
// mx - 4x4 matrix
typedef vector matrix[4];

// int 4-vector used by renderer
typedef struct {
  integral x;
  integral y;
  integral w;
  integral h;
} rect;

// bytestring storage
typedef struct bytestring {
  integral length;
  byte* content;
} bytestring;

#define nullstring (bytestring){0, NULL}

typedef void (*voidfn)();

typedef counter window_ref;
typedef counter application_ref;
typedef counter tabset_ref;
