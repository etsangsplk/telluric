#pragma once

#include "types.h"
#include "protocol.h"

typedef struct {
  ident id; // set to 0 for failed retreival
  byte* name;
  counter name_length;
  byte* key;
  counter key_length;
} ident_info;

ident_info get_ident(ident id);
void set_ident(ident_info id);

void free_ident_info(ident_info id);

bool validate_ident(ident id, signature sigvec, byte* cyphertext, counter cyphertext_length);
