#include "identify.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

data_domains append_domains(data_idents d) {
  data_domains o;
  o.count = 5;
  o.domains = calloc(o.count, sizeof(*o.domains));
  o.idents = calloc(o.count, sizeof(*o.idents));

  o.domains[0] = "local";
  o.idents[0] = d;
  o.domains[1] = "video";
  o.idents[1] = video_idents;
  o.domains[2] = "audio";
  o.idents[2] = audio_idents;
  o.domains[3] = "network";
  o.idents[3] = network_idents;
  o.domains[4] = "disk";
  o.idents[4] = disk_idents;

  return o;
}

data_domains identify_data(data_section d) {

  printf("identifying data\n");
  printf("defcount: %d\n", d.defs_count);

  if (d.defs_count > 64) {
    printf("too many data definitions\n");
    exit(1);
  }

  data_idents o;
  o.count = 0;
  o.ids = calloc(d.defs_count, sizeof(*o.ids));
  o.labels = calloc(d.defs_count, sizeof(*o.labels));

  for (int i = 0; i < d.defs_count; i++) {
    for (int j = 0; j < o.count; j++) {
      if (strcmp(d.defs[i].name, o.labels[j]) == 0) {
        printf("duplicate data definition %s, refusing to compile", d.defs[i].name);
        exit(1);
      }
    }

    printf("assigning id %d to def %s\n", o.count, d.defs[i].name);
    o.ids[o.count] = o.count; // extraneous?
    o.labels[o.count] = calloc(sizeof(char), strlen(d.defs[i].name) + 1);
    memcpy(o.labels[o.count], d.defs[i].name, strlen(d.defs[i].name));

    o.count++;
  }

  printf("local domain count: %d\n", o.count);

  return append_domains(o);
}

int get_label_id(data_domains d, char* c, int len) {
  for (int i = 0; i < d.idents[0].count; i++) {
    if (strlen(d.idents[0].labels[i]) > (unsigned) len) continue;

    if (strncmp(c, d.idents[0].labels[i], len) == 0)
      return d.idents[0].ids[i];
  }

  return -1;
}

int get_ref_id(data_domains d, char* c, int len) {
  int domain = -1;

  printf("retreiving id for label %s\n", c);

  int n = 0;
  while (n < len && c[n] != ':') n++;

  if (n == 0) {
    printf("local domain specified\n");
    domain = 0; // local
  }
  else
    for (int i = 0; i < d.count; i++)
      if (strncmp(c, d.domains[i], n))
        domain = i;

  if (domain == -1) {
    printf("invalid domain in label %s\n", c);
    exit(1);
  }

  for (int i = 0; i < d.idents[domain].count; i++) {
    if (strlen(d.idents[domain].labels[i]) > (unsigned) len-n) continue;

    if (strncmp(&c[n+1], d.idents[domain].labels[i], len-n) == 0)
      return d.idents[domain].ids[i];
  }

  return -1;
}
