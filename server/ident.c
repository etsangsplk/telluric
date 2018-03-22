#include "ident.h"

#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#include "state.h"
#include "crypto.h"

ident_info get_ident(ident id) {
  char* q = "select * from idents where id = ?";
  sqlite3_stmt* s;
  ident_info o;

  sqlite3_prepare(state.db, q, strlen(q), &s, NULL);
  sqlite3_bind_int(s, 1, id.i);
  if (sqlite3_step(s) == SQLITE_ROW) {
    o.id = (ident){ sqlite3_column_int(s, 0) };

    o.name_length = sqlite3_column_bytes(s, 1);
    o.name = malloc(o.name_length);
    memcpy(o.name, sqlite3_column_text(s, 1), o.name_length);

    o.key_length = sqlite3_column_bytes(s, 2);
    o.key = malloc(o.key_length);
    memcpy(o.key, sqlite3_column_blob(s, 2), o.key_length);
  } else {
    o.id = (ident){ 0 };
  }

  sqlite3_finalize(s);
  return o;
}

void set_ident(ident_info info) {
  char* q = "insert or replace into idents values (?, ?, ?)";
  sqlite3_stmt* s;

  sqlite3_prepare(state.db, q, strlen(q), &s, NULL);
  sqlite3_bind_int(s, 1, info.id.i);
  sqlite3_bind_text(s, 2, info.name, info.name_length, SQLITE_TRANSIENT);
  sqlite3_bind_blob(s, 3, info.key, info.key_length, SQLITE_TRANSIENT);

  sqlite3_step(s);
  sqlite3_finalize(s);
}

void free_ident_info(ident_info i) {
  free(i.name);
  free(i.key);
}

bool validate_ident(ident id, signature sigvec, byte* ciphertext, counter cyphertext_length) {
  if (cyphertext_length != 16) return false;

  byte expected[16];
  for (counter i = 0; i < 16; i++) {
    expected[i] = (i * 16) + 15;
  }

  ident_info k = get_ident(id);
  byte* decrypted = pk_decrypt(ciphertext, 16, sigvec, k.key);
  bool r = true;

  for (counter i = 0; i < 16; i++) {
    if (decrypted[i] != expected[i]) r = false;
  }

  free(decrypted);
  return r;
}
