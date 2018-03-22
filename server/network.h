#pragma once

#include <stdint.h>

#include "types.h"

typedef struct in6_addr ipv6_address;

void net_init();
void net_close();

void neighbour_send_tcp(neighbour_connection_ref d, void* m, counter len);
