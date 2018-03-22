#include "program.h"

#include <sqlite3.h>

program get_program(store_id p) {
  char* q =
    "select object from programs where index = ?";

  sqlite3_stmt* s;
  sqlite3_prepare(state.db, q, strlen(q), &s, NULL);
  sqlite3_bind()
}

void init_program(program p) {}
void free_program(program p) {}

byte* instr_dispatch(byte* instr, counter run) {
}

void execute_main(program* p, counter cycles) {
  for (counter i = 0; i < cycles; i++) {
    p->instr =
      instr_dispatch(p->instr, p->sections.main.limit_offset - (int) p->instr);
  }
}
