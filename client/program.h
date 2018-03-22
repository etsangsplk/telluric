#pragma once

#include "state.h"

program get_program(store_id p);
void free_program(program p);

void execute_main(program* p, counter cycles);
void execute_event(program* p);
