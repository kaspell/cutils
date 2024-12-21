#ifndef dynarr_h
#define dynarr_h

#include "types.h"


struct ByteArr {
        u64 size;
        u64 cpty;
        u8 *data;
};


void by_push(struct ByteArr *arr, u8 e);

#endif