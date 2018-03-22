#include "types.h"

// stolen from ryg's adaption of ispc code

fp16 f2h(fp32 in) {
  static const fp32 inf = {31 << 23};
  static const fp32 mag = {15 << 23};
  fp16 out = {0};

  if (in.exponent == 255) {
    out.exponent = 31;
    out.mantissa = in.mantissa ? 0x200 : 0;
  } else {
    in.u &= ~0xffff;
    in.f *= mag.f;
    in.u *= 0x1000;
    if (in.u > inf.u) in.u = inf.u;
    out.u = in.u >> 13;
  }

  out.sign = in.sign;
  return out;
}

fp32 h2f(fp16 in) {
  static const fp32 mag = {113 << 23};
  fp32 out;

  if (in.exponent == 0) {
    out.u = mag.u + in.mantissa;
    out.f -= mag.f;
  } else {
    out.mantissa = in.mantissa << 13;
    if (in.exponent == 0x1f) out.exponent = 255;
    else out.exponent = 127 - 15 + in.exponent;
  }

  out.sign = in.sign;
  return out;
}
