// geometric stream cipher
// mysterious and unproven

#pragma once

#include "types.h"
#include "vector4.h"
#include "matrix4.h"
#include "protocol.h"

typedef struct {
  byte key_depth; // how many keymats to request
} cipher_params;

typedef struct {
  matrix4* mats; // key sequence
  counter mats_count;
  counter mats_cur;

  matrix4 cmat; // cumulative key

  // float divergence;
} cipher_key;

// initial seeding
void prime_cipher(session_ref s);

// encrypt/decrypt and update state
byte* encrypt_packet(session_ref c, byte* b, counter len);
byte* decrypt_packet(session_ref c, byte* b, counter len);

// encrypt/decrypt without updating state
byte* encrypt_stream(session_ref s, byte* m, counter len);
byte* decrypt_stream(session_ref s, byte* m, counter len);

// entropy injection
void rekey_cipher(session_ref s);

// management
void alloc_cipher(session_ref s, counter mats);
void free_cipher(session_ref s);

////////////////////////////////////////////////////////////////////////////////
// public-key

void pk_genkeys(key* pubkey, key* privkey);

byte* pk_decrypt(byte* ciphertext, counter len, signature sigvec, key pubkey);
byte* pk_encrypt(byte* plaintext, counter len, key pubkey, key privkey);
