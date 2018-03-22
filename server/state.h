#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sqlite3.h>

#include "types.h"
#include "error.h"
#include "opt.h"
#include "network.h"
#include "vector3.h"
#include "crypto.h"
#include "protocol.h"
#include "job.h"

////////////////////////////////////////////////////////////////////////////////
// routing knowledge

typedef struct {
  host_address host;
  double heat;
  vector3 embedding;
} route_embedding;

typedef struct {
  ipv6_address addr;
  double heat;
  vector3 embedding;
} neighbour_embedding;

typedef struct {
  host_address host;
  ipv6_address neighbour;
} route_association;

typedef struct {
  route_embedding* routes;
  counter routes_count;

  neighbour_embedding* nodes;
  counter nodes_count;

  route_association* associations;
  counter associations_count;
} routing_knowledge;

////////////////////////////////////////////////////////////////////////////////
// client-interface connections

typedef struct {
  cipher_params crypto;
} client_params;

typedef struct {
  file fd;
  ipv6_address addr;

  client_params params;

  session_ref* sessions;
  counter sessions_count;
} client_connection;

typedef struct {
  client_connection* connections;
  counter connections_count;
} client_connection_pool;

typedef struct {
  struct random_data data;
  char* state;
  int seed;
} entropy_pool;

typedef struct {
  enum { pre_negotiate, negotiated, streaming } phase;
  cipher_key key;
  entropy_pool entropy;
} cipher_state;

////////////////////////////////////////////////////////////////////////////////
// internode connections

typedef struct {
  file fd;
  ipv6_address addr;

  session_ref* sessions;
  counter sessions_count;
} neighbour_connection;

typedef struct {
  neighbour_connection* connections;
  counter connections_count;
} neighbour_connection_pool;

////////////////////////////////////////////////////////////////////////////////
// network operation

typedef struct {
  file fd;
  ipv6_address addr;
} pending_connection;

typedef struct {
  enum {pending, active, closed} phase;
  session_token token;
  ipv6_address origin;
  ipv6_address destination;
  host_address host; // populated only for iface connections
  cipher_state crypto;
} session;

typedef struct {
  file listen_socket;
  file stream_socket;

  pthread_t listen_thread;
  pthread_t cycle_thread;
  pthread_t pending_thread;

  bool listening;

  session** sessions;
  counter sessions_count;

  pending_connection* pending;
  counter pending_count;
} network_state;

////////////////////////////////////////////////////////////////////////////////
// job runner

typedef struct {
  file fifo;

  pthread_mutex_t readlock;
  pthread_mutex_t interrupt;

  pthread_t* threads;
  counter threads_count;

  scheduled_job* scheduled;
  counter scheduled_count;
  counter scheduled_max;

  bool working;
} job_queue;

////////////////////////////////////////////////////////////////////////////////
// accumulation object

typedef struct {
  opts config;

  routing_knowledge routes;
  client_connection_pool clients;
  neighbour_connection_pool neighbours;
  network_state net;

  job_queue queue;

  sqlite3* db;

  bool exiting;
} node_state;

node_state state;

////////////////////////////////////////////////////////////////////////////////

void state_init(opts o);
