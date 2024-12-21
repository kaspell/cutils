#include <stdio.h>
#include <stdlib.h>

#include "dynarr.h"
#include "hfm.h"


struct TreeNode {
        u8 byte[2];
        u64 cnt;
        struct TreeNode *left;
        struct TreeNode *right;
};

u64
count_bits(u64 counts[256], u8 cdlens[256])
{
        u64 bitcnt = 0;
        for (size_t i=0; i<256; i++)
                if (counts[i] > 0)
                        bitcnt += counts[i] * (u64)cdlens[i];
        return bitcnt;
}

void
count_bytes(u8 *bytes, u64 len, u64 counts[256])
{
        u64 i = 0;
        while (i++ < len)
                ++counts[*bytes++];
}

// Produce a header that's embedded in the encoded sequence
// The header contains the byte counts that were observed in the original data
// Using these counts, we can recreate the Huffman code and proceed with decoding
u64
produce_hdr(struct ByteArr *encd, u64 counts[256])
{
        u64 max = 0;
        for (size_t i=0; i<256; i++)
                if (counts[i] > max)
                        max = counts[i];
        if (((u64)((u8)max)) == max)
                goto tiny_hdr;
        if (((u64)((u16)max)) == max)
                goto small_hdr;
        if (((u64)((u32)max)) == max)
                goto medium_hdr;
        goto large_hdr;

tiny_hdr:
        by_push(encd, (u8)0);
        for (size_t i=0; i<256; i++)
                by_push(encd, (u8)counts[i]);
        return 257;

small_hdr:
        by_push(encd, (u8)1);
        for (size_t i=0; i<256; i++) {
                u16 tmp = (u16) counts[i];
                u8 *bytes = (u8 *)&tmp;
                for (size_t j=0; j<2; j++)
                        by_push(encd, bytes[j]);
        }
        return 513;

medium_hdr:
        by_push(encd, (u8)2);
        for (size_t i=0; i<256; i++) {
                u32 tmp = (u32) counts[i];
                u8 *bytes = (u8 *)&tmp;
                for (size_t j=0; j<4; j++)
                        by_push(encd, bytes[j]);
        }
        return 1025;

large_hdr:
        by_push(encd, (u8)3);
        for (size_t i=0; i<256; i++) {
                u8 *bytes = (u8 *)&counts[i];
                for (size_t j=0; j<8; j++)
                        by_push(encd, bytes[j]);
        }
        return 2049;
}

// Recreate the observed byte counts in the original data by reading the header
void
read_hdr(u64 counts[256], u8 *header, u8 htype)
{
        switch (htype) {
                case 0: { goto tiny_hdr; break; }
                case 1: { goto small_hdr; break; }
                case 2: { goto medium_hdr; break; }
                default: { goto large_hdr; break; }
        }

tiny_hdr:
        for (size_t i=0; i<256; ++i)
                counts[i] = header[i];
        return;

small_hdr:
        for (size_t i=0; i<256; ++i) {
                u16 count = 0;
                u8 *bytes = (u8 *)&header[2*i];
                for (size_t j=0; j<2; j++)
                        ((u8 *)&count)[j] = bytes[j];
                counts[i] = (u64) count;
        }
        return;

medium_hdr:
        for (size_t i=0; i<256; ++i) {
                u32 count = 0;
                u8 *bytes = (u8 *)&header[4*i];
                for (size_t j=0; j<4; j++)
                        ((u8 *)&count)[j] = bytes[j];
                counts[i] = (u64) count;
        }
        return;

large_hdr:
        for (size_t i=0; i<256; ++i) {
                u64 count = 0;
                u8 *bytes = (u8 *)&header[8*i];
                for (size_t j=0; j<8; j++)
                        ((u8 *)&count)[j] = bytes[j];
                counts[i] = count;
        }
        return;
}

// Given a Huffman tree, produce the code for each observed byte in the data
void
produce_codes(struct TreeNode *node, u16 code, u8 idx, u16 codes[256], u8 cdlens[256])
{
        if (node->byte[1] == 1) {
                codes[node->byte[0]] = code;
                cdlens[node->byte[0]] = idx;
#ifdef DEBUG
                printf("produce_codes: byte %u has %u-length code: ", node->byte[0], idx);
                for (int x=15; x>=0; x--) {
                        printf("%u", (((u16)1 << x) & code) > 0 ? 1 : 0);
                }
                printf("\n");
#endif
        } else {
                if (node->left != NULL)
                        produce_codes(node->left, code | ((u16)1 << idx), idx+1, codes, cdlens);
                if (node->right != NULL)
                        produce_codes(node->right, code & ~((u16)1 << idx), idx+1, codes, cdlens);
        }
}

// Insertion sort
void
insort(struct TreeNode *nodes[256], u64 len)
{
        u64 j;
        struct TreeNode *ptn = NULL;
        for (u64 i=1; i<len; i++) {
                ptn = nodes[i];
                for (j = i; j>0 && nodes[j-1]->cnt < ptn->cnt; j--)
                        nodes[j] = nodes[j-1];
                nodes[j] = ptn;
        }
}

