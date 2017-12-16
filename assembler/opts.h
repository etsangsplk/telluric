#pragma once

typedef struct {
  char* in_file;
  char* out_file;
} opts;

opts parse_opts(int argc, char* argv[]);
