#pragma once

#include "vector4.h"

typedef struct {
  fp32 m[4][4];
} matrix4;

vector4 gemv(matrix4 m, vector4 v);
matrix4 gemm(matrix4 l, matrix4 r);
matrix4 invert(matrix4 m);
