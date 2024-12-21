#ifndef base64_h
#define base64_h

#include "types.h"


u8 *b64decode(u8 *enc);
u8 *b64encode(u8 *src);

#endif