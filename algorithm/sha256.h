#ifndef _SHA256_H_
#define _SHA256_H_

#include "archer.h"

void sha256(const uint8_t *content, const size_t content_len, Hash32 *hash);

#endif