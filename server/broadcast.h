#pragma once

#include "types.h"
#include "protocol.h"

typedef struct {
  ident ident;
  timestamp date;
  byte* message; // includes encrypted origin timestamp
  counter message_length;
} broadcast;

broadcast* broadcasts_since(timestamp t, counter* length);
void store_broadcast(broadcast b);
