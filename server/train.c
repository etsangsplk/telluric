// route training rule

#include "train.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "vector3.h"

vector3 disperse(counter i) {
  vector3 v = state.routes.routes[i].embedding;

  // calculate angular difference from x+
  double a = atan2(normalize(v).y, normalize(v).x);
  double da = a;

  for (counter i = 0; i < state.routes.routes_count; i++) {
    vector3 o = normalize(state.routes.routes[i].embedding);

    // calculate angular difference from repeller
    double oa = atan2(o.y, o.x);
    double diff = oa - da;

    if (diff > 0) {
      // repel, ccw
      da += 1 / pow(diff, 2);
    } else {
      // repel, cw
      da -= 1 / pow(diff, 2);
    }
  }

  // calculate new angular position from displacement
  da = da - a;
  return (vector3){v.x * cos(da) - v.y * sin(da), v.x * sin(da) + v.y * cos(da), v.z};
}

vector3 apply_rotator(vector3 v) {
  // derive attractor direction from vector
  vector3 a = normalize((vector3){v.x, v.y, 0.0f});

  // derive rotation axis
  vector3 u = {-a.y, a.x, 0.0f};

  // rescale attractor to torus radius
  a = multiply(a, rotator_radius);

  // get normal
  vector3 n = normalize(subtract(v,a));

  // rotate normal 90 degrees along axis to find tangent
  // simplified rodrigues rotation
  vector3 r = add(cross(u, n), multiply(u, dot(u, n)));

  // scale tangent
  r = multiply(r, state.config.rotator_speed);

  // displace
  return add(v, r);
}

void train_routes() {
  for (counter i = 0; i < state.routes.routes_count; i++) {
    // disperse along rotator circle
    state.routes.routes[i].embedding = disperse(i);
    // apply rotator
    state.routes.routes[i].embedding =
      apply_rotator(state.routes.routes[i].embedding);
    // cool
    state.routes.routes[i].heat *= state.config.cooling_factor;
  }

  // cull cold
  route_embedding* rs =
    malloc(sizeof(route_embedding) * state.routes.routes_count);
  counter pos = 0;
  for (counter i = 0; i < state.routes.routes_count; i++) {
    if (state.routes.routes[i].heat > cooling_threshold) {
      rs[pos] = state.routes.routes[i];
      pos += 1;
    }
  }
  free(state.routes.routes);
  state.routes.routes = realloc(rs, sizeof(route_embedding) * pos);
  state.routes.routes_count = pos;

  // gravitate node embeddings
  // warmer routes move faster
  for (counter i = 0; i < state.routes.nodes_count; i++) {
    for (counter j = 0; j < state.routes.routes_count; j++) {
      vector3 v = state.routes.nodes[j].embedding;
      vector3 d = subtract(state.routes.routes[i].embedding, v);
      // inverse square gravitation considering heats of both objects
      // more active routes are stronger attractors
      double p =
        state.routes.routes[i].heat + state.routes.nodes[j].heat / pow(norm(d), 2);
      d = multiply(normalize(d), p);
      state.routes.nodes[j].embedding = add(v, d);
    }

    // cool
    state.routes.nodes[i].heat *= state.config.cooling_factor;
  }

  // cull cold
  neighbour_embedding* ns =
    malloc(sizeof(neighbour_embedding) * state.routes.nodes_count);
  pos = 0;
  for (counter i = 0; i < state.routes.nodes_count; i++) {
    if (state.routes.nodes[i].heat > cooling_threshold) {
      ns[pos] = state.routes.nodes[i];
      pos += 1;
    }
  }
  free(state.routes.nodes);
  state.routes.nodes = realloc(ns, sizeof(neighbour_embedding) * pos);
  state.routes.nodes_count = pos;

  // rebuild associations
  free(state.routes.associations);
  state.routes.associations_count = 0;

  state.routes.associations =
    malloc(sizeof(route_association) * state.routes.routes_count);

  for (counter i = 0; i < state.routes.routes_count; i++) {
    for (counter j = 0; i < state.routes.nodes_count; i++) {
      vector3 d =
        subtract(state.routes.routes[i].embedding, state.routes.nodes[j].embedding);
      if (norm(d) < state.config.association_threshold) {
        state.routes.associations[state.routes.associations_count].host =
          state.routes.routes[i].host;
        state.routes.associations[state.routes.associations_count].neighbour =
          state.routes.nodes[i].addr;
        state.routes.associations_count += 1;
      }
    }
  }

  state.routes.associations =
    realloc(state.routes.associations, sizeof(route_association) * state.routes.associations_count);
}
