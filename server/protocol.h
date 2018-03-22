// protocol definition & abstraction over networking

#pragma once

#include <stdint.h>
#include <netinet/in.h>

#include "network.h"
#include "time.h"
#include "session.h"

#include "vector4.h"
#include "matrix4.h"

////////////////////////////////////////////////////////////////////////////////
// basic types

typedef netvec signature;
typedef matrix4 key;

////////////////////////////////////////////////////////////////////////////////
// proto byte values

typedef enum { client, iface, internode } proto_role;

typedef enum {
  n_stream,
  n_packet,
  n_route,
  n_explore,
  n_route_response,
  n_broadcast,
  n_identity,
  n_poll_broadcast,
  n_poll_identity,
  n_detrain
} internode_message_type;

typedef enum {
  i_stream,
  i_packet,
  i_iface_response,
  i_connect_response,
  i_negotiate_response,
  i_identify_response,
  i_identity_response, // publish / revision success
  i_broadcast,
  i_identity
} iface_message_type;

typedef enum {
  c_stream,
  c_packet,
  c_iface,
  c_connect,
  c_negotiate,
  c_identify,
  c_identity, // publish or revise
  c_broadcast,
  c_poll_broadcast,
  c_poll_identity
} client_message_type;

static const size_t message_error_type = 255;

typedef union {
  internode_message_type node;
  iface_message_type iface;
  client_message_type client;
} message_type;

typedef union {
  byte b;
  struct {
    byte       type : 6;
    proto_role role : 2;
  };
} proto;

////////////////////////////////////////////////////////////////////////////////
// message layouts
// (all messages are size_t 528)

typedef struct {
  proto proto;
  byte content[527];
} message_packed;

typedef struct {
  proto proto;
  session_token session;
  byte content[524];
} message_packet;

typedef struct {
  proto proto;
  session_token session;
  byte content[524];
} message_stream;

typedef struct {
  proto proto;
  ident id;
  signature sigvec;
  timestamp time;
  telluric_char message[128];
} message_broadcast;

typedef struct {
  proto proto;
  timestamp req;
} message_poll_broadcasts;

typedef struct {
  byte proto;
  ident claim;
  signature sigvec;
  byte ciphertext[16];
  byte padding[496];
} message_identify;

typedef struct {
  byte proto;
  ident hostid;
  byte time_limit[2];
  byte length[2];
  byte hostname[520];
} message_client_connect_request;

typedef struct {
  byte proto;
  session_token session;
  byte length;
  byte error[523];
} message_client_connect_response;

typedef struct {
  byte proto;
  ident id;
} message_poll_identities;

static const size_t message_size = 528;

typedef union {
  message_packed packed;
  message_packet packet;
  message_stream stream;
  message_broadcast broadcast;
  message_poll_broadcasts poll_proadcasts;
  message_identify identify;
  message_client_connect_request client_connect_request;
  message_client_connect_response client_connect_response;
  message_poll_identities poll_identities;
} message;

////////////////////////////////////////////////////////////////////////////////
// functions

void dispatch(proto p, ipv6_address o, message_packed msg);

