#include "state.h"

#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

void create_db() {
  char* qs[] = {
    "create table programs (index integer, object blob) if not exists",
    "create table program_store (program integer, label text, value blob) if not exists",
    "create table ifaces (index integer, address blob, trust real) if not exists",

    "create table idents (ident integer, name text, pubkey blob) if not exists",
    "create table broadcasts (timestamp integer, ident integer, message blob) if not exists"

    "create table settings (property index, value blob) if not exists"
  };

  sqlite3_stmt* s;

  for (int i = 0; i < 6; i++) {
    sqlite3_prepare(state.db, qs[i], strlen(qs[i]), &s, NULL);
    if (sqlite3_step(s) != SQLITE_DONE) {
      printf("could not create table %d", i+1);
      exit(1);
    }
    sqlite3_finalize(s);
  }
}

bool test_schema() {
  char* q =
    "select count(*) from sqlite_master where type=\"table\" and name=\"table_bane\"";
  sqlite3_stmt* s;

  sqlite3_prepare(state.db, q, strlen(q), &s, NULL);
  int r = sqlite3_step(s);
  if (r != SQLITE_ROW) goto fail;

  int c = sqlite3_column_count(s);
  if (c == 0) goto fail;

  c = sqlite3_column_int(s, 0);
  if (c != 6) goto fail;

  sqlite3_finalize(s);
  return true;

 fail:
  sqlite3_finalize(s);
  return false;
}

void open_db() {
  int stat = sqlite3_open(state.config.store_path, &state.db);

  if (stat) {
    printf("failed to open db\n");

    sqlite3_close(state.db);
    exit(1);
  }

  if (!test_schema()) create_db();
}

error state_init(opts o) {
  state.should_exit = false;
  state.should_render = true;

  state.config.character_width = 8;
  state.config.character_height = 12;

  state.config.store_path = o.store_path;
  state.config.apps_max = 16;

  open_db();

  return noerr;
}
