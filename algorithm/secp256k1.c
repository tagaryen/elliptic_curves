#include "secp256k1.h"

typedef struct secp256k1_curve {
    mpz_t p, a, b, gx, gy, n, _0;
} secp256k1_curve;

static pthread_mutex_t _secp256k1_lock = PTHREAD_MUTEX_INITIALIZER;
static secp256k1_curve *_secp256k1 = NULL;

static void secp256k1_init() {
    if(!_secp256k1) {
        pthread_mutex_lock(&_secp256k1_lock);
        if(!_secp256k1) {
            _secp256k1 = (secp256k1_curve *)malloc(sizeof(secp256k1_curve));
            mpz_init_set_str(_secp256k1->p, "fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f", 16);
            mpz_init_set_ui(_secp256k1->a, 0);
            mpz_init_set_ui(_secp256k1->b, 7);
            mpz_init_set_str(_secp256k1->gx, "79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798", 16);
            mpz_init_set_str(_secp256k1->gy, "483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8", 16);
            mpz_init_set_str(_secp256k1->n, "fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141", 16);
            mpz_init_set_ui(_secp256k1->_0, 0);
        }
        pthread_mutex_unlock(&_secp256k1_lock);
    }
}

void secp256k1_key_gen(EcPrivateKey *sk, EcPublicKey *pk) {
    if(!sk || !pk) {
        return ;
    }
    ec_random_k(sk->d, (uint16_t)((int64_t) (pk)));
    secp256k1_privateKey_to_publicKey(sk, pk);
}

void secp256k1_privateKey_to_publicKey(const EcPrivateKey *sk, EcPublicKey *pk) {    
    if(!sk || !pk) {
        return ;
    }

    secp256k1_init();
    
    mpz_t x, y, d;
    mpz_init(x);
    mpz_init(y);
    mpz_init(d);
    mpz_import(d, 32, 1, 1, 0, 0, sk->d);
    ec_point_mul(x, y, d, _secp256k1->p, _secp256k1->a, _secp256k1->b, _secp256k1->gx, _secp256k1->gy);
    
    uint8_t xc[32], yc[32];
    size_t lx = 32, ly = 32;
    mpz_export(xc, &lx, 1, 1, 0, 0, x);
    mpz_export(yc, &ly, 1, 1, 0, 0, y);

    memset(pk->x, 0, 32);
    memset(pk->y, 0, 32);
    memcpy(pk->x + (32 - lx), xc, lx);
    memcpy(pk->y + (32 - lx), yc, ly);

    mpz_clear(x);
    mpz_clear(y);
    mpz_clear(d);
}

void secp256k1_sign(const EcPrivateKey *sk, const uint8_t *msg, const size_t msg_len, EcSignature *sig, int *recv_id) {
    if(!sk || !msg || !sig) {
        return ;
    }
    
    secp256k1_init();
    
    EcPublicKey pk;
    uint8_t raw_k[32];
    uint16_t seed = (uint16_t) ((int64_t) raw_k);
    ec_random_k(raw_k, seed++);

    mpz_t d, m, k, r, s;
    mpz_init(d);
    mpz_init(m);
    mpz_init(k);
    mpz_init(r);
    mpz_init(s);
    mpz_import(d, 32, 1, 1, 0, 0, sk->d);
    mpz_import(m, msg_len, 1, 1, 0, 0, msg);
    mpz_import(k, 32, 1, 1, 0, 0, raw_k);

    // s = (d * r + m) * k^(-1), r = k * G  
    ec_point_mul(r, s, k, _secp256k1->p, _secp256k1->a, _secp256k1->b, _secp256k1->gx, _secp256k1->gy);
    int v = mpz_odd_p(s);
    mpz_mul(s, d, r);
    mpz_add(s, s, m);
    mpz_invert(k, k, _secp256k1->n);
    mpz_mul(s, s, k);
    mpz_mod(s, s, _secp256k1->n);
    mpz_mul_ui(k, s, 2);
    if(mpz_cmp(k, _secp256k1->n) > 0) {
        mpz_sub(s, _secp256k1->n , s);
        v ^= 1;
    }
    *recv_id = v;

    size_t lr = 32, ls = 32;
    uint8_t rc[32], sc[32];
    mpz_export(rc, &lr, 1, 1, 0, 0, r);
    mpz_export(sc, &ls, 1, 1, 0, 0, s);
    
    memset(sig->r, 0, 32);
    memset(sig->s, 0, 32);
    memcpy(sig->r + (32 - lr), rc, lr);
    memcpy(sig->s + (32 - ls), sc, ls);

    mpz_clear(d);
    mpz_clear(m);
    mpz_clear(k);
    mpz_clear(r);
    mpz_clear(s);
}


