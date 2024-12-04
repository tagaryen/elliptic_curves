#ifndef _SEC_P256_K1_H_
#define _SEC_P256_K1_H_

#include "ec_point.h"
#include "keccak256.h"

// secp256k1 algorithm
void secp256k1_key_gen(EcPrivateKey *sk, EcPublicKey *pk);
void secp256k1_privateKey_to_publicKey(const EcPrivateKey *sk, EcPublicKey *pk);
void secp256k1_sign(const EcPrivateKey *sk, const uint8_t *msg, const size_t msg_len, EcSignature *sig, int *recv_id);
int secp256k1_verify(const EcPublicKey *pk, const uint8_t *msg, const size_t msg_len, const EcSignature *sig);
void secp256k1_recover_publicKey(const EcSignature *sig, const uint8_t *msg, const size_t msg_len, int v, EcPublicKey *pk);

#endif