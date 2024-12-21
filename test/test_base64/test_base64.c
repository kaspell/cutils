#include <assert.h>
#include <stdio.h>

#include "base64.h"


int
streq(unsigned char *s, unsigned char *t)
{
        unsigned char a = 0, b = 0;
        while ((a != '\0') && (b != '\0') && (a = *s++) == (b = *t++))
                ;
        return (a == b) && (b == '\0');
}

void
test_bytes_encoding_decoding()
{
        unsigned char bytes[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};
        unsigned char bytes_b64enc[] = "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4vMDEyMzQ1Njc4OTo7PD0+P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWltcXV5fYGFiY2RlZmdoaWprbG1ub3BxcnN0dXZ3eHl6e3x9fn+AgYKDhIWGh4iJiouMjY6PkJGSk5SVlpeYmZqbnJ2en6ChoqOkpaanqKmqq6ytrq+wsbKztLW2t7i5uru8vb6/wMHCw8TFxsfIycrLzM3Oz9DR0tPU1dbX2Nna29zd3t/g4eLj5OXm5+jp6uvs7e7v8PHy8/T19vf4+fr7/P3+/w==";
        unsigned char *encoded = b64encode(bytes);
        unsigned char *decoded = b64decode(encoded);
        assert(streq(encoded, bytes_b64enc));
        assert(streq(decoded, bytes));

}

void
test_padding()
{
        unsigned char *a = b64encode("");
        unsigned char *b = b64encode("a");
        unsigned char *c = b64encode("%g");
        unsigned char *d = b64encode("gcc");

        assert(streq(a, ""));
        assert(streq(b, "YQ=="));
        assert(streq(c, "JWc="));
        assert(streq(d, "Z2Nj"));

        assert(streq(b64decode(a), ""));
        assert(streq(b64decode(b), "a"));
        assert(streq(b64decode(c), "%g"));
        assert(streq(b64decode(d), "gcc"));
}

void
test_shakespeare()
{
        unsigned char scene[] = "An earnest conjuration from the king,\nAs England was his faithful tributary,\nAs love between them like the palm might flourish,\nAs peace should stiff her wheaten garland wear\nAnd stand a comma 'tween their amities,\nAnd many such-like 'As'es of great charge,\nThat, on the view and knowing of these contents,\nWithout debatement further, more or less,\nHe should the bearers put to sudden death,\nNot shriving-time allow'd.";
        unsigned char scene_enc[] = "QW4gZWFybmVzdCBjb25qdXJhdGlvbiBmcm9tIHRoZSBraW5nLApBcyBFbmdsYW5kIHdhcyBoaXMgZmFpdGhmdWwgdHJpYnV0YXJ5LApBcyBsb3ZlIGJldHdlZW4gdGhlbSBsaWtlIHRoZSBwYWxtIG1pZ2h0IGZsb3VyaXNoLApBcyBwZWFjZSBzaG91bGQgc3RpZmYgaGVyIHdoZWF0ZW4gZ2FybGFuZCB3ZWFyCkFuZCBzdGFuZCBhIGNvbW1hICd0d2VlbiB0aGVpciBhbWl0aWVzLApBbmQgbWFueSBzdWNoLWxpa2UgJ0FzJ2VzIG9mIGdyZWF0IGNoYXJnZSwKVGhhdCwgb24gdGhlIHZpZXcgYW5kIGtub3dpbmcgb2YgdGhlc2UgY29udGVudHMsCldpdGhvdXQgZGViYXRlbWVudCBmdXJ0aGVyLCBtb3JlIG9yIGxlc3MsCkhlIHNob3VsZCB0aGUgYmVhcmVycyBwdXQgdG8gc3VkZGVuIGRlYXRoLApOb3Qgc2hyaXZpbmctdGltZSBhbGxvdydkLg==";
        assert(streq(b64encode(scene), scene_enc));
        assert(streq(b64decode(scene_enc), scene));
}

int
main()
{
        test_bytes_encoding_decoding();
        test_padding();
        test_shakespeare();
}