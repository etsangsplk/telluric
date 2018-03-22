#include "route.h"

#include <stdlib.h>
#include <string.h>

// simple qsort on vector norm

counter part(neighbour_embedding* s, counter l, counter r) {
  double pivot = norm(s[l].embedding);
  counter i = l - 1;
  counter j = r + 1;
  neighbour_embedding swap;
  while (true) {
    while(norm(s[i].embedding) < pivot) i += 1;
    while(norm(s[j].embedding) > pivot) j -= 1;

    if (i >= j) return j;

    swap = s[i];
    s[i] = s[j];
    s[j] = swap;
  }
}

void sort_nodes(neighbour_embedding* s, counter l, counter r) {
  if (l >= r) return;

  counter p = part(s, l, r);
  sort_nodes(s, l, p-1);
  sort_nodes(s, p+1, r);
}

neighbour_embedding* route_nodes(host_address h, counter n) {
  route_embedding* r = NULL;
  for (counter i = 0; i < state.routes.routes_count; i++) {
    if (strcmp(state.routes.routes[i].host.name, h.name) != 0)
      continue;
    if (state.routes.routes[i].host.id.i != h.id.i)
      continue;

    r = &state.routes.routes[i];
    break;
  }

  if (r == NULL) return NULL; // indicate explore is needed
  if (n > state.routes.nodes_count) n = state.routes.nodes_count;

  neighbour_embedding* sort = malloc(sizeof(neighbour_embedding) * state.routes.nodes_count);
  memcpy(sort, state.routes.nodes, sizeof(neighbour_embedding) * state.routes.nodes_count);

  sort_nodes(sort, 0, state.routes.nodes_count);

  return realloc(sort, sizeof(neighbour_embedding) * n);
}
