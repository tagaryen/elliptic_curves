#ifndef _SM2_P256_V1_H_
#define _SM2_P256_V1_H_

#include "ec_point.h"
#include "sm3.h"

// sm2 crypto
void sm2p256v1_key_gen(EcPrivateKey *sk, EcPublicKey *pk);
void sm2p256v1_encrypt(const EcPublicKey *pk, const uint8_t *msg, const size_t msg_len, const int mode, uint8_t **out, size_t *out_len);
int sm2p256v1_decrypt(const EcPrivateKey *sk, const uint8_t *cipher, const size_t cipher_len, const int mode, uint8_t **out, size_t *out_len);

// sm2 sign algorithm
// void sm2p256v1_init();
void sm2p256v1_privateKey_to_publicKey(const EcPrivateKey *sk, EcPublicKey *pk);
void sm2p256v1_sign(const EcPrivateKey *sk, const uint8_t *msg, const size_t msg_len, EcSignature *sig);
int sm2p256v1_verify(const EcPublicKey *pk, const uint8_t *msg, const size_t msg_len, const EcSignature *sig);

#endif