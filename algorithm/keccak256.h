#ifndef _KECCAK256_H_
#define _KECCAK256_H_

#include "archer.h"

void keccak256(const uint8_t *content, const size_t content_len, Hash32 *hash);

#endif