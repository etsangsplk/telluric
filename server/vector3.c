#include "vector3.h"

#include <math.h>

double norm(vector3 i) {
  return sqrt(pow(i.x, 2) + pow(i.y, 2) + pow(i.z, 2));
}

vector3 normalize(vector3 i) {
  return divide(i, norm(i));
}

vector3 add(vector3 l, vector3 r) {
  return (vector3) { l.x + r.x, l.y + r.y, l.z + r.z };
}

vector3 subtract(vector3 l, vector3 r) {
  return (vector3) { l.x - r.x, l.y - r.y, l.z - r.z };
}

vector3 multiply(vector3 l, double s) {
  return (vector3) { l.x * s, l.y * s, l.z * s};
}

vector3 divide(vector3 l, double s) {
  return (vector3) { l.x / s, l.y / s, l.z / s };
}

vector3 cross(vector3 l, vector3 r) {
  vector3 o;
  o.x = l.y*r.z - l.z*r.y;
  o.y = l.z*r.x - l.x*r.z;
  o.z = l.x*r.y - l.y*r.x;
  return o;
}

double dot(vector3 l, vector3 r) {
  return l.x*r.x + l.y*r.y + l.z*r.z;
}