void
print_inorder(struct TreeNode *node)
{
        if (node->left != NULL)
                print_inorder(node->left);
        if (node->byte[1] == 1) {
                for (int z=7; z>=0; z--)
                        printf("%u", (((u8)1 << z) & node->byte[0]) > 0 ? 1 : 0);
                printf("\n");
        }
        if (node->right != NULL)
                print_inorder(node->right);
}

// Given the counts of observed bytes in the data, produce the Huffman tree
struct TreeNode *
produce_tree(u64 bycnts[256])
{
        struct TreeNode *nodes[256] = { NULL };
        size_t i=0, j=0;
        for (i=0; i<256; i++) {
                if (bycnts[i] == 0)
                        continue;
                struct TreeNode *ptn = (struct TreeNode *) malloc(sizeof(struct TreeNode));
                ptn->byte[0] = (u8)i;
                ptn->byte[1] = (u8)1;
                ptn->cnt = bycnts[(u8)i];
                ptn->left = NULL;
                ptn->right = NULL;
                nodes[j++] = ptn;
        }
        insort(nodes, j);
#ifdef DEBUG
        for (size_t x=0; x<j; x++) {
                if (nodes[x] != NULL) {
                        printf("produce_tree: nodelist[%zu] carries byte: ", x);
                        for (int z=7; z>=0; z--) {
                                printf("%u", (((u8)1 << z) & nodes[x]->byte[0]) > 0 ? 1 : 0);
                        }
                        printf(" (%u) with count: %llu\n", nodes[x]->byte[0], nodes[x]->cnt);
                } else
                        printf("produce_tree: nodelist[%zu] carries an internal node\n", x);
        }
#endif
        for (i=j-1; i>0; i--) {
                struct TreeNode *n = nodes[i];
                nodes[i] = NULL;
                struct TreeNode *m = nodes[i-1];
                u64 tocnt = n->cnt + m->cnt;
                struct TreeNode *ppar = (struct TreeNode *) malloc(sizeof(struct TreeNode));
                ppar->byte[0] = 0;
                ppar->byte[1] = 0;
                ppar->cnt = tocnt;
                ppar->right = n;
                ppar->left = m;
                ssize_t k = (ssize_t)i - 1;
                nodes[k] = ppar;
                while ((k-1 >= 0) && (nodes[k-1]->cnt < tocnt)) {
                        struct TreeNode *tmp = nodes[k-1];
                        nodes[k-1] = nodes[k];
                        nodes[k] = tmp;
                        --k;
                }
        }
#ifdef DEBUG
        printf("produce_tree: inorder traversal of tree nodes:\n");
        print_inorder(nodes[0]);
#endif
        return nodes[0];
}

// Decode a byte sequence
// Return the number of bytes present in the decoded data
u64
hfmdec(u8 *encoded, u8 **decoded)
{
        struct ByteArr ret = { .data = NULL, .size = 0, .cpty = 0 };
        u64 retlen = 0;
        if (encoded == NULL) {
#ifdef DEBUG
                printf("hfmdec: encoding was null, returning\n");
#endif
                *decoded = NULL;
                return 0;
        }
        u8 htype = encoded[8];
        u16 hdrlen = 2048;
        switch (htype) {
                case 0: { hdrlen = 256; break; }
                case 1: { hdrlen = 512; break; }
                case 2: { hdrlen = 1024; break; }
                default: { hdrlen = 2048; break; }
        }
        u8 padding_bit_cnt = encoded[hdrlen+9];
        u8 *content = &encoded[hdrlen+10];

        u64 len = 0;
        u8 *lbytes = (u8 *)&encoded[0];
        for (u8 i=0; i<8; i++)
                ((u8 *)&len)[i] = lbytes[i];

        u64 counts[256] = { 0 };
        u8 *header = (u8 *)&encoded[9];
        read_hdr(counts, header, htype);

        if (len == 0) {
#ifdef DEBUG
                printf("hfmdec: encoding length was 0 or 1, pushing the seen byte\n");
#endif
                for (size_t t=0; t<256; t++)
                        if (counts[t] == 1) {
                                by_push(&ret, (u8)t);
                                *decoded = ret.data;
                                return 1;
                        }
                return 0;
        }

        struct TreeNode *root = produce_tree(counts);
        struct TreeNode *curr = root;

        u64 bit_idx = 8;
        size_t offset = 0;
        if (padding_bit_cnt == 0)
                offset = 1;
        for (size_t i=0; i<len+offset; i++) {
#ifdef DEBUG
                printf("hfmdec: saw code byte: ");
                for (int x=7; x>=0; x--)
                        printf("%u", (((u8)1 << x) & content[i]) > 0 ? 1 : 0);
                printf("\n");
#endif
                for (ssize_t j=7; j>=0; j--) {
                        if (bit_idx >= 8*(len + 1) - (u64)padding_bit_cnt)
                                goto done;
                        if ((((u16)1 << j) & content[i]) > 0)
                                curr = curr->left;
                        else
                                curr = curr->right;
                        if (curr != NULL && (curr->byte[1] == 1)) {
                                by_push(&ret, curr->byte[0]);
#ifdef DEBUG
                                printf("hfmdec: decoded byte: ");
                                for (int x=7; x>=0; x--)
                                        printf("%u", (((u8)1 << x) & curr->byte[0]) > 0 ? 1 : 0);
                                printf("\n");
#endif
                                ++retlen;
                                curr = root;
                        }
                        ++bit_idx;
                }
        }
done:
        *decoded = ret.data;
#ifdef DEBUG
        printf("hfmdec: decoded total of %llu bytes\n", retlen);
        printf("hfmdec: saw header size: %hu\n", hdrlen);
        printf("hfmdec: saw encoded content length: %llu\n", len);
        printf("hfmdec: saw padding bit count: %u\n", padding_bit_cnt);
#endif
        return retlen;
}

