#include "sm2p256v1.h"

typedef struct sm2p256v1_curve {
    mpz_t p, a, b, gx, gy, n;
} sm2p256v1_curve;

static pthread_mutex_t _sm2p256v1_lock = PTHREAD_MUTEX_INITIALIZER;
static sm2p256v1_curve *_sm2p256v1 = NULL;

// userId = "1234567812345678"
static void sm2p256v1_get_za(const uint8_t *x, const uint8_t *y, const uint8_t *msg, const size_t msg_len, Hash32 *out) {
    uint8_t base[146] = {0,-128,49,50,51,52,53,54,55,56,49,50,51,52,53,54,55,
                56,-1,-1,-1,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,0,0,0,0,-1,-1,-1,-1,-1,-1,-1,-4,40,-23,-6,-98,
                -99,-97,94,52,77,90,-98,75,-49,101,9,-89,-13,-105,-119,
                -11,21,-85,-113,-110,-35,-68,-67,65,77,-108,14,-109,50,
                -60,-82,44,31,25,-127,25,95,-103,4,70,106,57,-55,-108,
                -113,-29,11,-65,-14,102,11,-31,113,90,69,-119,51,76,116,
                -57,-68,55,54,-94,-12,-10,119,-100,89,-67,-50,-29,107,
                105,33,83,-48,-87,-121,124,-58,42,71,64,2,-33,50,-27,33,
                57,-16,-96};
    uint8_t input[210];
    memcpy(input, base, 146);
    memcpy(&(input[146]), x, 32);
    memcpy(&(input[146 + 32]), y, 32);
    Hash32 z;
    sm3(input, 210, &z);
    uint8_t *e = malloc(32 + msg_len);
    memcpy(e, z.h, 32);
    memcpy(&(e[32]), msg, msg_len);
    sm3(e, 32 + msg_len, out);
    free(e);
}

static void sm2p256v1_cal_rs(mpz_t d, mpz_t e, mpz_t r, mpz_t s) {
    uint8_t raw_k[32];
    uint16_t seed = (uint16_t) ((int64_t) raw_k);
    ec_random_k(raw_k, seed++);
    mpz_t k, kx, ky;
    mpz_init(k);
    mpz_init(kx);
    mpz_init(ky);
    mpz_import(k, 32, 1, 1, 0, 0, raw_k);
    ec_point_mul(kx, ky, k, _sm2p256v1->p, _sm2p256v1->a, _sm2p256v1->b, _sm2p256v1->gx, _sm2p256v1->gy);
    mpz_add(kx, kx, e);
    mpz_mod(kx, kx, _sm2p256v1->n);
    mpz_add(ky, kx, e);
    while(!mpz_cmp(ky, _sm2p256v1->n)) {
        ec_random_k(raw_k, seed++);
        mpz_import(k, 32, 1, 1, 0, 0, raw_k);
        ec_point_mul(kx, ky, k, _sm2p256v1->p, _sm2p256v1->a, _sm2p256v1->b, _sm2p256v1->gx, _sm2p256v1->gy);
        mpz_add(kx, kx, e);
        mpz_mod(kx, kx, _sm2p256v1->n);
        mpz_add(ky, kx, e);
    }
    mpz_set(r, kx);
    mpz_mul(s, kx, d);
    mpz_sub(s, k, s);
    mpz_mod(s, s, _sm2p256v1->n);

    mpz_add_ui(kx, d, 1);
    mpz_invert(kx, kx, _sm2p256v1->n);
    mpz_mul(s, s, kx);
    mpz_mod(s, s, _sm2p256v1->n);

    mpz_clear(k);
    mpz_clear(kx);
    mpz_clear(ky);
}

