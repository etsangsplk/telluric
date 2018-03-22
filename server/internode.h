// internode role state machine

#pragma once

#include "protocol.h"

////////////////////////////////////////////////////////////////////////////////
// in

void forward_packet(ipv6_address o, message p);

void forward_broadcast();
void replay_broadcasts();

void forward_identity();
void share_identity();

void forward_explore();
void forward_route();

void route_response();
void detrain();

////////////////////////////////////////////////////////////////////////////////
// out

