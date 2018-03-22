#include "crypto.h"

#include <stdlib.h>
#include <math.h>

#include "state.h"

void transform_trailing_vec(byte* m, counter cur, counter excess, matrix4 mat) {
  byte* mt = m + cur;

  vector4 v;
  v.x = b2f(m[0]);
  v.y = excess == 2 ? b2f(m[1]) : (fp32){0};
  v.z = excess == 3 ? b2f(m[2]) : (fp32){0};
  v.w = (fp32){0};

  bytevec b = serialize_vector(gemv(mat, v));

  mt[0] = b.x;
  if (excess == 2) mt[1] = b.y;
  if (excess == 3) mt[2] = b.z;
}

void update_key(session_ref s, vector4 v) {
  cipher_key* key = &state.net.sessions[s].crypto.key;

  key->mats[key->mats_cur / 4].m[key->mats_cur % 4][0] = v.x;
  key->mats[key->mats_cur / 4].m[key->mats_cur % 4][1] = v.y;
  key->mats[key->mats_cur / 4].m[key->mats_cur % 4][2] = v.z;
  key->mats[key->mats_cur / 4].m[key->mats_cur % 4][3] = v.w;

  if (key->mats_cur > key->mats_count * 4)
    key->mats_cur = 0;
  else
    key->mats_cur += 1;

  // regen cmat when new mat is finished
  if (key->mats_cur % 4 == 0) {
    matrix4 cmat = key->mats[0];
    for (counter i = 1; i < key->mats_count; i++)
      cmat = gemm(cmat, key->mats[i]);
    key->cmat = cmat;
  }
}

// sum and wrap the message vectors to get new key vec
// discards trailing, fairly weak hash
// very weak when operating on word-aligned telluric chars
vector4 sumvec(byte* m, counter len) {
  vector4 v;

  for (counter i = 0; i < len / 4; i++)
    v.x.f = fmodf(v.x.f + b2f(m[i*4]).f, 2);
  v.x.f -= 1.0f;

  for (counter i = 0; i < len / 4; i++)
    v.y.f = fmodf(v.y.f + b2f(m[i*4+1]).f, 2);
  v.y.f -= 1.0f;

  for (counter i = 0; i < len / 4; i++)
    v.z.f = fmodf(v.z.f + b2f(m[i*4+2]).f, 2);
  v.z.f -= 1.0f;

  for (counter i = 0; i < len / 4; i++)
    v.w.f = fmodf(v.w.f + b2f(m[i*4+3]).f, 2);
  v.w.f -= 1.0f;

  return v;
}

byte* symmetric_transform(byte* m, counter len, matrix4 mat) {
  byte* o = malloc(len);
  bytevec* ov = (bytevec*) o;

  for (counter i = 0; i < len / 4; i++) {
    ov[i] = serialize_vector(gemv(mat, deserialize_vector(*(bytevec*)(m+i*4))));
  }

  counter n = len % 4;
  if (n != 0) transform_trailing_vec(o, len/4, n, mat);

  return o;
}

////////////////////////////////////////////////////////////////////////////////
// symmetric cipher

// populate keymats from initial session entropy pool
// initial cmat is trivially predictable
void prime_cipher(session_ref s) {
  cipher_state* c = &state.net.sessions[s].crypto;

  c->entropy.seed = state.net.sessions[s].token.b;
  c->entropy.state = malloc(256);
  initstate_r(c->entropy.seed, c->entropy.state, 256, &c->entropy.data);

  fp32 f;
  for (counter m = 0; m < c->key.mats_count; m++) {
    for (counter i = 0; i < 16; i++) {
      random_r(&c->entropy.data, &f.u);
      // this could be simpler pointer arithmetic
      c->key.mats[m].m[i/4][i%4] = f;
    }
  }
}

byte* encrypt_stream(session_ref s, byte* m, counter len) {
  return symmetric_transform(m, len, state.net.sessions[s].crypto.key.cmat);
}

byte* decrypt_stream(session_ref s, byte* m, counter len) {
  return symmetric_transform(m, len, invert(state.net.sessions[s].crypto.key.cmat));
}

byte* encrypt_packet(session_ref s, byte* m, counter len) {
  update_key(s, sumvec(m, len));

  return symmetric_transform(m, len, state.net.sessions[s].crypto.key.cmat);
}

byte* decrypt_packet(session_ref s, byte* m, counter len) {
  byte* d =
    symmetric_transform(m, len, invert(state.net.sessions[s].crypto.key.cmat));

  update_key(s, sumvec(d, len));

  return d;
}

// add 4 keyvecs from entropy pool
void rekey_cipher(session_ref s) {
  struct random_data* r =
    &state.net.sessions[s].crypto.entropy.data;

  vector4 v;
  for (counter i = 0; i < 4; i++) {
    random_r(r, &v.x.f);
    random_r(r, &v.y.f);
    random_r(r, &v.z.f);
    random_r(r, &v.w.f);
    update_key(s, v);
  }
}

void alloc_cipher(session_ref s, counter mats) {
  cipher_key* k = &state.net.sessions[s].crypto.key;

  k->mats_count = mats;
  k->mats_cur = 0;
  k->mats = malloc(sizeof(matrix4) * mats);
}

void free_cipher(session_ref s) {
  cipher_state* c = &state.net.sessions[s].crypto;
  free(c->key.mats);
  free(c->entropy.state);
}

////////////////////////////////////////////////////////////////////////////////
// public key

// prepare a public key for decryption
// fill trace with sigvec
key signkey(key pubkey, signature sigvec) {
  matrix4 o = pubkey;
  vector4 v = unpack_netvec(sigvec);

  o.m[0][0] = v.x;
  o.m[1][1] = v.y;
  o.m[2][2] = v.z;
  o.m[3][3] = v.w;

  return o;
}

// generate paired pubkey and privkey mats
void pk_genkeys(key* pubkey, key* privkey) {
}

byte* pk_encrypt(byte* c, counter len, key pubkey, key privkey) {
}

byte* pk_decrypt(byte* c, counter len, signature sigvec, key pubkey) {
  return symmetric_transform(c, len, signkey(pubkey, sigvec));
}
