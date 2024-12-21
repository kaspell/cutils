#ifndef huff_h
#define huff_h

#include "types.h"

u64 hfmenc(u8 *src, u64 srclen, u8 **encoded);
u64 hfmdec(u8 *encoded, u8 **decoded);

#endif