#include "paillier.h"

static void paillier_prime_random(mpz_t p, int bits) {
    uint32_t seed = (uint64_t) p;
    gmp_randstate_t grt;
    gmp_randinit_default(grt);
    gmp_randseed_ui(grt, seed);
    mpz_urandomb(p, grt, bits);
    while (!mpz_probab_prime_p(p, 25)) {
        mpz_urandomb(p, grt, bits);
    }
    gmp_randclear(grt);
}

static void paillier_random(mpz_t p, int bits) {
    uint32_t seed = (uint64_t) p;
    gmp_randstate_t grt;
    gmp_randinit_default(grt);
    gmp_randseed_ui(grt, seed);
    mpz_urandomb(p, grt, bits);
    gmp_randclear(grt);
}

void paillier_key_gen(PaillierPrivateKey *sk, PaillierPublicKey *pk) {
    mpz_t p, q, n, l;
    mpz_init(p);
    mpz_init(q);
    mpz_init(n);
    mpz_init(l);

    paillier_prime_random(p, P_SIZE * 8);
    paillier_prime_random(q, P_SIZE * 8);

    mpz_mul(n, p, q);
    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_lcm(l, p, q);

    
    size_t ln = N_SIZE, ll = N_SIZE;
    uint8_t nc[N_SIZE], lc[N_SIZE];
    mpz_export(nc, &ln, 1, 1, 0, 0, n);
    mpz_export(lc, &ll, 1, 1, 0, 0, l);

    
    memcpy(sk->l + (N_SIZE - ll), lc, ll);
    memcpy(sk->n + (N_SIZE - ln), nc, ln);
    
    memcpy(pk->n, &(nc[N_SIZE-ln]), ln);

    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(n);
    mpz_clear(l);
}

void paillier_encrypt(const PaillierPublicKey *pk, const uint8_t *msg, const size_t msg_len, uint8_t **cipher, size_t *cipher_len) {    
    mpz_t r, m, n, mn, rl;
    mpz_init(r);
    mpz_init(m);
    mpz_init(n);
    mpz_init(mn);
    mpz_init(rl);
    paillier_random(r, P_SIZE);

    mpz_import(m, msg_len, 1, 1, 0, 0, msg);
    mpz_import(n, N_SIZE, 1, 1, 0, 0, pk->n);

    // printf("encrypt r = %s\n", mpz_get_str(NULL, 10, r));
    // printf("encrypt m = %s\n", mpz_get_str(NULL, 10, m));
    // printf("encrypt n = %s\n", mpz_get_str(NULL, 10, n));

    mpz_mul(mn, m, n);
    mpz_add_ui(mn, mn, 1);

    mpz_mul(rl, n, n);
    mpz_powm(rl, r, n, rl);

    mpz_mul(mn, mn, rl);
    mpz_mul(rl, n, n);
    mpz_mod(mn, mn, rl);
    
    size_t lc = N_SIZE + N_SIZE;
    uint8_t c[N_SIZE + N_SIZE];
    mpz_export(c, &lc, 1, 1, 0, 0, mn);
    (*cipher) = (uint8_t *)malloc(lc);
    (*cipher_len) = lc;
    memcpy((*cipher), c, lc);

    mpz_clear(r);
    mpz_clear(n);
    mpz_clear(m);
    mpz_clear(mn);
    mpz_clear(rl);
}

void paillier_decrypt(const PaillierPrivateKey *sk, const uint8_t *cipher, const size_t cipher_len, uint8_t **msg, size_t *msg_len) {
    mpz_t l, n, c, t, u;
    mpz_init(l);
    mpz_init(n);
    mpz_init(c);
    mpz_init(t);
    mpz_init(u);

    mpz_import(c, cipher_len, 1, 1, 0, 0, cipher);
    mpz_import(n, N_SIZE, 1, 1, 0, 0, sk->n);
    mpz_import(l, N_SIZE, 1, 1, 0, 0, sk->l);

    // printf("decrypt c = %s\n", mpz_get_str(NULL, 10, c));
    // printf("decrypt l = %s\n", mpz_get_str(NULL, 10, l));
    // printf("decrypt n = %s\n", mpz_get_str(NULL, 10, n));

    mpz_mul(t, n, n);
    mpz_powm(t, c, l, t);
    mpz_sub_ui(t, t, 1);
    mpz_div(t, t, n);

    mpz_invert(u, l, n);

    mpz_mul(t, t, u);

    mpz_mod(t, t, n);
    
    size_t mc = N_SIZE;
    uint8_t m[N_SIZE];
    mpz_export(m, &mc, 1, 1, 0, 0, t);
    (*msg) = (uint8_t *)malloc(mc);
    *msg_len = mc;
    memcpy(*msg, m, mc);

    mpz_clear(l);
    mpz_clear(n);
    mpz_clear(c);
    mpz_clear(t);
    mpz_clear(u);
}

void paillier_add(const PaillierPublicKey *pk, const uint8_t *cipher0, const size_t cipher0_len, const uint8_t *cipher1, const size_t cipher1_len, uint8_t **cipher, size_t *cipher_len) {
    mpz_t c0, c1, n;
    mpz_init(c0);
    mpz_init(c1);
    mpz_init(n);

    mpz_import(c0, cipher0_len, 1, 1, 0, 0, cipher0);
    mpz_import(c1, cipher1_len, 1, 1, 0, 0, cipher1);
    mpz_import(n, N_SIZE, 1, 1, 0, 0, pk->n);

    // printf("add c0 = %s\n", mpz_get_str(NULL, 10, c0));
    // printf("add c1 = %s\n", mpz_get_str(NULL, 10, c1));
    mpz_mul(n, n, n);
    mpz_mul(c0, c0, c1);
    mpz_mod(c0, c0, n);

    // printf("add enc = %s\n", mpz_get_str(NULL, 10, c0));

    size_t mc = N_SIZE + N_SIZE;
    uint8_t m[N_SIZE + N_SIZE];
    mpz_export(m, &mc, 1, 1, 0, 0, c0);
    (*cipher) = (uint8_t *)malloc(mc);
    *cipher_len = mc;
    memcpy(*cipher, m, mc);

    mpz_clear(c0);
    mpz_clear(c1);
    mpz_clear(n);
}

void paillier_mul(const PaillierPublicKey *pk, const uint8_t *cipher_in, const size_t cipher_in_len, const uint8_t *msg, const size_t msg_len, uint8_t **cipher, size_t *cipher_len) {
    mpz_t c, m, n;
    mpz_init(c);
    mpz_init(m);
    mpz_init(n);

    mpz_import(c, cipher_in_len, 1, 1, 0, 0, cipher_in);
    mpz_import(m, msg_len, 1, 1, 0, 0, msg);
    mpz_import(n, N_SIZE, 1, 1, 0, 0, pk->n);

    mpz_mul(n, n, n);
    mpz_powm(c, c, m, n);
    
    size_t mc = N_SIZE + N_SIZE;
    uint8_t c_out[N_SIZE + N_SIZE];
    mpz_export(c_out, &mc, 1, 1, 0, 0, c);
    (*cipher) = (uint8_t *)malloc(mc);
    *cipher_len = mc;
    memcpy(*cipher, c_out, mc);

    mpz_clear(c);
    mpz_clear(m);
    mpz_clear(n);
}