static void kdf(uint8_t *c1x, size_t x_l, uint8_t *c1y, size_t y_l, uint8_t *c2, size_t c2_l) {
    size_t l = x_l + y_l, off = 0, ct = 0, digest_size = 32;
    Hash32 buf;
    uint8_t *in = malloc(l + 4);
    memcpy(in, c1x, x_l);
    memcpy(in + x_l, c1y, y_l);
    while(off < c2_l) {
        ++ct;
        in[l] = (ct >> 24) & 0xff;
        in[l+1] = (ct >> 16) & 0xff;
        in[l+2] = (ct >> 8) & 0xff;
        in[l+3] = ct & 0xff;

        sm3(in, l+4, &buf);
        digest_size = (c2_l - off) > 32 ? 32 : (c2_l - off);
        for(int i = 0; i < digest_size; i++) {
            c2[off + i] ^= buf.h[i];
        }
        off += digest_size;
    }
    free(in);
}

static void sm2p256v1_init() {
    if(!_sm2p256v1) {
        pthread_mutex_lock(&_sm2p256v1_lock);
        if(!_sm2p256v1) {
            _sm2p256v1 = (sm2p256v1_curve *)malloc(sizeof(sm2p256v1_curve));
            mpz_init_set_str(_sm2p256v1->p, "fffffffeffffffffffffffffffffffffffffffff00000000ffffffffffffffff", 16);
            mpz_init_set_str(_sm2p256v1->a, "fffffffeffffffffffffffffffffffffffffffff00000000fffffffffffffffc", 16);
            mpz_init_set_str(_sm2p256v1->b, "28e9fa9e9d9f5e344d5a9e4bcf6509a7f39789f515ab8f92ddbcbd414d940e93", 16);
            mpz_init_set_str(_sm2p256v1->gx, "32c4ae2c1f1981195f9904466a39c9948fe30bbff2660be1715a4589334c74c7", 16);
            mpz_init_set_str(_sm2p256v1->gy, "bc3736a2f4f6779c59bdcee36b692153d0a9877cc62a474002df32e52139f0a0", 16);
            mpz_init_set_str(_sm2p256v1->n, "fffffffeffffffffffffffffffffffff7203df6b21c6052b53bbf40939d54123", 16);
        }
        pthread_mutex_unlock(&_sm2p256v1_lock);
    }
}

void sm2p256v1_key_gen(EcPrivateKey *sk, EcPublicKey *pk) {
    if(!sk || !pk) {
        return ;
    }
    ec_random_k(sk->d, (uint16_t)((int64_t) (pk)));
    sm2p256v1_privateKey_to_publicKey(sk, pk);
}


void sm2p256v1_encrypt(const EcPublicKey *pk, const uint8_t *msg, const size_t msg_len, const int mode, uint8_t **out, size_t *out_len) {
    if(!pk || !msg) {
        return ;
    }
    
    sm2p256v1_init();

    uint8_t raw_k[32];
    ec_random_k(raw_k, (uint16_t) ((int64_t) raw_k));
    
    mpz_t k, kx, ky, kpx, kpy, x, y;
    mpz_init(k);
    mpz_init(kx);
    mpz_init(ky);
    mpz_init(kpx);
    mpz_init(kpy);
    mpz_init(x);
    mpz_init(y);
    mpz_import(k, 32, 1, 1, 0, 0, raw_k);
    mpz_import(x, 32, 1, 1, 0, 0, pk->x);
    mpz_import(y, 32, 1, 1, 0, 0, pk->y);

    ec_point_mul(kx, ky, k, _sm2p256v1->p, _sm2p256v1->a, _sm2p256v1->b, _sm2p256v1->gx, _sm2p256v1->gy);
    ec_point_mul(kpx, kpy, k, _sm2p256v1->p, _sm2p256v1->a, _sm2p256v1->b, x, y);

    Hash32 c3;
    uint8_t xc[32], yc[32], c1x[32], c1y[32], *c2 = malloc(msg_len);
    size_t lx = 32, ly = 32, c1_xl = 32, c1_yl = 32;

    mpz_export(c1x, &c1_xl, 1, 1, 0, 0, kx);
    mpz_export(c1y, &c1_yl, 1, 1, 0, 0, ky);
    
    mpz_export(xc, &lx, 1, 1, 0, 0, kpx);
    mpz_export(yc, &ly, 1, 1, 0, 0, kpy);

    memcpy(c2, msg, msg_len);
    kdf(xc, lx, yc, ly, c2, msg_len);
    
    *out_len = 65 + msg_len + 32;
    *out = malloc(*out_len);
    memset(*out, 0, *out_len);

    memcpy(*out, xc + (32 - lx), lx);
    memcpy((*out) + 32, msg, msg_len);
    memcpy((*out) + (32 + msg_len), yc + (32 - ly), ly);
    sm3(*out, 64 + msg_len, &c3);

    memset(*out, 0, *out_len);
    (*out)[0] = 4;
    memcpy((*out) + 1, c1x + (32 - c1_xl), c1_xl);
    memcpy((*out) + 33, c1y+ (32 - c1_yl), c1_yl);
    if(SM2_C1C2C3 == mode) {
        memcpy((*out) + 65, c2, msg_len);
        memcpy((*out) + (65 + msg_len), c3.h, 32);
    } else if(SM2_C1C3C2 == mode) {
        memcpy((*out) + 65, c3.h, 32);
        memcpy((*out) + 97, c2, msg_len);
    }
    free(c2);
    mpz_clear(k);
    mpz_clear(kx);
    mpz_clear(ky);
    mpz_clear(kpx);
    mpz_clear(kpy);
    mpz_clear(x);
    mpz_clear(y);
}