// Encode a byte sequence
// Return the number of bytes present in the encoding
u64
hfmenc(u8 *src, u64 srclen, u8 **encoded)
{
        u64 byte_cnt = 0;
        u64 content_byte_cnt = 0;
        struct ByteArr enc_bytes = { .data = NULL, .size = 0, .cpty = 0 };
        u64 counts[256] = { 0 };
        count_bytes(src, srclen, counts);

#ifdef DEBUG
        for (size_t x=0; x<256; x++) {
                printf("count for byte ");
                for (int z=7; z>=0; z--)
                        printf("%u", (((u8)1 << z) & (u8)x) > 0 ? 1 : 0);
                printf(" (%zu) equals %llu\n", x, counts[x]);
        }
#endif

        for (size_t i=0; i<8; i++) {
                by_push(&enc_bytes, 0);
                ++byte_cnt;
        }
        u64 hdrbytes = produce_hdr(&enc_bytes, counts);
        byte_cnt += hdrbytes;
        u64 total = 0;
        for (size_t i=0; i<256; i++)
                total += counts[i];

        if (srclen == 0)
                goto done;
        if (total == 1)
                goto done;

        struct TreeNode *root = produce_tree(counts);
        u8 cdlens[256] = { 0 };
        u16 codes[256] = { 0 };
        produce_codes(root, 0, 0, codes, cdlens);

        u64 bitcnt = count_bits(counts, cdlens);
        u64 bytecnt = bitcnt/8 + (bitcnt % 8 != 0);
        u8 pbcnt = (u8)(8*bytecnt - bitcnt);
        by_push(&enc_bytes, pbcnt);
        ++byte_cnt;

#ifdef DEBUG
        printf("hfmenc: writing padding bit count: ");
        for (int x=7; x>=0; x--)
                printf("%u", (((u8)1 << x) & pbcnt) > 0 ? 1 : 0);
        printf("\n");
#endif

        size_t bit_idx = 0;
        u8 curr_byte = 0;

        for (size_t i=0; i<srclen; i++) {
                for (size_t j=0; j<cdlens[src[i]]; j++) {
                        if ((((u16)1 << j) & codes[src[i]]) > 0)
                                curr_byte |= (u8)1 << (7 - bit_idx);
                        ++bit_idx;
                        if (bit_idx == 8) {
                                by_push(&enc_bytes, curr_byte);
#ifdef DEBUG
                                printf("hfmenc: wrote content byte: ");
                                for (int x=7; x>=0; x--) {
                                        printf("%u", (((u8)1 << x) & curr_byte) > 0 ? 1 : 0);
                                }
                                printf("\n");
#endif
                                ++content_byte_cnt;
                                bit_idx = 0;
                                curr_byte = 0;
                        }

                }
        }
        if ((bit_idx < 8) && (pbcnt != 0)) {
                by_push(&enc_bytes, curr_byte);
                ++content_byte_cnt;
#ifdef DEBUG
                printf("hfmenc: wrote content byte with padding: ");
                for (int x=7; x>=0; x--) {
                        printf("%u", (((u8)1 << x) & curr_byte) > 0 ? 1 : 0);
                }
                printf("\n");
#endif
        }
        byte_cnt = byte_cnt + content_byte_cnt;
#ifdef DEBUG
        printf("hfmenc: final content byte count is: %llu\n", content_byte_cnt);
        printf("hfmenc: final enc byte count is: %llu\n", byte_cnt);
        printf("hfmenc: header byte count is: %llu\n", hdrbytes);
        printf("hfmenc: padding bit count is: %u\n", pbcnt);
        printf("hfmenc: bit count is: %llu\n", bitcnt);
#endif

done:
        for (size_t i=0; i<8; i++)
                enc_bytes.data[i] = ((u8 *) &content_byte_cnt)[i];
        *encoded = enc_bytes.data;
        return enc_bytes.size;
}