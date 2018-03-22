#include "state.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

void create_db() {
  char* qs[] = {
    "create table routes (index integer, content blob) if not exists",
    "create table neighbours (index integer, content blob) if not exists",
    "create table associations (index integer, content blob) if not exists",

    "create table idents (ident integer, name text, pubkey blob) if not exists",
    "create table broadcasts (timestamp integer, ident integer, message blob) if not exists"
  };

  sqlite3_stmt* s;

  for (counter i = 0; i < 5; i++) {
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
  if (c != 5) goto fail;

  sqlite3_finalize(s);
  return true;

 fail:
  sqlite3_finalize(s);
  return false;
}

void open_db() {
  int stat = sqlite3_open(state.config.state_file, &state.db);

  if (stat) {
    printf("failed to open db\n");

    sqlite3_close(state.db);
    exit(1);
  }

  if (!test_schema()) create_db();
}

counter count_table(char* table) {
  char q[64];
  sprintf(q, "select count(*) from %s", table);

  sqlite3_stmt* s;
  sqlite3_prepare(state.db, q, strlen(q), &s, NULL);

  sqlite3_step(s);
  counter c = sqlite3_column_int(s, 0);

  sqlite3_finalize(s);
  return c;
}

void read_state() {
  routing_knowledge r;
  sqlite3_stmt* s;

  state.routes.routes_count = count_table("routes");
  state.routes.routes = malloc(sizeof(route_embedding) * state.routes.routes_count);

  char* q = "select * from routes order by index asc";
  sqlite3_prepare(state.db, q, strlen(q), &s, NULL);
  for (counter i = 0; i < state.routes.routes_count; i++) {
    if(sqlite3_step(s) == SQLITE_ROW) {
      void* b = sqlite3_column_blob(s, 1);
      memcpy(&state.routes.routes[i], b, sizeof(route_embedding));
    }
  }
  sqlite3_finalize(s);

  state.routes.nodes_count = count_table("neighbours");
  state.routes.nodes = malloc(sizeof(neighbour_embedding) * state.routes.nodes_count);

  q = "select * from neighbours order by index asc";
  sqlite3_prepare(state.db, q, strlen(q), &s, NULL);
  for (counter i = 0; i < state.routes.nodes_count; i++) {
    if(sqlite3_step(s) == SQLITE_ROW) {
      void* b = sqlite3_column_blob(s, 1);
      memcpy(&state.routes.nodes[i], b, sizeof(neighbour_embedding));
    }
  }
  sqlite3_finalize(s);

  state.routes.associations_count = count_table("associations");
  state.routes.associations = malloc(sizeof(route_association) * state.routes.associations_count);

  q = "select * from associations order by index asc";
  sqlite3_prepare(state.db, q, strlen(q), &s, NULL);
  for (counter i = 0; i < state.routes.associations_count; i++) {
    if(sqlite3_step(s) == SQLITE_ROW) {
      void* b = sqlite3_column_blob(s, 1);
      memcpy(&state.routes.associations[i], b, sizeof(route_association));
    }
  }
  sqlite3_finalize(s);
}

void write_state() {
  sqlite3_stmt* s;

  char* q = "insert or replace into routes values (?, ?)";
  for (counter i = 0; i < state.routes.routes_count; i++) {
    sqlite3_prepare(state.db, q, strlen(q), &s, NULL);
    sqlite3_bind_int(s, 1, i);
    sqlite3_bind_blob(s, 2, &state.routes.routes[i], sizeof(route_embedding), SQLITE_TRANSIENT);
    sqlite3_step(s);
    sqlite3_finalize(s);
  }

  q = "insert or replace into neighbours values (?, ?)";
  for (counter i = 0; i < state.routes.nodes_count; i++) {
    sqlite3_prepare(state.db, q, strlen(q), &s, NULL);
    sqlite3_bind_int(s, 1, i);
    sqlite3_bind_blob(s, 2, &state.routes.nodes[i], sizeof(neighbour_embedding), SQLITE_TRANSIENT);
    sqlite3_step(s);
    sqlite3_finalize(s);
  }

  q = "insert or replace into associations values (?, ?)";
  for (counter i = 0; i < state.routes.nodes_count; i++) {
    sqlite3_prepare(state.db, q, strlen(q), &s, NULL);
    sqlite3_bind_int(s, 1, i);
    sqlite3_bind_blob(s, 2, &state.routes.associations[i], sizeof(route_association), SQLITE_TRANSIENT);
    sqlite3_step(s);
    sqlite3_finalize(s);
  }
}

void state_init(opts o) {
  state.config = o;

  open_db();
  read_state();
}