int secp256k1_verify(const EcPublicKey *pk, const uint8_t *msg, const size_t msg_len, const EcSignature *sig) {
    if(!pk || !msg || !sig) {
        return 0;
    }
    
    secp256k1_init();

    int ret = 0;

    mpz_t x, y, m, r, s, b0x, b0y, b1x, b1y;
    mpz_init(x);
    mpz_init(y);
    mpz_init(m);
    mpz_init(r);
    mpz_init(s);
    mpz_init(b0x);
    mpz_init(b0y);
    mpz_init(b1x);
    mpz_init(b1y);

    mpz_import(x, 32, 1, 1, 0, 0, pk->x);
    mpz_import(y, 32, 1, 1, 0, 0, pk->y);
    mpz_import(m, msg_len, 1, 1, 0, 0, msg);
    mpz_import(r, 32, 1, 1, 0, 0, sig->r);
    mpz_import(s, 32, 1, 1, 0, 0, sig->s);

    mpz_invert(s, s, _secp256k1->n);

    mpz_mul(m, m, s);
    mpz_mod(m, m, _secp256k1->n);
    ec_point_mul(b0x, b0y, m, _secp256k1->p, _secp256k1->a, _secp256k1->b, _secp256k1->gx, _secp256k1->gy);

    mpz_mul(m, r, s);
    mpz_mod(m, m, _secp256k1->n);
    ec_point_mul(b1x, b1y, m, _secp256k1->p, _secp256k1->a, _secp256k1->b, x, y);

    ec_point_add(x, y, b0x, b0y, b1x, b1y, _secp256k1->p);
    mpz_mod(x, x, _secp256k1->p);
    ret = !(mpz_cmp(x, r));

    mpz_clear(x);
    mpz_clear(y);
    mpz_clear(m);
    mpz_clear(r);
    mpz_clear(s);
    mpz_clear(b0x);
    mpz_clear(b0y);
    mpz_clear(b1x);
    mpz_clear(b1y);

    return ret;
}

void secp256k1_recover_publicKey(const EcSignature *sig, const uint8_t *msg, const size_t msg_len, int recv_id, EcPublicKey *pk) {
    if(!sig || !msg || !pk) {
        return ;
    }
    
    secp256k1_init();

    mpz_t x, y, t, m, mx, my, s, sx, sy;
    mpz_init(x);
    mpz_init(y);
    mpz_init(t);
    mpz_init(mx);
    mpz_init(my);
    mpz_init(m);
    mpz_init(s);
    mpz_init(sx);
    mpz_init(sy);
    mpz_import(x, 32, 1, 1, 0, 0, sig->r);
    mpz_import(s, 32, 1, 1, 0, 0, sig->s);
    mpz_import(m, msg_len, 1, 1, 0, 0, msg);
    

    mpz_pow_ui(y, x, 3);
    mpz_add(y, y, _secp256k1->b);
    mpz_mod(y, y, _secp256k1->p);
    mpz_add_ui(t, _secp256k1->p, 1);
    mpz_div_ui(t, t, 4);
    mpz_powm(y, y, t, _secp256k1->p);
    int odd = mpz_odd_p(y);
    if(odd ^ recv_id) {
        mpz_sub(y, _secp256k1->p , y);
    }
    
    // s = (d * r + m) * k^(-1), r = k * G  = (x, y)
    ec_point_mul(sx, sy, s, _secp256k1->p, _secp256k1->a, _secp256k1->b, x, y);
    ec_point_mul(mx, my, m, _secp256k1->p, _secp256k1->a, _secp256k1->b, _secp256k1->gx, _secp256k1->gy);

    mpz_set(t, x);
    mpz_invert(t, t, _secp256k1->n);

    mpz_sub(my, _secp256k1->_0, my);
    ec_point_add(x, y, sx, sy, mx, my, _secp256k1->p);
    ec_point_mul(mx, my, t, _secp256k1->p, _secp256k1->a, _secp256k1->b, x, y);

    size_t lx = 32, ly = 32;
    uint8_t xc[32], yc[32];
    mpz_export(xc, &lx, 1, 1, 0, 0, mx);
    mpz_export(yc, &ly, 1, 1, 0, 0, my);

    memset(pk->x, 0, 32);
    memset(pk->y, 0, 32);
    memcpy(pk->x + (32 - lx), xc, lx);
    memcpy(pk->y + (32 - ly), yc, ly);

    mpz_clear(x);
    mpz_clear(y);
    mpz_clear(t);
    mpz_clear(mx);
    mpz_clear(my);
    mpz_clear(m);
    mpz_clear(s);
    mpz_clear(sx);
    mpz_clear(sy);
}