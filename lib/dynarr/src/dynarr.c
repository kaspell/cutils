#include <stdlib.h>

#include "dynarr.h"


void
by_push(struct ByteArr *ay, u8 e)
{
        if (ay->cpty < ay->size+1) {
                ay->cpty = ay->cpty < 8 ? 8 : 2*ay->cpty;
                void *pdata = realloc(ay->data, ay->cpty*sizeof(u8));
                if (!pdata)
                        exit(EXIT_FAILURE);
                ay->data = pdata;
        }
        ay->data[ay->size] = e;
        ++ay->size;
}