int sm2p256v1_decrypt(const EcPrivateKey *sk, const uint8_t *cipher, const size_t cipher_len, const int mode, uint8_t **out, size_t *out_len) {
    if(cipher_len <= 97) {
        return 0;
    }
    
    sm2p256v1_init();
    
    int ret = 1;
    *out_len =  cipher_len - 65 - 32;
    *out = malloc(*out_len);
    uint8_t c1[65], *c2 = (*out), c3[32];
    memcpy(c1, cipher, 65);
    if(SM2_C1C2C3 == mode) {
        memcpy(c2, cipher + 65, cipher_len - 65 - 32);
        memcpy(c3, cipher + (cipher_len - 32), 32);
    } else if(SM2_C1C3C2 == mode) {
        memcpy(c3, cipher + 65, 32);
        memcpy(c2, cipher + 97, cipher_len - 97);
    }

    mpz_t x, y, d, kx, ky;
    mpz_init(x);
    mpz_init(y);
    mpz_init(d);
    mpz_init(kx);
    mpz_init(ky);
    mpz_import(d, 32, 1, 1, 0, 0, sk->d);
    mpz_import(kx, 32, 1, 1, 0, 0, c1+1);
    mpz_import(ky, 32, 1, 1, 0, 0, c1+33);
    ec_point_mul(x, y, d, _sm2p256v1->p, _sm2p256v1->a, _sm2p256v1->b, kx, ky);

    Hash32 c3_cpy;
    uint8_t xc[32], yc[32], *hash_in = malloc(64 + *out_len);
    size_t lx = 32, ly = 32;
    mpz_export(xc, &lx, 1, 1, 0, 0, x);
    mpz_export(yc, &ly, 1, 1, 0, 0, y);

    kdf(xc, lx, yc, ly, c2, *out_len);
    memcpy(hash_in, xc+(32-lx), lx);
    memcpy(hash_in + 32, c2, *out_len);
    memcpy(hash_in + (32 + *out_len), yc+(32-lx), ly);

    sm3(hash_in, 64 + *out_len, &c3_cpy);
    for(int i = 0; i < 32; i++) {
        if(c3[i] != c3_cpy.h[i]) {
            ret = 0;
            break ;
        }
    }
    free(hash_in);
    mpz_clear(x);
    mpz_clear(y);
    mpz_clear(d);
    mpz_clear(kx);
    mpz_clear(ky);

    return ret;
}

