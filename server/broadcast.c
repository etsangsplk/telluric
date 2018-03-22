#include "broadcast.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

#include "state.h"
#include "time.h"

broadcast row_to_broadcast(sqlite3_stmt* s) {
  broadcast b;
  b.ident.i = sqlite3_column_int(s, 0);
  b.date.i = sqlite3_column_int(s, 1);
  b.message_length = sqlite3_column_bytes(s, 2);
  b.message = malloc(b.message_length);
  memcpy(b.message, sqlite3_column_blob(s, 2), b.message_length);
  return b;
}

// length initially set to maximum, overwritten to count
broadcast* broadcasts_since(timestamp t, counter* l) {
  timestamp now = get_timestamp();
  char* qs = "select * from broadcasts where (%s) and timestamp >= ?";
  char* q0 = "timestamp < (1 << 31)";
  char* q1 = "timestamp >= (1 << 31)";
  char q[128];

  sqlite3_stmt* s;
  broadcast* bs = malloc(sizeof(broadcast) * *l);
  *l = 0;

  // get all with matching yearity

  sprintf(q, qs, t.yearity ? q1 : q0);
  sqlite3_prepare(state.db, q, strlen(q), &s, NULL);
  sqlite3_bind_int(s, 1, t.i);

  while (sqlite3_step(s) == SQLITE_ROW) {
    bs[*l] = row_to_broadcast(s);
    *l += 1;
  }

  sqlite3_finalize(s);

  if (now.yearity != t.yearity) {
    // get all with mistmatched yearity
    sprintf(q, qs, now.yearity ? q1 : q0);
    sqlite3_prepare(state.db, q, strlen(q), &s, NULL);
    sqlite3_bind_int(s, 1, now.yearity << 31);

    while (sqlite3_step(s) == SQLITE_ROW) {
      bs[*l] = row_to_broadcast(s);
      *l += 1;
    }

    sqlite3_finalize(s);
  }

  bs = realloc(bs, sizeof(broadcast) * *l);
  return bs;
}

void store_broadcast(broadcast b) {
  char* q = "insert into broadcasts values (?, ?, ?)";
  sqlite3_stmt* s;

  sqlite3_prepare(state.db, q, strlen(q), &s, NULL);
  sqlite3_bind_int(s, 1, b.date.i);
  sqlite3_bind_int(s, 2, b.ident.i);
  sqlite3_bind_blob(s, 3, b.message, b.message_length, SQLITE_TRANSIENT);

  sqlite3_step(s);
  sqlite3_finalize(s);
}
