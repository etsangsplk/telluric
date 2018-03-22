#include "vector4.h"

////////////////////////////////////////////////////////////////////////////////
// packings

vector4 unpack_netvec(netvec v) {
  return (vector4) {h2f(v.x), h2f(v.y), h2f(v.z), h2f(v.w)};
}

netvec pack_netvec(vector4 v) {
  return (netvec) {f2h(v.x), f2h(v.y), f2h(v.z), f2h(v.w)};
}

fp32 b2f(byte in) {
  return (fp32) {((float) in / 255.0f * 2.0f) - 1.0f};
}

byte f2b(fp32 in) {
  return (byte) ((in.f + 1.0f) / 2.0f * 255.0f);
}

vector4 deserialize_vector(bytevec b) {
  return (vector4) {b2f(b.x), b2f(b.y), b2f(b.z), b2f(b.w)};
}

bytevec serialize_vector(vector4 v) {
  return (bytevec) {f2b(v.x), f2b(v.y), f2b(v.z), f2b(v.w)};
}
