#include "protocol.h"

#include "internode.h"

void dispatch(proto p, ipv6_address o, message_packed m) {
  if (p.role == client) {
    switch(p.type) {}
  }

  if (p.role == internode) {
    switch(p.type) {
    case n_stream:
      forward_packet(o, m);
      break;
    case n_packet:
      forward_packet(o, m);
      break;
    case n_route:
      forward_route();
      break;
    case n_route_response:
      route_response();
      break;
    case n_broadcast:
      forward_broadcast();
      break;
    case n_identity:
      forward_identity();
      break;
    case n_poll_broadcast:
      replay_broadcasts();
      break;
    case n_poll_identity:
      share_identity();
      break;
    case n_detrain:
      detrain();
      break;
    }
  }
}
