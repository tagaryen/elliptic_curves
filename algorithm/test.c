#include "archer.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define HEX_CHECK(h) ((48<=h)&&(h<=57)||(65<=h&&h<=57)||(97<=h&&h<=102))

static void print_uints(const char *name, uint8_t *bs, size_t len) {
    char m[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    int b0, b1;
    printf(name);
    for(int i = 0; i < len; i++) {
        b0 = (bs[i] >> 4) & 0xf;
        b1 = bs[i] & 0xf;
        printf("%c%c",m[b0], m[b1]);
    }
    printf("\n");
    fflush(stdout);
}


void sm2CostTest() {
    printf("****begin sm2 sign cost test****\n");
    uint8_t d[32] = {29, -3, 74, 47, 123, 64, 41, 123, 67, -9, 89, 16, 84, 115, 18, -8, -41, -97, -57, 36, 103, 60, 115, -123, -5, -38, -97, 127, 32, -21, -25, 2};
    uint8_t p[64] = {124, -111, 78, 61, -127, 10, -126, -115, 18, -118, 16, 64, 63, -12, 77, 32, 8, 95, -32, 73, 36, 98, 63, -81, -1, -112, -45, -87, -119, -31, -91, -5, -76, 120, -20, -101, -57, 45, -115, -110, -52, -50, 83, -74, -117, -113, -38, -51, -125, 18, -42, -84, 59, -33, -105, -3, 23, -8, 83, 51, 45, 74, -31, -105};
	

    int count = 10000;
    EcPrivateKey sk;
    EcPublicKey pk;
    // EcSignature *sig = calloc(count, sizeof(EcSignature));
    EcSignature sig;

    memcpy(sk.d, d, 32);
    memcpy(pk.x, p, 32);
    memcpy(pk.y, &(p[32]), 32);
    printf("****start sm2 sign cost test****\n");
    clock_t t1, t2, t3, t4;
    t1 = clock();
    for(int i = 0; i < count; i++) {
        sm2p256v1_sign(&sk, d, 32, &sig);
    }
    t2 = clock();
    printf("sm2 sign result:\n round: %d\n cost: %ldms\n", count, t2 - t1);
    printf("****start sm2 verify cost test****\n");
    t3 = clock();
    for(int i = 0; i < count; i++) {
        int ret = sm2p256v1_verify(&pk, d, 32, &sig);
        if(!ret) {
            printf("round[%d] verify failed\n", i);
        }
    }
    t4 = clock();
    printf("sm2 verify result:\n round: %d\n cost: %ldms\n", count, t4 - t3);
}

void secTest() {
    printf("****begin secp256k1 sign test****\n");
    uint8_t d[32] = {29, -3, 74, 47, 123, 64, 41, 123, 67, -9, 89, 16, 84, 115, 18, -8, -41, -97, -57, 36, 103, 60, 115, -123, -5, -38, -97, 127, 32, -21, -25, 2};
    uint8_t p[64] = {36, 117, -87, 86, -21, 0, 78, 37, -128, -38, -1, -36, -74, -16, 60, -55, -46, 47, -29, -101, 95, 53, 113, 31, 0, 37, -46, 89, -70, -126, 10, -86, 44, -69, -127, -11, -19, 120, -83, 90, 46, 81, 15, -101, -16, -87, -106, -67, -33, -23, 18, 54, -67, 36, 99, 11, 59, -73, -96, 99, -98, 95, -115, -68};

    EcPrivateKey sk;
    EcPublicKey pk;
    EcSignature sig;
    int recv_id = 0;
    memcpy(sk.d, d, 32);
    memcpy(pk.x, p, 32);
    memcpy(pk.y, &(p[32]), 32);
    printf("****start secp256k1 sign****\n");
    secp256k1_sign(&sk, d, 32, &sig, &recv_id);
    printf("****after secp256k1 sign****\n");
    print_uints("sig = ", (uint8_t *)&sig, 64);
    int ret = secp256k1_verify(&pk, d, 32, &sig);
    printf("ret = %d\n", ret);

    print_uints("pk = ", p, 64);
    EcPublicKey pk_new;
    secp256k1_recover_publicKey(&sig, d, 32, recv_id, &pk_new);
    printf("recv_id = %d\n", recv_id);
    print_uints("pk_new= ", (uint8_t *)&pk_new, 64);
    
}

void sm2Test() {
    printf("****begin sm2 sign test****\n");
    uint8_t d[32] = {29, -3, 74, 47, 123, 64, 41, 123, 67, -9, 89, 16, 84, 115, 18, -8, -41, -97, -57, 36, 103, 60, 115, -123, -5, -38, -97, 127, 32, -21, -25, 2};
    uint8_t p[64] = {124, -111, 78, 61, -127, 10, -126, -115, 18, -118, 16, 64, 63, -12, 77, 32, 8, 95, -32, 73, 36, 98, 63, -81, -1, -112, -45, -87, -119, -31, -91, -5, -76, 120, -20, -101, -57, 45, -115, -110, -52, -50, 83, -74, -117, -113, -38, -51, -125, 18, -42, -84, 59, -33, -105, -3, 23, -8, 83, 51, 45, 74, -31, -105};
	
    EcPrivateKey sk;
    EcPublicKey pk;
    EcSignature sig;
    memcpy(sk.d, d, 32);
    memcpy(pk.x, p, 32);
    memcpy(pk.y, &(p[32]), 32);
    printf("****start sm2 sign****\n");
    sm2p256v1_sign(&sk, d, 32, &sig);
    printf("****after sm2 sign****\n");
    print_uints("sig = ", (uint8_t *)&sig, 64);
    int ret = sm2p256v1_verify(&pk, d, 32, &sig);
    printf("ret = %d\n", ret);
}

void sm4Test() {
    uint8_t key[16] = "keys0123456789ab";
    const char *text = "nihao,shijie,.:.";
    uint8_t *cipher = NULL, *de_text = NULL;
    size_t cipher_len = 0, de_text_len = 0;
    sm4_encrypt(key, text, strlen(text), &cipher, &cipher_len);
    printf("text_len = %lu, ciper_len = %d\n", strlen(text), cipher_len);
    print_uints("cipher = ", cipher, cipher_len);

    if(sm4_decrypt(key, cipher, cipher_len, &de_text, &de_text_len)) {
        printf("de_text_len = %lu\n", de_text_len);
        char txt[de_text_len + 1];
        memcpy(txt, de_text, de_text_len);
        txt[de_text_len] = '\0';
        print_uints("de = ", de_text, de_text_len);
        printf("txt = %s", txt);
    } else {
        printf("decrypt failed\n");
    }

    free(cipher);
    free(de_text);
}

void sm2CryptoTest() {
    printf("****begin sm2 crypto test****\n");
    EcPrivateKey sk;
    EcPublicKey pk;
    const uint8_t *msg = "nihao,dashabi";
    uint8_t *cipher, *text;
    size_t cipher_l, text_l;

    sm2p256v1_key_gen(&sk, &pk);
    printf("****start sm2 crypto test****\n");
    sm2p256v1_encrypt(&pk, msg, strlen(msg), SM2_C1C3C2, &cipher, &cipher_l);
    print_uints("cipher = ", cipher, cipher_l);
    if(sm2p256v1_decrypt(&sk, cipher, cipher_l, SM2_C1C3C2, &text, &text_l)) {
        char txt[text_l + 1];
        memcpy(txt, text, text_l);
        txt[text_l] = '\0';
        printf("sm2 encrypt success. l = %lu, txt = %s\n", text_l, txt);
    } else {
        printf("sm2 encrypt failed\n");
    }
}

void paillierTest() {
    PaillierPrivateKey *sk = (PaillierPrivateKey *)malloc(sizeof(PaillierPrivateKey));
    PaillierPublicKey *pk = (PaillierPublicKey *)malloc(sizeof(PaillierPublicKey));
    paillier_key_gen(sk, pk);

    const char *msg = "nihao, shijie";
    uint8_t *cipher = NULL;
    size_t cipher_len = 0;
    uint8_t *decrypt_m = NULL;
    size_t decrypt_len = 0;
    paillier_encrypt(pk, msg, strlen(msg), &cipher, &cipher_len);
    paillier_decrypt(sk, cipher, cipher_len, &decrypt_m, &decrypt_len);
    char buf[decrypt_len + 1];
    memcpy(buf, decrypt_m, decrypt_len);
    buf[decrypt_len] = '\0';
    printf("base decrypt msg = %s, len = %d\n", buf, decrypt_len);




    const uint8_t m1[1] = {97}, m2[1] = {96};
    uint8_t *c1 = NULL, *c2 = NULL, *c_add = NULL, *c_add_de = NULL;
    size_t c1_len = 0, c2_len = 0, c_add_len = 0, c_add_de_len = 0;
    paillier_encrypt(pk, m1, 1, &c1, &c1_len);
    paillier_encrypt(pk, m2, 1, &c2, &c2_len);
    paillier_add(pk, c1, c1_len, c2, c2_len, &c_add, &c_add_len);
    paillier_decrypt(sk, c_add, c_add_len, &c_add_de, &c_add_de_len);

    printf("add decrypt msg = %d, len = %d\n", c_add_de[0], c_add_de_len);




    const uint8_t m_mul[1] = {10};
    uint8_t *c_mul = NULL, *c_mul_de = NULL;
    size_t c_mul_len = 0, c_mul_de_len = 0;
    paillier_mul(pk, c1, c1_len, m_mul, 1, &c_mul, &c_mul_len);
    paillier_decrypt(sk, c_mul, c_mul_len, &c_mul_de, &c_mul_de_len);
    printf("mul decrypt m[0] = %d, m[1] = %d, len = %d\n", c_mul_de[0], c_mul_de[1], c_mul_de_len);
}

// gcc test.c -L. -lalg -O3 -o test.exe
// gcc *.c -lgmp -O3 -o test.exe
int main() {

    // sm2Test();
    secTest();
    // sm4Test();

    // sm2CostTest();

    // sm2CryptoTest();

    // testBits();

    // paillierTest();

    return 0;
}
