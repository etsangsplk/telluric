// client options parsing & format

#pragma once

typedef struct opts {
  char* store_path;
} opts;

opts parse_opts(int argc, char* argv[]);
