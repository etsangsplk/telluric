#include "util.h"

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

void* alloc_array(integral length, size_t width) {
  return malloc(width * length);
}

void* resize_array(integral length, size_t width, void* array) {
  return realloc(array, width * length);
}

void* delete_elem(integral length, size_t width, void* array, integral index) {
  void* new = malloc(width * (length - 1));

  // copy elems before index
  memcpy(new, array, width * index);

  // copy elems after index
  memcpy(new, &array[index+1], width * (length - index - 1));

  free(array);

  return new;
}

error read_file(char* path, byte* dest) {
  struct stat s;
  if (stat(path, &s) == -1) return (error){true, NULL};

  dest = malloc(s.st_size);

  FILE* f = fopen(path, "r");

  fread(dest, sizeof(byte), s.st_size, f);

  fclose(f);
  return noerr;
}

integral file_length(char* path) {
  struct stat s;
  if (stat(path, &s) == -1) return 0;

  return s.st_size;
}
