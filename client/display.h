// client renderer subsystem

#pragma once

#include "types.h"
#include "error.h"

error display_init();
void display_resize(int x, int y);
void display_close();

// temporary

tabset_ref create_tabset();
window_ref create_window(application_ref a);
