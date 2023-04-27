#include <stdio.h>
#include <stdlib.h>
#include "city.h"
#include "ht.h"

struct HashTable {
    uint64_t salt;
    uint32_t size;
    uint32_t n_keys;
    uint32_t n_hits;
    uint32_t n_misses;
    uint32_t n_examined;
    bool mtf;
    LinkedList **lists;
};

HashTable *ht_create(uint32_t size, bool mtf) {
    HashTable *ht = (HashTable *) malloc(sizeof(HashTable));
    if (ht != NULL) {
        ht->mtf = mtf;
        ht->salt = 0x9846e4f157fe8840;
        ht->n_hits = ht->n_misses = ht->n_examined = ht->n_keys = 0;
        ht->size = size;
        ht->lists = (LinkedList **) calloc(size, sizeof(LinkedList *));
        if (!ht->lists) {
            free(ht);
            ht = NULL;
        }
    }
    return ht;
}

void ht_delete(HashTable **ht) {
    for (uint32_t i = 0; i < (*ht)->size; i += 1) {
        if ((*ht)->lists[i]) { 
            //ll_print((*ht)->lists[i]);
            ll_delete(&(*ht)->lists[i]);
        }
    }
    free((*ht)->lists);
    (*ht)->lists = NULL;
    free(*ht);
    *ht = NULL;
    return;
}

uint32_t ht_size(HashTable *ht) {
    return ht->size;
}

Node *ht_lookup(HashTable *ht, char *oldspeak) {
    uint32_t links_before, links_after, seeks;
    uint64_t h_ind = hash(ht->salt, oldspeak);
    if (ht->size > 1) {
        h_ind %= ht->size;
    } else {
        h_ind = 0;
    }
    Node *look;
    ll_stats(&seeks, &links_before);
    if (ht->lists[h_ind]) {
        look = ll_lookup(ht->lists[h_ind], oldspeak);
        if (look) {
            ht->n_hits += 1;
        } else {
            ht->n_misses += 1;
        }
    } else {
        ht->n_misses += 1;
        look = NULL;
    }
    ll_stats(&seeks, &links_after);
    ht->n_examined += (links_after - links_before);
    //printf("Looking for '%s', %u counted, %u length of LL\n", oldspeak, (links_after - links_before), ll_length(ht->lists[h_ind]));
    //printf("printing h_ind: %lu\n", h_ind);
    //ll_print(ht->lists[h_ind]);
    return look;
}

void ht_insert(HashTable *ht, char *oldspeak, char *newspeak) {
    //printf("looking up in insert\n");
    uint64_t h_ind;
    h_ind = hash(ht->salt, oldspeak);
    if (ht->size > 1) {
        h_ind %= ht->size;
    } else {
        h_ind = 0;
    }
    if (!ht->lists[h_ind]) {                            //checks if the linked list doesn't exist
        ht->lists[h_ind] = ll_create(ht->mtf);
    }
    if (!ll_lookup(ht->lists[h_ind], oldspeak)) {
        ht->n_keys += 1;
    }
    ll_insert(ht->lists[h_ind], oldspeak, newspeak);
    return;
}

uint32_t ht_count(HashTable *ht) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < ht->size; i += 1) {
        if (ht->lists[i]) {
            count += 1;
        }
    }
    return count;
}

void ht_print(HashTable *ht) {
    printf("printing hashtable:\n\n");
    printf("salt: %lu\n", ht->salt);
    printf("size: %u\n", ht->size);
    printf("n_keys: %u\n", ht->n_keys);
    printf("n_hits: %u\n", ht->n_hits);
    printf("n_misses: %u\n", ht->n_misses);
    printf("n_examined: %u\n", ht->n_examined);
    printf("mtf: %s\n", ht->mtf ? "true" : "false");
    for (uint32_t i = 0; i < ht->size; i += 1) {
        if (ht->lists[i]) {
            printf("i:%u\n", i);
            ll_print(ht->lists[i]);
        }
    }
    return;
}

void ht_stats(HashTable *ht, uint32_t *nk, uint32_t *nh, uint32_t *nm, uint32_t *ne) {
    *nk = ht->n_keys;
    *nh = ht->n_hits;
    *nm = ht->n_misses;
    *ne = ht->n_examined;
    return;
}
