#ifndef _SM4_H_
#define _SM4_H_

#include "archer.h"

// pkcs#7 padding
void sm4_encrypt(const uint8_t user_key[16], const uint8_t *in, const size_t in_size, uint8_t **out, size_t *out_size);
int sm4_decrypt(const uint8_t user_key[16], const uint8_t *in, const size_t in_size, uint8_t **out, size_t *out_size);

#endif