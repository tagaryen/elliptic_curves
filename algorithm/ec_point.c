#include "ec_point.h"

void ec_point_mul(mpz_t x, mpz_t y, mpz_t d, mpz_t p, mpz_t a, mpz_t b, mpz_t gx, mpz_t gy) {
    // char *bits = mpz_get_str(NULL, 2, d);
    // uint32_t len = strlen(bits);

    uint32_t len = mpz_sizeinbase(d, 2);

    mpz_t inv, k, tx, ty;
    mpz_init(inv);
    mpz_init(k);
    mpz_init_set(tx, gx);
    mpz_init_set(ty, gy);
    // for(int i = 1; i < len; i++) {
    
    for(int i = len - 2; i >= 0; --i) {

        /**double**/ 
        mpz_mul_ui(inv, ty, 2);
        mpz_invert(inv, inv, p);
        mpz_mul(k, tx, tx);
        mpz_mul_ui(k, k, 3);
        mpz_add(k, k, a);
        mpz_mul(k, k, inv);
        mpz_mod(k, k, p);
        //x
        mpz_pow_ui(x, k, 2);
        mpz_sub(x, x, tx);
        mpz_sub(x, x, tx);
        mpz_mod(x, x, p);
        //y
        mpz_sub(y, tx, x);
        mpz_mul(y, y, k);
        mpz_sub(y, y, ty);
        mpz_mod(y, y, p);
        
        mpz_set(tx, x);
        mpz_set(ty, y);
        /**add**/
        // if(bits[i] == '1') {
        if(mpz_tstbit(d, i)) {

            mpz_sub(inv, tx, gx);
            mpz_invert(inv, inv, p);
            mpz_sub(k, ty, gy);
            mpz_mul(k, k, inv);
            mpz_mod(k, k, p);

            mpz_pow_ui(x, k, 2);
            mpz_sub(x, x, tx);
            mpz_sub(x, x, gx);
            mpz_mod(x, x, p);

            mpz_sub(y, tx, x);
            mpz_mul(y, y, k);
            mpz_sub(y, y, ty);
            mpz_mod(y, y, p);
            
            mpz_set(tx, x);
            mpz_set(ty, y);
        }
    }
    mpz_clear(inv);
    mpz_clear(k);
    mpz_clear(tx);
    mpz_clear(ty);
}

void ec_point_add(mpz_t x, mpz_t y, mpz_t x1, mpz_t y1, mpz_t x2, mpz_t y2, mpz_t p) {
    mpz_t t, k;
    mpz_init(t);
    mpz_init(k);

    mpz_sub(t, x1, x2);
    mpz_invert(t, t, p);
    mpz_sub(k, y1, y2);
    mpz_mul(k, k, t);
    mpz_mod(k, k, p);

    mpz_pow_ui(x, k, 2);
    mpz_sub(x, x, x1);
    mpz_sub(x, x, x2);
    mpz_mod(x, x, p);

    mpz_sub(y, x1, x);
    mpz_mul(y, y, k);
    mpz_sub(y, y, y1);
    mpz_mod(y, y, p);

    mpz_clear(k);
    mpz_clear(t);
}

void ec_random_k(uint8_t *k, uint16_t seed) {
    uint8_t r[32];
    for(int i = 0; i < 32; i++) {
        if(!r[i]) {
            r[i] = (i+3) * (seed + i);
        } else {
            r[i] = r[i] * (seed + i);
        }
    }
    memcpy(k, r, 32);
}