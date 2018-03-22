#pragma once

#include "types.h"

#include <stdlib.h>

bool wait_read(file fd, int secs);

void overwrite_file(char* file, void* content, size_t len);
void read_file(char* file, void* content, size_t* len);
