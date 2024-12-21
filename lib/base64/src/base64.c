#include <stdlib.h>

#include "base64.h"
#include "dynarr.h"


static u8 B64_BTREE[80]  = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

// + as char == 43
// / as char == 47
// 0 as char == 48
// 9 as char == 57
// A as char == 65
// Z as char == 90
// a as char == 97
// z as char == 122
//                         +               /   0   1   2   3   4   5   6   7   8   9                               A  B  C  D  E  F  G  H  I  J  K   L   M   N   O   P   Q   R   S   T   U   V   W   X   Y   Z                           a   b   c   d   e   f   g   h   i   j   k   l   m   n   o   p   q   r   s   t   u   v   w   x   y   z
static u8 B64_POS[80] = {62, 64, 64, 64, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64, 64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64, 64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51};


u8 *
b64encode(u8 *src)
{
        struct ByteArr arr = { .data = NULL, .size = 0, .cpty = 0 };
        u8 btree_idx = 0;
        u8 curr = 0;

        while ((curr = *src++) != '\0') {
                for (int i=7; i>=0; --i) {
                        btree_idx = 2*btree_idx + 1;
                        if ((((u8)1 << i) & curr) > 0)
                                ++btree_idx;
                        if (btree_idx > 62) {
                                by_push(&arr, B64_BTREE[btree_idx - 63]);
                                btree_idx = 0;
                        }
                }
        }

        // Add padding bits
        if (btree_idx != 0) {
                int twice = btree_idx < 15;
                while (btree_idx < 63)
                        btree_idx = 2*btree_idx + 1;
                by_push(&arr, B64_BTREE[btree_idx - 63]);
                by_push(&arr, '=');
                if (twice)
                        by_push(&arr, '=');
        }
        by_push(&arr, '\0');
        return arr.data;
}


u8 *
b64decode(u8 *enc)
{
        struct ByteArr arr = { .data = NULL, .size = 0, .cpty = 0 };
        u8 byte = 0;
        u8 bit_idx = 7;
        u8 curr = 0;
        while ((curr = *enc++) != '\0') {
                if (curr == '=')
                        break;
                u8 bits = B64_POS[curr - 43];
                for (int i=5; i>=0; --i) {
                        if ((((u8)1 << i) & bits) > 0)
                                byte |= 1 << bit_idx;
                        if (bit_idx == 0) {
                                by_push(&arr, byte);
                                byte = 0;
                                bit_idx = 8;
                        }
                        --bit_idx;
                }
        }
        by_push(&arr, '\0');
        return arr.data;
}