// client virtual machine

#pragma once

#include "types.h"
#include "error.h"
#include "graphics.h"
#include "state.h"

application_ref new_app(program p, window_ref w);

error vm_init();
void vm_close();

void vm_render(application_ref a);
