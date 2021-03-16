#ifndef _QARMA_H
#define _QARMA_H

#define MAX_LENGTH 64
#define subcells sbox[sbox_use]
#define subcells_inv sbox_inv[sbox_use]

typedef unsigned long long int const_t;
typedef unsigned long long int tweak_t;
typedef unsigned long long int text_t;
typedef unsigned long long int qkey_t;
typedef unsigned char          cell_t;

text_t qarma64_enc(text_t plaintext, tweak_t tweak, qkey_t w0, qkey_t k0, int rounds);
text_t qarma64_dec(text_t plaintext, tweak_t tweak, qkey_t w0, qkey_t k0, int rounds);

#endif
