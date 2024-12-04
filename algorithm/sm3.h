#ifndef _SM3_H_
#define _SM3_H_

#include "archer.h"

void sm3(const uint8_t *content, const size_t content_len, Hash32 *hash);

#endif
