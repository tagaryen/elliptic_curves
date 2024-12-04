#ifndef _EC_POINT_H_
#define _EC_POINT_H_

#include "archer.h"

void ec_point_mul(mpz_t x, mpz_t y, mpz_t d, mpz_t p, mpz_t a, mpz_t b, mpz_t gx, mpz_t gy);
void ec_point_add(mpz_t x, mpz_t y, mpz_t x1, mpz_t y1, mpz_t x2, mpz_t y2, mpz_t p);
void ec_random_k(uint8_t *k, uint16_t seed);

#endif
