#ifndef _PAILLIER_H_
#define _PAILLIER_H_

#include "archer.h"

#define P_SIZE  64
#define N_SIZE  128

void paillier_key_gen(PaillierPrivateKey *sk, PaillierPublicKey *pk);
void paillier_encrypt(const PaillierPublicKey *pk, const uint8_t *msg, const size_t msg_len, uint8_t **cipher, size_t *cipher_len);
void paillier_decrypt(const PaillierPrivateKey *sk, const uint8_t *cipher, const size_t cipher_len, uint8_t **msg, size_t *msg_len);
void paillier_add(const PaillierPublicKey *pk, const uint8_t *cipher0, const size_t cipher0_len, const uint8_t *cipher1, const size_t cipher1_len, uint8_t **cipher, size_t *cipher_len);
void paillier_mul(const PaillierPublicKey *pk, const uint8_t *cipher_in, const size_t cipher_in_len, const uint8_t *msg, const size_t msg_len, uint8_t **cipher, size_t *cipher_len);
#endif