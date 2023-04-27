#include <stdio.h>
#include "bf.h"
#include "city.h"

#define N_HASHES 5

struct BloomFilter {
    uint64_t salts[N_HASHES];
    uint32_t n_keys;
    uint32_t n_hits;
    uint32_t n_misses;
    uint32_t n_bits_examined;
    BitVector *filter;
};

static uint64_t default_salts[] = {
    0x5adf08ae86d36f21 ,
    0x419d292ea2ffd49e ,
    0x50d8bb08de3818df ,
    0x272347aea4045dd5 ,
    0x7c8e16f768811a21
};

BloomFilter *bf_create(uint32_t size) {
    BloomFilter *bf = (BloomFilter *) malloc(sizeof(BloomFilter));
    if (bf) {
        bf->n_keys = bf->n_hits = bf->n_misses = bf->n_bits_examined = 0;
        for (int i = 0; i < N_HASHES; i++) {
            bf->salts[i] = default_salts[i];
        }
        bf->filter = bv_create(size);
        if (bf->filter == NULL) {
            free(bf);
            bf = NULL;
        }
    }
    return bf;
}

void bf_delete(BloomFilter **bf) {
    bv_delete(&(*bf)->filter);
    free(*bf);
    *bf = NULL;
    return;
}

uint32_t bf_size(BloomFilter *bf) {
    return bv_length(bf->filter);
}

void bf_insert(BloomFilter *bf, char *oldspeak) {
    uint64_t h_ind;
    bf->n_keys += 1;
    for (int i = 0; i < N_HASHES; i++) {
        h_ind = hash(bf->salts[i], oldspeak);
        h_ind %= bv_length(bf->filter);
        bv_set_bit(bf->filter, h_ind);
    }
    return;
}

bool bf_probe(BloomFilter *bf, char *oldspeak) {
    uint64_t h_ind;
    for (int i = 0; i < N_HASHES; i++) {
        h_ind = hash(bf->salts[i], oldspeak);
        h_ind %= bv_length(bf->filter);
        bf->n_bits_examined += 1;
        if (bv_get_bit(bf->filter, h_ind) == 0) {
            bf->n_misses += 1;
            return false;
        }
    }
    bf->n_hits += 1;
    return true;
}

uint32_t bf_count(BloomFilter *bf) {
    uint32_t count = 0, length;
    if (bv_length(bf->filter) < 64) {
        length = 1;
    } else {
        length = (bv_length(bf->filter) + 63)/64;
    }
    for (uint32_t i = 0; i < length; i += 1) {
        for (uint32_t j = 0; j < 64; j += 1) {
            if (bv_get_bit(bf->filter, (64*i)+j) == 1) {
                count += 1;
            }
        }
    }
    return count;
}

void bf_print(BloomFilter *bf) {
    printf("salts:\n");
    for (int i = 0; i < N_HASHES; i++) {
        printf("%lu\n", bf->salts[i]);
    }
    printf("\n");
    printf("n_keys: %u\n", bf->n_keys);
    printf("n_hits: %u\n", bf->n_hits);
    printf("n_misses: %u\n", bf->n_misses);
    printf("n_bits_examined: %u\n", bf->n_bits_examined);
    bv_print(bf->filter);
    return;
}

void bf_stats(BloomFilter *bf, uint32_t *nk, uint32_t *nh, uint32_t *nm, uint32_t *ne) {
    *nk = bf->n_keys;
    *nh = bf->n_hits;
    *nm = bf->n_misses;
    *ne = bf->n_bits_examined;
    return;
}
