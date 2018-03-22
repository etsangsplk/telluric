// 4-vector with packed half-float representation

#pragma once

#include <stdint.h>

#include "types.h"

typedef struct {
  fp32 x;
  fp32 y;
  fp32 z;
  fp32 w;
} vector4;

////////////////////////////////////////////////////////////////////////////////
// packings

typedef struct {
  fp16 x;
  fp16 y;
  fp16 z;
  fp16 w;
} netvec;

typedef struct {
  byte x;
  byte y;
  byte z;
  byte w;
} bytevec;

vector4 unpack_netvec(netvec v);
netvec pack_netvec(vector4 v);

vector4 deserialize_vector(bytevec b);
bytevec serialize_vector(vector4 v);

fp32 b2f(byte in);
byte f2b(fp32 in);
