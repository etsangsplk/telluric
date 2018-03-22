// api to server endpoints
// mocked here for planning purposes

#include <stdint.h>

#include "network.h"

struct attach_details {
  uint8_t* name;
  uint64_t name_length;
};

struct accept_connection_details {
  ipv6_address client;
};

typedef struct in_api_message {
  enum {
    attach,
    unattach,
    estimate_loss,
    estimate_competition,
    accept_connection
  } action;
  union {
    struct attach_details attach;
    struct accept_connection_details accept_connection;
  };
} in_api_message;

typedef struct out_api_message {
  enum {
    attach_success,
    attach_failure,
    heartbeat,
    connection_request,
    loss_estimate,
    competition_estimate
  } message;
} out_api_message;
