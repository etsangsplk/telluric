#include "file.h"

#include <string.h>
#include <stdio.h>

byte set_flags(meta_section m) {
  byte o = 0;

  for (int i = 0; i < m.count; i++) {
    if (strcmp(m.keys[i], "display") == 0 && strcmp(m.vals[i], "true") == 0)
      o |= 0b00000001;
    if (strcmp(m.keys[i], "audio") == 0 && strcmp(m.vals[i], "true") == 0)
      o |= 0b00000010;
    if (strcmp(m.keys[i], "network") == 0 && strcmp(m.vals[i], "true") == 0)
      o |= 0b00000100;
    if (strcmp(m.keys[i], "disk") == 0 && strcmp(m.vals[i], "true") == 0)
      o |= 0b00001000;
  }

  return o;
}

void set_storage(byte* count, offset_index** indices, storage_section** section,
                 data_domains dd, data_object* dats, int dats_len) {
  *count = dats_len;
  *indices = calloc(*count, sizeof(**indices));
  *section = calloc(*count, sizeof(**section));

  int offset = 0;

  for (int i = 0; i < *count; i++) {
    (*indices)[i].id = dats[i].id;
    (*indices)[i].offset = offset;

    (*section)[i].flags = dats[i].flags;
    (*section)[i].len = dats[i].length;
    (*section)[i].data = dats[i].content;
    offset += sizeof(uint32_t) + 1 + dats[i].length;
  }
}

byte hdr2b(header h) {
  return (h.has_return << 7) | h.length;
}

void set_code(byte* indices_len, offset_index** indices, program_section** section,
              object* objs, int objs_len) {
  *indices_len = objs_len;
  *indices = calloc(*indices_len, sizeof(**indices));
  *section = calloc(*indices_len, sizeof(**section));

  int offset = 0;

  for (int i = 0; i < objs_len; i++) {
    (*indices)[i].id = objs[i].ev;
    (*indices)[i].offset = offset;

    byte* instrs = NULL;
    int instrs_len = 0;

    for (int j = 0; j < objs[i].instrs_length; j++) {
      int newlen = instrs_len + objs[i].instrs[j].hdr.length + 1;

      instrs = realloc(instrs, newlen);
      instrs[instrs_len] = hdr2b(objs[i].instrs[j].hdr);
      memcpy(&instrs[instrs_len+1], objs[i].instrs[j].bytes, objs[i].instrs[j].hdr.length);

      instrs_len = newlen;
    }

    (*section)[i].instrs = instrs;
    (*section)[i].len = instrs_len;
    offset += sizeof(uint32_t) + instrs_len;
  }
}

program_file compile_file(data_domains d, meta_section m, object* objs, int objs_len, data_object* dats, int dats_len) {
  program_file o;

  printf("assembling file\n");

  o.magic = telluric_filesig;
  o.flags = set_flags(m);
  set_storage(&o.storage_count, &o.storage_indices, &o.storage, d, dats, dats_len);
  set_code(&o.code_indices_len, &o.code_indices, &o.code, objs, objs_len);

  return o;
}

// flatten to binary, fill in offset indices
byte* collapse_file(program_file p, int* len) {

  printf("flattening file\n");

  int header_len = sizeof(uint64_t) + 1;
  byte* header = calloc(header_len, sizeof(*header));
  memcpy(header, &p.magic, sizeof(uint64_t));
  header[sizeof(uint64_t)] = p.flags;

  int storage_header_len = 1 + (sizeof(offset_index) * p.storage_count);
  byte* storage_header = calloc(storage_header_len, sizeof(*storage_header));

  storage_header[0] = p.storage_count;
  int b = 1;
  for (int i = 0; i < p.storage_count; i++) {
    storage_header[b] = p.storage_indices[i].id;
    memcpy(&storage_header[b+1], &p.storage_indices[i].offset, sizeof(uint32_t));

    b += 1 + sizeof(uint32_t);
  }

  int storage_blob_len = 0;
  byte* storage_blob = NULL;

  for (int i = 0; i < p.storage_count; i++) {
    storage_blob = realloc(storage_blob, storage_blob_len + p.storage[i].len + sizeof(uint32_t) + 1);
    storage_blob[storage_blob_len] = p.storage[i].flags;
    memcpy(&storage_blob[storage_blob_len+1], &p.storage[i].len, sizeof(uint32_t));
    memcpy(&storage_blob[storage_blob_len+1+sizeof(uint32_t)], p.storage[i].data, p.storage[i].len);
    storage_blob_len += p.storage[i].len + sizeof(uint32_t) + 1;
  }

  int code_header_len = 1 + (sizeof(offset_index) * p.code_indices_len);
  byte* code_header = calloc(code_header_len, sizeof(*code_header));

  code_header[0] = p.storage_count;
  b = 1;
  for (int i = 0; i < p.code_indices_len; i++) {
    code_header[b] = p.code_indices[i].id;
    memcpy(&code_header[b+1], &p.code_indices[i].offset, sizeof(uint32_t));

    b += 1 + sizeof(uint32_t);
  }

  int code_blob_len = 0;
  byte* code_blob = NULL;

  for (int i = 0; i < p.code_indices_len; i++) {
    code_blob = realloc(code_blob, code_blob_len + p.code[i].len + sizeof(uint32_t));
    memcpy(&code_blob[code_blob_len], &p.code[i].len, sizeof(uint32_t));
    memcpy(&code_blob[code_blob_len+sizeof(uint32_t)], p.code[i].instrs, p.code[i].len);
    code_blob_len += p.code[i].len + sizeof(uint32_t);
  }

  byte* out =
    calloc(header_len + storage_header_len + storage_blob_len + code_header_len + code_blob_len, sizeof(*out));

  *len = header_len;
  memcpy(out, header, *len);

  memcpy(&out[*len], storage_header, storage_header_len);
  *len += storage_header_len;

  memcpy(&out[*len], storage_blob, storage_blob_len);
  *len += storage_blob_len;

  memcpy(&out[*len], code_header, code_header_len);
  *len += code_header_len;

  memcpy(&out[*len], code_blob, code_blob_len);
  *len += code_header_len;

  free(header);
  free(storage_header);
  free(storage_blob);
  free(code_header);
  free(code_blob);

  return out;
}
