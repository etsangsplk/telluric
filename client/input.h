// client input handling

#pragma once

#include "types.h"
#include "error.h"

error input_init();
void input_poll();
void input_close();
