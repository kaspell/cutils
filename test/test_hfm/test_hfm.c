#include <assert.h>
#include <stdio.h>

#include "hfm.h"

#define DEBUG

int
beq(unsigned char *s, unsigned char *t, unsigned long long len)
{
        for (unsigned long long i=0; i<len; i++)
                if (s[i] != t[i])
                        return 0;
        return 1;
}

void
test_empty()
{
        u8 bytes[0] = {};
        for (size_t i=0; i<0; i++)
                bytes[i] = (u8)i;
        u8 *encoded = NULL;
        u8 *decoded = NULL;
        assert(encoded == NULL);
        assert(decoded == NULL);
        u64 elen = hfmenc(bytes, 0, &encoded);
        assert(elen == 9 + 256);
        u64 dlen = hfmdec(encoded, &decoded);
        assert(dlen == 0);
        assert(beq(bytes, decoded, 0));
}

void
test_zero()
{
        u8 bytes[1] = { 0 };
        for (size_t i=0; i<1; i++)
                bytes[i] = (u8)i;
        u8 *encoded = NULL;
        u8 *decoded = NULL;
        assert(encoded == NULL);
        assert(decoded == NULL);
        u64 elen = hfmenc(bytes, 1, &encoded);
        assert(elen == 9 + 256);
        u64 dlen = hfmdec(encoded, &decoded);
        assert(dlen == 1);
        assert(beq(bytes, decoded, 1));
}

void
test_two()
{
        u8 bytes[3] = { 'a', 'b', 'c' };
        for (size_t i=0; i<3; i++)
                bytes[i] = (u8)i;
        u8 *encoded = NULL;
        u8 *decoded = NULL;
        assert(encoded == NULL);
        assert(decoded == NULL);
        u64 elen = hfmenc(bytes, 3, &encoded);
        assert(elen == 10 + 256 + 1);
        u64 dlen = hfmdec(encoded, &decoded);
        assert(dlen == 3);
        assert(beq(bytes, decoded, 3));
}

void
test_bytes_half()
{
        u8 bytes[127] = { 0 };
        for (size_t i=0; i<127; i++)
                bytes[i] = (u8)i;
        u8 *encoded = NULL;
        u8 *decoded = NULL;
        assert(encoded == NULL);
        assert(decoded == NULL);
        u64 elen = hfmenc(bytes, 127, &encoded);
        assert(elen >= 10 + 256 + 111);
        u64 dlen = hfmdec(encoded, &decoded);
        assert(dlen == 127);
        assert(beq(bytes, decoded, 127));
}

void
test_bytes()
{
        u8 bytes[256] = { 0 };
        for (size_t i=0; i<256; i++)
                bytes[i] = (u8)i;
        u8 *encoded = NULL;
        u8 *decoded = NULL;
        assert(encoded == NULL);
        assert(decoded == NULL);
        u64 elen = hfmenc(bytes, 256, &encoded);
        assert(elen >= 10 + 256);
        u64 dlen = hfmdec(encoded, &decoded);
        assert(dlen == 256);
        assert(beq(bytes, decoded, 256));
}

void
test_double_bytes()
{
        u8 bytes[512] = { 0 };
        for (size_t i=0; i<512; i++)
                bytes[i] = (u8)i;
        u8 *encoded = NULL;
        u8 *decoded = NULL;
        assert(encoded == NULL);
        assert(decoded == NULL);
        u64 elen = hfmenc(bytes, 512, &encoded);
        assert(elen >= 10 + 256);
        u64 dlen = hfmdec(encoded, &decoded);
        assert(dlen == 512);
        assert(beq(bytes, decoded, 512));
}

void
test_arbitrary_bytes()
{
        u8 bytes[1112] = { 0 };
        for (size_t i=0; i<1112; i++)
                bytes[i] = (u8)i;
        u8 *encoded = NULL;
        u8 *decoded = NULL;
        assert(encoded == NULL);
        assert(decoded == NULL);
        u64 elen = hfmenc(bytes, 1112, &encoded);
        assert(elen != 0);
        u64 dlen = hfmdec(encoded, &decoded);
        assert(dlen == 1112);
        assert(beq(bytes, decoded, 1112));
}

void
test_alphabet()
{
        u8 bytes[26] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
        u8 *encoded = NULL;
        u8 *decoded = NULL;
        assert(encoded == NULL);
        assert(decoded == NULL);
        u64 elen = hfmenc(bytes, 26, &encoded);
        assert(elen != 0);
        u64 dlen = hfmdec(encoded, &decoded);
        assert(dlen == 26);
        assert(beq(bytes, decoded, 26));
}

void
test_text()
{
        u8 bytes[] = "but as they turned up all the same, that did not matter. Many people from other parts of the Shire were also asked; and there were even a few from outside the borders. Bilbo met the guests (and additions) at the new white gate in person. He gave away presents to all and sundry – the latter were those who went out again by a back way and came in again by the gate. Hobbits give presents to other people on their own birthdays. Not very expensive ones, as a rule, and not so lavishly as on this occasion; but it was not a bad system. Actually in Hobbiton and Bywater every day in the year was somebody’s birthday, so that every hobbit in those parts had a fair chance of at least one present at least once a week. But they never got tired of them. On this occasion the presents were unusually good. The hobbit-children were so excited that for a while they almost forgot about eating. There were toys the like of which they had never seen before, all beautiful and some obviously magi- cal. Many of them had indeed been ordered a year before, and had come all the way from the Mountain and from Dale, and were of real dwarf-make. When every guest had been welcomed and was finally inside the gate, there were songs, dances, music, games, and, of course, food and drink. There were three official meals: lunch, tea, and dinner (or supper). But lunch and tea were marked chiefly by the fact that at those times all the guests were sitting down and eating together. At other times there were merely lots of people eating and drinking – continuously from elevenses until six-thirty, when the fireworks started. The fireworks were by Gandalf: they were not only brought by him, but designed and made by him; and the special effects, set pieces, and flights of rockets were let off by him. But there was also a generous distribution of squibs, crackers, backarappers, sparklers, torches, dwarf-candles, elf- fountains, goblin-barkers and thunder-claps. They were all superb. The art of Gandalf improved with age. There were rockets like a flight of scintillating birds singing with sweet voices. There were green trees with trunks of dark";
        u8 *encoded = NULL;
        u8 *decoded = NULL;
        assert(encoded == NULL);
        assert(decoded == NULL);
        u64 elen = hfmenc(bytes, 752, &encoded);
        assert(elen != 0);
        u64 dlen = hfmdec(encoded, &decoded);
        assert(dlen == 752);
        assert(beq(bytes, decoded, 752));
}

int
main()
{
        test_empty();
        test_zero();
        test_two();
        test_bytes_half();
        test_bytes();
        test_double_bytes();
        test_arbitrary_bytes();
        test_alphabet();
        test_text();
}