void sm2p256v1_privateKey_to_publicKey(const EcPrivateKey *sk, EcPublicKey *pk) {    
    if(!sk || !pk) {
        return ;
    }

    sm2p256v1_init();

    mpz_t x, y, d;
    mpz_init(x);
    mpz_init(y);
    mpz_init(d);
    mpz_import(d, 32, 1, 1, 0, 0, sk->d);
    ec_point_mul(x, y, d, _sm2p256v1->p, _sm2p256v1->a, _sm2p256v1->b, _sm2p256v1->gx, _sm2p256v1->gy);
    
    uint8_t xc[32], yc[32];
    size_t lx = 32, ly = 32;
    mpz_export(xc, &lx, 1, 1, 0, 0, x);
    mpz_export(yc, &ly, 1, 1, 0, 0, y);

    memset(pk->x, 0, 32);
    memset(pk->y, 0, 32);
    memcpy(pk->x + (32 - lx), xc, lx);
    memcpy(pk->y + (32 - ly), yc, ly);

    mpz_clear(x);
    mpz_clear(y);
    mpz_clear(d);
}

void sm2p256v1_sign(const EcPrivateKey *sk, const uint8_t *msg, const size_t msg_len, EcSignature *sig) {
    if(!sk || !msg || !sig) {
        return ;
    }

    sm2p256v1_init();

    EcPublicKey pk;
    Hash32 za;
    sm2p256v1_privateKey_to_publicKey(sk, &pk);
    sm2p256v1_get_za(pk.x, pk.y, msg, msg_len, &za);

    mpz_t d, e, r, s;
    mpz_init(r);
    mpz_init(s);

    mpz_init(d);
    mpz_init(e);
    mpz_import(d, 32, 1, 1, 0, 0, sk->d);
    mpz_import(e, 32, 1, 1, 0, 0, za.h);

    sm2p256v1_cal_rs(d, e, r, s);

    size_t lr = 32, ls = 32;
    uint8_t rc[32], sc[32];
    mpz_export(rc, &lr, 1, 1, 0, 0, r);
    mpz_export(sc, &ls, 1, 1, 0, 0, s);
    
    memset(sig->r, 0, 32);
    memset(sig->s, 0, 32);
    memcpy(sig->r + (32 - lr),rc, lr);
    memcpy(sig->s + (32 - ls), sc, ls);

    mpz_clear(r);
    mpz_clear(s);
    mpz_clear(d);
    mpz_clear(e);
}


int sm2p256v1_verify(const EcPublicKey *pk, const uint8_t *msg, const size_t msg_len, const EcSignature *sig) {
    if(!pk || !msg || !sig) {
        return 0;
    }

    sm2p256v1_init();

    int ret = 0;
    Hash32 za;
    sm2p256v1_get_za(pk->x, pk->y, msg, msg_len, &za);

    mpz_t x, y, e, r, s, b0x, b0y, b1x, b1y;
    mpz_init(x);
    mpz_init(y);
    mpz_init(e);
    mpz_init(r);
    mpz_init(s);
    mpz_init(b0x);
    mpz_init(b0y);
    mpz_init(b1x);
    mpz_init(b1y);

    mpz_import(x, 32, 1, 1, 0, 0, pk->x);
    mpz_import(y, 32, 1, 1, 0, 0, pk->y);
    mpz_import(e, 32, 1, 1, 0, 0, za.h);
    mpz_import(r, 32, 1, 1, 0, 0, sig->r);
    mpz_import(s, 32, 1, 1, 0, 0, sig->s);

    ec_point_mul(b0x, b0y, s, _sm2p256v1->p, _sm2p256v1->a, _sm2p256v1->b, _sm2p256v1->gx, _sm2p256v1->gy);
    mpz_add(s, r, s);
    mpz_mod(s, s, _sm2p256v1->n);
    ec_point_mul(b1x, b1y, s, _sm2p256v1->p, _sm2p256v1->a, _sm2p256v1->b, x, y);
    ec_point_add(x, y, b0x, b0y, b1x, b1y, _sm2p256v1->p);
    mpz_add(x, x, e);
    mpz_mod(x, x, _sm2p256v1->n);
    ret = !(mpz_cmp(x, r));

    mpz_clear(x);
    mpz_clear(y);
    mpz_clear(e);
    mpz_clear(r);
    mpz_clear(s);
    mpz_clear(b0x);
    mpz_clear(b0y);
    mpz_clear(b1x);
    mpz_clear(b1y);

    return ret;
}