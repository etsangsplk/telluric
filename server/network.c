#include "network.h"

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <poll.h>
#include <fcntl.h>

#include "util.h"
#include "state.h"
#include "job.h"
#include "protocol.h"

// select for tcp input on all open connections
// dispatch to job queues
void* stream_cycle(void* n) {
  struct sockaddr_in6 addr;
  unsigned int addr_len;
  job stream_job;

  while (state.net.listening) {
    recvfrom(state.net.stream_socket, &stream_job.packet,
             message_size, 0, (struct sockaddr*)&addr, &addr_len);

    switch (stream_job.packet.proto.role) {
    case client:
      stream_job.type = client_packet;
      break;
    case internode:
      stream_job.type = neighbour_packet;
      break;
    case iface:
      // errant
      continue;
    }

    if (addr_len != sizeof(struct sockaddr_in6)) continue;

    stream_job.origin = addr.sin6_addr;
    job_add(stream_job);
  }

  return n;
}

// neighbour session cycle
void* neighbour_cycle(void* n) {
  job packet_job;
  packet_job.type = neighbour_packet;

  while (state.net.listening) {

    // update fd pool
    // note: this should be managed by the listener in the future
    struct pollfd fds[state.neighbours.connections_count];
    for (counter i = 0; i < state.neighbours.connections_count; i++) {
      fds[i].fd = state.neighbours.connections[i].fd;
      fds[i].events = POLLIN;
    }

    if (poll(fds, state.neighbours.connections_count, 100)) {
      for (counter i = 0; i < state.neighbours.connections_count; i++) {
        if (fds[i].revents & POLLIN) {
          recv(fds[i].fd, &packet_job.packet, message_size, 0);

          packet_job.origin = state.neighbours.connections[i].addr;
          job_add(packet_job);
        }
      }
    }
  }

  return n;
}

void new_neighbour(file fd, ipv6_address incoming, message_packed msg) {
  if(state.neighbours.connections_count >= state.config.neighbour_limit) return;

  neighbour_connection* conn =
    &state.neighbours.connections[state.neighbours.connections_count];

  state.neighbours.connections_count += 1;

  conn->fd = fd;
  conn->addr = incoming;

  job j;
  j.type = neighbour_packet;
  j.packet = msg;

  job_add(j);
}

void new_client(file fd, ipv6_address incoming, message_packed msg) {
  if (state.clients.connections_count >= state.config.client_limit) return;

  client_connection* conn =
    &state.clients.connections[state.clients.connections_count];
  state.clients.connections_count += 1;

  conn->fd = fd;
  conn->addr = incoming;

  job j;
  j.type = client_packet;
  j.packet = msg;

  job_add(j);
}

void* pending_cycle() {
  message_packed unknown;

  while (state.net.listening) {
    struct pollfd fds[state.net.pending_count];
    for (counter i = 0; i < state.net.pending_count; i++) {
      fds[i].fd = state.net.pending[i].fd;
      fds[i].events = POLLIN;
    }

    if (poll(fds, state.net.pending_count, 100)) {
      for (counter i = 0; i < state.net.pending_count; i++) {
        if (fds[i].revents & POLLIN) {
          recv(fds[i].fd, &unknown,
               sizeof(message_packed), 0);

          switch (unknown.proto.role) {
          case internode:
            new_neighbour(state.net.pending[i].fd, state.net.pending[i].addr, unknown);
            break;
          case client:
            new_client(state.net.pending[i].fd, state.net.pending[i].addr, unknown);
            break;
          case iface:
            close(state.net.pending[i].fd);
            break;
          }

          state.net.pending_count -= 1;
        }
      }
    }
  }
}

// incoming tcp dispatch thread, kept out of job queue
void* listener(void* n){
  // bind tcp
  state.net.listen_socket = socket(PF_INET6, SOCK_STREAM, 0);

  struct sockaddr_in6 tcp_addr = {
    .sin6_family = AF_INET6,
    .sin6_port = htonl(1600),
    .sin6_flowinfo = 255,
    .sin6_addr = in6addr_any
  };

  bind(state.net.listen_socket, (const void*)&tcp_addr, sizeof(tcp_addr));

  listen(state.net.listen_socket, 10);

  // udp
  // note: can be improved with kernel-side filtering to ignore nonconnected
  state.net.stream_socket = socket(PF_INET6, SOCK_DGRAM, 0);
  fcntl(state.net.stream_socket, F_SETFL,
        fcntl(state.net.stream_socket, F_GETFL, 0) | O_NONBLOCK);

  struct sockaddr_in6 udp_addr = {
    .sin6_family = AF_INET6,
    .sin6_port = htonl(1600),
    .sin6_flowinfo = 255,
    .sin6_addr = in6addr_any
  };

  bind(state.net.stream_socket, (const void*)&udp_addr, sizeof(udp_addr));

  // spawn cyclers
  pthread_create(&state.net.cycle_thread, NULL, &neighbour_cycle, NULL);
  pthread_create(&state.net.pending_thread, NULL, &pending_cycle, NULL);

  struct sockaddr_in6 incoming;
  socklen_t incoming_len;

  while (state.net.listening) {

    if (wait_read(state.net.listen_socket, 10)) {
      int fd =
        accept(state.net.listen_socket,
               (struct sockaddr*) &incoming,
               &incoming_len);

      if (incoming_len != sizeof(struct sockaddr_in6)) {
        close(fd);
        continue;
      }

      if (state.net.pending_count >= state.config.pending_limit) {
        close(fd);
        continue;
      }

      state.net.pending[state.net.pending_count].fd = fd;
      state.net.pending[state.net.pending_count].addr = incoming.sin6_addr;
      state.net.pending_count += 1;
    }
  }

  return n;
}

void neighbour_send_tcp(neighbour_connection_ref d, void* m, counter len) {
  send(state.neighbours.connections[d].fd, m, len, 0);
}

void neighbour_send_udp(neighbour_connection_ref d, void* m, counter len) {}

void net_init() {
  // alloc connection pools
  state.neighbours.connections =
    malloc(sizeof(neighbour_connection) * state.config.neighbour_limit);
  state.clients.connections =
    malloc(sizeof(client_connection) * state.config.client_limit);
  state.net.sessions =
    malloc(sizeof(session) * state.config.session_limit);

  state.net.listening = true;
  pthread_create(&state.net.listen_thread, NULL, &listener, NULL);
}

void net_close() {
  state.net.listening = false;
  pthread_join(state.net.cycle_thread, NULL);
  pthread_join(state.net.listen_thread, NULL);
}
