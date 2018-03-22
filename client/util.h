// memory and io util functions

#include "types.h"
#include "error.h"

void* alloc_array(integral length, size_t width);
void* resize_array(integral length, size_t width, void* array);
void* delete_elem(integral length, size_t width, void* array, integral index);

error read_file(char* path, byte* dest);
integral file_length(char* path);
