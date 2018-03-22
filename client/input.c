#include "input.h"

#include <stdio.h>

#include "state.h"
#include "display.h"

void keyboard_event() {}

// ime support
void text_event() {}

void mouse_event() {}

void window_event() {
}

error input_init() {
  return noerr;
}

void input_poll() {
  return;
}

void input_close() {}
