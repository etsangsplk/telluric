#pragma once

#include "reduce.h"

typedef struct {
  int* ids;
  char** labels;
  int count;
} data_idents;

typedef struct {
  char** domains;
  data_idents* idents;
  int count;
} data_domains;

static const data_idents video_idents = {
  .ids = (int[]) {
    64,
    65,
    66,
    67,
    68,
    69
  },
  .labels = (char*[]) {
    "char",
    "pixel",
    "line",
    "blit",
    "rectblit",
    "read"
  },
  .count = 6
};

static const data_idents audio_idents = {
  .ids = (int[]) {
    80,
    81
  },
  .labels = (char*[]) {
    "play",
    "listen",
  },
  .count = 2
};

static const data_idents network_idents = {
  .ids = (int[]) {
    96,
    97,
    98,
    99,
    100,
    101,
    102,
    103,
    104
  },
  .labels = (char*[]) {
    "interface",
    "negotiate",
    "accept",
    "transfer",
    "deinterface",
    "connect",
    "packet",
    "stream",
    "disconnect"
  },
  .count = 9
};

static const data_idents disk_idents = {
  .ids = (int[]) {
    112,
    113
  },
  .labels = (char*[]) {
    "copy",
    "clear",
  },
  .count = 2
};

data_domains identify_data(data_section d);
int get_label_id(data_domains d, char* c, int len);
int get_ref_id(data_domains d, char* c, int len);
