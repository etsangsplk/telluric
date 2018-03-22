#pragma once

// todo: merge train.c into this file

#include "state.h"
#include "protocol.h"

// return array of neighbours relevant to route
neighbour_embedding* route_nodes(host_address h, counter n);
