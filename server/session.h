#pragma once

#include "types.h"
#include "network.h"

typedef enum {client_session, node_session} session_namespace;

typedef struct {
  // session_namespace n;
  unsigned int b : 24;
} session_token;

void sessions_init();
void sessions_free();

session_token open_client_session(ipv6_address from, host_address to);
void close_client_session(session_token s);

void open_neighbour_session(session_token t, ipv6_address from, ipv6_address to);
void close_neighbour_session(session_token s);
