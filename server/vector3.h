// simple 3-vector with arithmetic

#pragma once

typedef struct {
  double x;
  double y;
  double z;
} vector3;

double norm(vector3 i);
vector3 normalize(vector3 i);

vector3 add(vector3 l, vector3 r);
vector3 subtract(vector3 l, vector3 r);

vector3 multiply(vector3 l, double s);
vector3 divide(vector3 l, double s);

vector3 cross(vector3 l, vector3 r);
double dot(vector3 l, vector3 r);
