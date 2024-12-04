#ifndef _ARCHER_H_
#define _ARCHER_H_

#include <stdlib.h>
#include <gmp.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>

#define SM2_C1C2C3 16
#define SM2_C1C3C2 64


typedef struct EcPrivateKey {
    uint8_t d[32];
} EcPrivateKey;

typedef struct EcPublicKey {
    uint8_t x[32];
    uint8_t y[32];
} EcPublicKey;

typedef struct EcSignature {
    uint8_t r[32];
    uint8_t s[32];
} EcSignature;

typedef struct Hash32 {
    uint8_t h[32];
} Hash32;

typedef struct PaillierPrivateKey {
    uint8_t n[128];
    uint8_t l[128];
} PaillierPrivateKey;

typedef struct PaillierPublicKey {
    uint8_t n[128];
} PaillierPublicKey;

// secp256k1 sign algorithm
/**
 * @return EcPrivateKey, EcPublicKey
*/
void secp256k1_key_gen(EcPrivateKey *sk, EcPublicKey *pk);
/**
 * @param sk, privatekey
 * @return EcPublicKey
*/
void secp256k1_privateKey_to_publicKey(const EcPrivateKey *sk, EcPublicKey *pk);
/**
 * @param sk, privatekey
 * @param msg, input data
 * @param msg_len, length of input data
 * @return sig, signature
 * @return recv_id, the recover id
*/
void secp256k1_sign(const EcPrivateKey *sk, const uint8_t *msg, const size_t msg_len, EcSignature *sig, int *recv_id);
/**
 * @param pk, publickey
 * @param msg, input data
 * @param msg_len, length of input data
 * @return 0 = fail, 1 = success
*/
int secp256k1_verify(const EcPublicKey *pk, const uint8_t *msg, const size_t msg_len, const EcSignature *sig);
/**
 * @param sig, the signature
 * @param msg, input data
 * @param msg_len, length of input data
 * @param recv_id, the recover id
 * @return pk, publickey
*/
void secp256k1_recover_publicKey(const EcSignature *sig, const uint8_t *msg, const size_t msg_len, int recv_id, EcPublicKey *pk);


// sm2 crypto
/**
 * @return EcPrivateKey, EcPublicKey
*/
void sm2p256v1_key_gen(EcPrivateKey *sk, EcPublicKey *pk);
/**
 * @param pk, public key
 * @param msg, input data
 * @param msg_len, length of input data
 * @param mode, SM2_C1C2C3 or SM2_C1C3C2
 * @return out, encrypted data
 * @return out_len, the length of encrypted data
*/
void sm2p256v1_encrypt(const EcPublicKey *pk, const uint8_t *msg, const size_t msg_len, const int mode, uint8_t **out, size_t *out_len);
/**
 * @param sk, the signature
 * @param cipher, input data (cipher)
 * @param cipher_len, length of input data
 * @param mode, SM2_C1C2C3 or SM2_C1C3C2
 * @return out, decrypted data
 * @return out_len, the length of decrypted data
 * @return 1=success, 0=decrypt failed 
*/
int sm2p256v1_decrypt(const EcPrivateKey *sk, const uint8_t *cipher, const size_t cipher_len, const int mode, uint8_t **out, size_t *out_len);



// sm2 sign algorithm
/**
 * @param sk, privatekey
 * @return EcPublicKey
*/
void sm2p256v1_privateKey_to_publicKey(const EcPrivateKey *sk, EcPublicKey *pk);
/**
 * @param sk, privatekey
 * @param msg, input data
 * @param msg_len, length of input data
 * @return sig, signature
 * @return recv_id, the recover id
*/
void sm2p256v1_sign(const EcPrivateKey *sk, const uint8_t *msg, const size_t msg_len, EcSignature *sig);
/**
 * @param pk, publickey
 * @param msg, input data
 * @param msg_len, length of input data
 * @return 0 = fail, 1 = success
*/
int sm2p256v1_verify(const EcPublicKey *pk, const uint8_t *msg, const size_t msg_len, const EcSignature *sig);



// hash
void keccak256(const uint8_t *content, const size_t content_len, Hash32 *hash);
void sha256(const uint8_t *content, const size_t content_len, Hash32 *hash);
void sm3(const uint8_t *content, const size_t content_len, Hash32 *hash);



// sm4
void sm4_encrypt(const uint8_t user_key[16], const uint8_t *in, const size_t in_size, uint8_t **out, size_t *out_size);
/**
 * @return 1=success, 0=decrypt failed 
*/
int sm4_decrypt(const uint8_t user_key[16], const uint8_t *in, const size_t in_size, uint8_t **out, size_t *out_size);

/**
 * @return sk, pk
*/
void paillier_key_gen(PaillierPrivateKey *sk, PaillierPublicKey *pk);
/**
 * @param pk, publicKey
 * @param msg, input data
 * @param msg_len, length of input data
 * @return cipher, encrypted data
 * @return cipher_len, the length of encrypted data
*/
void paillier_encrypt(const PaillierPublicKey *pk, const uint8_t *msg, const size_t msg_len, uint8_t **cipher, size_t *cipher_len);
/**
 * @param sk, privateKey
 * @param cipher, input data
 * @param cipher_len, length of input data
 * @return msg, decrypted data
 * @return msg_len, the length of decrypted data
*/
void paillier_decrypt(const PaillierPrivateKey *sk, const uint8_t *cipher, const size_t cipher_len, uint8_t **msg, size_t *msg_len);
/**
 * c = a + b
 * @param pk, publickey
 * @param cipher0, input encrypted data a
 * @param cipher0_len, length of input encrypted data a
 * @param cipher1, input encrypted data b
 * @param cipher1_len, length of input data b
 * @return cipher, encrypted data c (c = a + b)
 * @return cipher_len, the length of encrypted data c
*/
void paillier_add(const PaillierPublicKey *pk, const uint8_t *cipher0, const size_t cipher0_len, const uint8_t *cipher1, const size_t cipher1_len, uint8_t **cipher, size_t *cipher_len);
/**
 * c = a * b  (a is encrypted num, b is unencrypted num)
 * @param pk, publickey
 * @param cipher_in, input encrypted data a
 * @param cipher_in_len, length of input encrypted data a
 * @param msg, num b, eg 259 = {1, 3} 
 * @param msg_len, length of num b
 * @return cipher, encrypted data c (c = a * b)
 * @return cipher_len, the length of encrypted data c
*/
void paillier_mul(const PaillierPublicKey *pk, const uint8_t *cipher_in, const size_t cipher_in_len, const uint8_t *msg, const size_t msg_len, uint8_t **cipher, size_t *cipher_len);


#endif