#pragma once

#include <stdint.h>

typedef enum { false, true } bool;
typedef struct { char* s; int start; int end;} bytestring; // "string" doesn't seem to be a valid typedef name

typedef enum {main_fn, close_fn, render_fn, resize_fn, network_fn} event;

typedef uint8_t byte;
