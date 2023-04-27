#include <stdio.h>
#include <stdlib.h>
#include "bv.h"

struct BitVector {
    uint32_t length;
    uint64_t *vector;
};

BitVector *bv_create(uint32_t length) {
    BitVector *bv = (BitVector *) malloc(sizeof(BitVector));
    if (!bv) {
        bv = NULL;
    } else {
        bv->length = length;
        if (length < 64) {
            bv->vector = (uint64_t *)calloc(1, sizeof(uint64_t));
        } else {
            bv->vector = (uint64_t *)calloc(((length+63)/64), sizeof(uint64_t));
        }
        if (!bv->vector) {
            bv = NULL;
        }
    }
    return bv;
}

void bv_delete(BitVector **bv) {
    free((*bv)->vector);
    free(*bv);
    return;
}

uint32_t bv_length(BitVector *bv) {
    return bv->length;
}

void bv_set_bit(BitVector *bv, uint32_t i) {
    uint32_t byte_i = i / 64;
    uint32_t bit_i = i % 64;
    bv->vector[byte_i] |= 1UL << bit_i;
    return;
}

void bv_clr_bit(BitVector *bv, uint32_t i) {
    uint32_t byte_i = i / 64;
    uint32_t bit_i = i % 64;
    bv->vector[byte_i] &= ~(1UL << bit_i);
    return;
}

uint8_t bv_get_bit(BitVector *bv, uint32_t i) {
    uint32_t byte_i = i / 64;
    uint32_t bit_i = i % 64;
    uint8_t res = (uint8_t)(bv->vector[byte_i] >> bit_i) & 1U;
    return res;
}

void bv_print(BitVector *bv) {
    printf("printing bit vector:\n");
    uint32_t length;
    if (bv->length < 64) {
        length = 1;
    } else {
        length = (bv->length + 63)/64;
    }
    for (uint32_t i = 0; i < length; i += 1) {
        for (uint32_t j = 0; j < 64; j += 1) {
            printf("%lu ", (bv->vector[i] >> j) & 1U);
        }
        printf("\n");
    }
    printf("done printing bit vector.\n");
    return;
}



