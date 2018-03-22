#include "session.h"

#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#include "state.h"

session_token gen_token(session_namespace n) {
  return (session_token){rand()};
}

session_token open_client_session(ipv6_address from, host_address to) {
  session_token t = gen_token(client_session);

  // try again if collision
  if (state.net.sessions[t.b] != NULL)
    return open_client_session(from, to);

  state.net.sessions[t.b] = malloc(sizeof(session));
  state.net.sessions[t.b]->origin = from;
  state.net.sessions[t.b]->host = to;
  state.net.sessions[t.b]->phase = pending;
  state.net.sessions[t.b]->token = t;

  // route here
}
void close_client_session(session_token s) {
  free(state.net.sessions[s.b]);
}

void open_neighbour_session(session_token t, ipv6_address from, ipv6_address to) {
  // fail on collision
  if (state.net.sessions[t.b] != NULL) return;

  state.net.sessions[t.b] = malloc(sizeof(session));
  state.net.sessions[t.b]->origin = from;
  state.net.sessions[t.b]->destination = to;
  state.net.sessions[t.b]->token = t;
}

void close_neighbour_session(session_token s) {
  free(state.net.sessions[s.b]);
}

////////////////////////////////////////////////////////////////////////////////

void sessions_init() {
  state.net.sessions = malloc(sizeof(void*) * (2<<23)); // memory is cheap lol
  // todo make this not allocate 128mb for sparse array
}

void sessions_free() {
  free(state.net.sessions);
}
