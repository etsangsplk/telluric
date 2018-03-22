#include "internode.h"

#include <stdlib.h>
#include <string.h>

#include "state.h"
#include "session.h"
#include "network.h"

void forward_packet(ipv6_address o, message p) {
  if (p.packed.proto.type == 0 && p.packed.proto.role == internode) {
    // stream
  } else if (p.packed.proto.type == 1 && p.packed.proto.role == internode) {
    // packet
    session_ref s = find_session(p.packet.session, o);
    if (s == -1) return;

    session* session = &state.net.sessions[s];

    neighbour_connection_ref n;

    // todo

    neighbour_send_tcp(n, &p, message_size);
  }
}

void forward_stream(ipv6_address o, message p) {
  
}

